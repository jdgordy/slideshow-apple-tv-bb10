#include "AirPlaySlideshow.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/ArrayDataModel>
#include <bb/cascades/LocaleHandler>
#include <bb/data/JsonDataAccess>
#include <bb/system/InvokeTargetReply>
#include <QtCore/QDir>
#include <QtCore/QUuid>
#include <iostream>
#include "BonjourBrowser.hpp"
#include "BonjourResolver.hpp"
#include "AirPlayDevice.hpp"
#include "DevicePropertiesViewer.hpp"
#include "SlideshowViewer.hpp"

// Declare namespaces
using namespace bb::cascades;
using namespace bb::system;
using namespace bb::data;
using namespace std;

// Constants
static const QString g_strAirPlayServiceType = "_airplay._tcp";
static const QString g_strDefaultConnectionTimeout = "20000";
static const QString g_strDefaultEnableScaleImages = "false";
static const QString g_strDefaultScreenSize = "720p";
static const QString g_strDefaultImageQuality = "high";

// Helper function to map from URL file extension to MIME type
static QString findMimeType(const QUrl& url)
{
    QString type;

    // Retrieve filename
    QFileInfo fileInfo(url.path());
    QString suffix = fileInfo.suffix();
    if( suffix == "jpeg" || suffix == "jpg" )
    {
        type = "image/jpeg";
    }
    else if( suffix == "png" )
    {
        type = "image/png";
    }
    else if( suffix == "gif" )
    {
        type = "image/gif";
    }
    else
    {
        type = "application/unknown";
    }

    return type;
}

// Validate from set of supported image types
static bool validateMimeType(const QString& type)
{
    if( (type == "image/jpeg") || (type == "image/jpg") || (type == "image/png") || (type == "image/gif") )
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Constructor
AirPlaySlideshow::AirPlaySlideshow(bb::cascades::Application *app) :
    QObject(app),
    m_pRecordListModel(new ArrayDataModel(this)),
    m_pFileListModel(new ArrayDataModel(this)),
    m_pDevice(NULL),
    m_pDevicePropertiesViewer(new DevicePropertiesViewer(this)),
    m_pSlideshowViewer(new SlideshowViewer(this)),
    m_pInvokeManager(new InvokeManager(this)),
    m_transition(0),
    m_pBrowser(new BonjourBrowser(this)),
    m_pResolver(new BonjourResolver(this)),
    m_pTranslator(new QTranslator(this)),
    m_pLocaleHandler(new LocaleHandler(this))
{
    // We set up the application Organization and name, this is used by QSettings
    // when saving values to the persistent store.
    QCoreApplication::setOrganizationName("JamesGordy");
    QCoreApplication::setApplicationName("AirPlaySlideshow");

    // Set default parameters
    QSettings settings;
    if( settings.value("ConnectionTimeout").isNull() )
    {
        settings.setValue("ConnectionTimeout", g_strDefaultConnectionTimeout);
    }
    if( settings.value("EnableScaleImages").isNull() )
    {
        settings.setValue("EnableScaleImages", g_strDefaultEnableScaleImages);
    }
    if( settings.value("ScreenSize").isNull() )
    {
        settings.setValue("ScreenSize", g_strDefaultScreenSize);
    }
    if( settings.value("ImageQuality").isNull() )
    {
        settings.setValue("ImageQuality", g_strDefaultImageQuality);
    }

    // Connect the invocation manager signals
    bool bResult = QObject::connect(m_pInvokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(handleInvoke(const bb::system::InvokeRequest&)));
    Q_ASSERT(bResult);

    // Connect the browser instance for replies and errors
    bResult = QObject::connect(m_pBrowser, SIGNAL(browserRecordsChanged(const QList<BonjourRecord>&)), this, SLOT(handleBrowserRecordsChanged(const QList<BonjourRecord>&)));
    Q_ASSERT(bResult);
    bResult = QObject::connect(m_pBrowser, SIGNAL(error(DNSServiceErrorType)), this, SLOT(handleError(DNSServiceErrorType)));
    Q_ASSERT(bResult);

    // Connect the resolver instance for replies and errors
    bResult = QObject::connect(m_pResolver, SIGNAL(recordResolved(const QString&, const QHostInfo&, int, const QVariantMap&)), this, SLOT(handleRecordResolved(const QString&, const QHostInfo&, int, const QVariantMap&)));
    Q_ASSERT(bResult);
    bResult = QObject::connect(m_pResolver, SIGNAL(error(DNSServiceErrorType)), this, SLOT(handleError(DNSServiceErrorType)));
    Q_ASSERT(bResult);

    // Connect us to receive system language change events
    bResult = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(handleSystemLanguageChanged()));
    Q_ASSERT(bResult);

    // Initial load
    handleSystemLanguageChanged();

    // Start browsing for services
    m_pBrowser->browseForServiceType(g_strAirPlayServiceType);

    // create scene document from main.qml asset
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument* qml = QmlDocument::create("asset:///main.qml").parent(this);
    if( !qml->hasErrors() )
    {
        // Register us
        qml->setContextProperty("mainApp", this);

        // create root object for the UI
        AbstractPane *root = qml->createRootObject<AbstractPane>();

        // set created root object as a scene
        app->setScene(root);
    }
}

// Destructor
AirPlaySlideshow::~AirPlaySlideshow()
{
}

// Set or retrieve configuration parameter
void AirPlaySlideshow::setParameter(const QString& parameter, const QString& value)
{
    // Persist the parameter
    QSettings settings;
    settings.setValue(parameter, QVariant(value));
}

// Set or retrieve configuration parameter
QString AirPlaySlideshow::getParameter(const QString& parameter, const QString& defaultValue)
{
    QSettings settings;

    // First check if the parameter exists
    if( settings.value(parameter).isNull() )
    {
        return defaultValue;
    }

    // Retrieve the parameter from the persistent store
    QString result = settings.value(parameter).toString();
    return result;
}

// Set device and trigger resolution
void AirPlaySlideshow::setDevice(const QVariantList& indexPath)
{
    // Clear old device
    if( m_pDevice )
    {
        // Release device instance
        m_pDevice->deleteLater();
        m_pDevice = NULL;

        // Emit device changed signal
        emit deviceChanged();
    }

    // Check if we have a valid index
    if( !indexPath.isEmpty() )
    {
        // Retrieve the record fields
        const QVariantMap entry = m_pRecordListModel->data(indexPath).toMap();
        QString name = entry.value("name").value<QString>();
        QString serviceType = entry.value("serviceType").value<QString>();
        QString domain = entry.value("domain").value<QString>();

        // Begin record resolution
        BonjourRecord record(name, serviceType, domain);
        m_pResolver->resolveRecord(record);
    }
}

// Add item(s) to slideshow file list
void AirPlaySlideshow::addFileList(const QStringList& fileList)
{
    // Iterate over the list of files
    foreach( const QString& fileName, fileList )
    {
        // Retrieve information about the file
        QFileInfo fileInfo(fileName);
        QString name = fileInfo.fileName();
        QUrl url = QUrl("file://" + fileName);
        QString type = findMimeType(url);

        // Create a unique UUID for the file
        QUuid uuid = QUuid::createUuid();
        QString assetID = uuid.toString().remove(QChar('{')).remove(QChar('}'));

        // Create a QVariantMap to describe the file
        QVariantMap entry;
        entry["name"] = name;
        entry["url"] = url;
        entry["thumbnailUrl"] = url;
        entry["location"] = url.path();
        entry["type"] = type;
        entry["assetID"] = assetID;

        // Add the file entry
        addFileEntry(entry);
    }
}

// Remove one or more items from slideshow file list
void AirPlaySlideshow::removeFiles(const QVariantList& selectionList)
{
    // Check if selection list is a single item or multiple items
    if( selectionList.at(0).canConvert<QVariantList>() )
    {
        // Loop through all selected items
        for( int i = selectionList.count() - 1; i >= 0; i-- )
        {
            // Get the index path of position i in the selection list.
            QVariantList indexPath = selectionList.at(i).toList();

            // Remove the entry
            removeFile(indexPath);
        }
    }
    else
    {
        // Remove the entry
        removeFile(selectionList);
    }
}

// Set password
void AirPlaySlideshow::setPassword(const QString& password)
{
    m_password = password;
}

// Set slideshow transition
void AirPlaySlideshow::setTransition(int transition)
{
    m_transition = transition;
}

// View current device record
void AirPlaySlideshow::viewDevice(const QVariantList& indexPath)
{
    // Check if we have a valid index
    BonjourRecord record;
    if( !indexPath.isEmpty() )
    {
        // Retrieve the record fields
        const QVariantMap entry = m_pRecordListModel->data(indexPath).toMap();
        QString name = entry.value("name").value<QString>();
        QString serviceType = entry.value("serviceType").value<QString>();
        QString domain = entry.value("domain").value<QString>();

        // Populate the device record
        record = BonjourRecord(name, serviceType, domain);
    }

    // View the supplied device details
    m_pDevicePropertiesViewer->setDeviceRecord(record);
}

// Start slideshow
void AirPlaySlideshow::configureSlideshow()
{
    // Set the device and password
    m_pSlideshowViewer->setDevice(m_pDevice, m_password);

    // Copy the file list
    QVariantList fileList;
    for( int i = 0; i < m_pFileListModel->size(); i++ )
    {
        QVariantMap entry = m_pFileListModel->value(i).toMap();
        fileList.append(entry);
    }

	// Retrieve image scaling settings
	QSettings settings;
	bool scaleImages = settings.value("EnableScaleImages").toBool();
	QString screenSize = settings.value("ScreenSize").toString();
	QString imageQuality = settings.value("ImageQuality").toString();

	// Map screen size
	int screenWidth, screenHeight;
	if( screenSize == "1080p" )
	{
		screenWidth = 1920;
		screenHeight = 1080;
	}
	else
	{
		screenWidth = 1280;
		screenHeight = 720;
	}

	// Map image quality
	int quality;
	if( imageQuality == "low" )
	{
		quality = 25;
	}
	else if( imageQuality == "medium" )
	{
		quality = 50;
	}
	else
	{
		quality = 75;
	}

    // Set the slideshow properties
    m_pSlideshowViewer->setSlideshowProperties(fileList, m_transition, screenWidth, screenHeight, quality, scaleImages);
}

// Handler for browser replies
void AirPlaySlideshow::handleBrowserRecordsChanged(const QList<BonjourRecord>& recordList)
{
    // Clear the data model
    m_pRecordListModel->clear();

    // Iterate over the list of records
    foreach( const BonjourRecord& record, recordList )
    {
        // Copy the record data into a model entry
        QVariantMap entry;
        entry["name"] = record.name();
        entry["serviceType"] = record.serviceType();
        entry["domain"] = record.domain();

        // Add the entry to the model
        m_pRecordListModel->append(entry);
    }

    // Emit change signal
    emit recordListModelChanged();
}

// Handler for resolver replies
void AirPlaySlideshow::handleRecordResolved(const QString& fullName, const QHostInfo& hostInfo, int port, const QVariantMap& txtRecord)
{
    // Create and set device
    m_pDevice = new AirPlayDevice(fullName, hostInfo, port, txtRecord, this);
    emit deviceChanged();
}

// Handler for browser / resolver errors
void AirPlaySlideshow::handleError(DNSServiceErrorType err)
{
    // Log error
    cout << "AirPlaySlideshow - Received error: " << err << endl;
}

// Handler for invocation messages
void AirPlaySlideshow::handleInvoke(const InvokeRequest& request)
{
    // Retrieve information from the request
    QString requestAction = request.action();
    QString requestType = request.mimeType();
    QUrl requestUrl = request.uri();
    QByteArray requestData = request.data();

    // Check MIME type
    if( (requestType == "filelist/media") || (requestType == "filelist/mixed") || (requestType == "filelist/image") )
    {
        // Parse JSON data
        JsonDataAccess dataAccess;
        QVariant jsonData = dataAccess.loadFromBuffer(requestData);
        QVariantList fileDescriptionList = jsonData.value<QVariantList>();

        // Loop through list of supplied files
        foreach( const QVariant& fileDescriptionItem, fileDescriptionList )
        {
            // Extract the file description as a QVariantMap
            QVariantMap fileDescription = fileDescriptionItem.value<QVariantMap>();

            // Retrieve the URL and extract information from it
            QUrl url;
            QString name;
            if( fileDescription.contains("uri") )
            {
                url = fileDescription.value("uri").toUrl();
                QFileInfo fileInfo(url.path());
                name = fileInfo.fileName();
            }

            // Retrieve or divine MIME type
            QString type;
            if( fileDescription.contains("type") )
            {
                type = fileDescription.value("type").toString();
            }
            else
            {
                type = findMimeType(url);
            }

            // Validate the file's MIME type before adding it to the list
            if( validateMimeType(type) )
            {
                // Retrieve metadata and process for thumbnail
                QUrl thumbnailUrl = url;
                if( fileDescription.contains("metadata") )
                {
                    QByteArray metadataArray = fileDescription.value("metadata").toByteArray();
                    jsonData = dataAccess.loadFromBuffer(metadataArray);
                    QVariantMap metadata = jsonData.value<QVariantMap>();
                    if( metadata.contains("thumb") )
                    {
                        // Retrieve URL and test for extra "file://" protocol
                        QString temp = metadata.value("thumb").toString();
                        if( temp.startsWith("file://file://") )
                        {
                            temp = temp.mid(7);
                        }
                        thumbnailUrl = QUrl(temp);
                    }
                }

                // Create a unique UUID for the file
                QUuid uuid = QUuid::createUuid();
                QString assetID = uuid.toString().remove(QChar('{')).remove(QChar('}'));

                // Create a QVariantMap to describe the file
                QVariantMap entry;
                entry["name"] = name;
                entry["url"] = url;
                entry["thumbnailUrl"] = thumbnailUrl;
                entry["location"] = url.path();
                entry["type"] = type;
                entry["assetID"] = assetID;

                // Add the file entry
                addFileEntry(entry);
            }
        }
    }
    else if( validateMimeType(requestType) )
    {
        // Extract the file name from the URL
        QFileInfo fileInfo(requestUrl.path());
        QString name = fileInfo.fileName();

        // Create a unique UUID for the file
        QUuid uuid = QUuid::createUuid();
        QString assetID = uuid.toString().remove(QChar('{')).remove(QChar('}'));

        // Create a QVariantMap to describe the file
        QVariantMap entry;
        entry["name"] = name;
        entry["url"] = requestUrl;
        entry["thumbnailUrl"] = requestUrl;
        entry["location"] = requestUrl.path();
        entry["type"] = requestType;
        entry["assetID"] = assetID;

        // Add the file entry
        addFileEntry(entry);
    }
}

// Handler for system language change event
void AirPlaySlideshow::handleSystemLanguageChanged()
{
    // Remove existing translation files
    QCoreApplication::instance()->removeTranslator(m_pTranslator);

    // Initiate, load and install the application translation files
    QString locale_string = QLocale().name();
    QString file_name = QString("AirPlaySlideshow_%1").arg(locale_string);
    if( m_pTranslator->load(file_name, "app/native/qm") )
    {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}


// Add item to file list, checking for duplicates
void AirPlaySlideshow::addFileEntry(const QVariantMap& entry)
{
    // Search for matching file URL
    bool bFound = false;
    for( int i = 0; i < m_pFileListModel->size(); i++ )
    {
        QVariantMap testEntry = m_pFileListModel->value(i).toMap();
        if( testEntry.value("url").toString() == entry.value("url").toString() )
        {
            bFound = true;
            break;
        }
    }

    // If no match found, add the entry to the data model
    if( bFound == false )
    {
        m_pFileListModel->append(entry);

        // Emit change signal
        emit fileListModelChanged();
    }
}

// Remove item from file list
void AirPlaySlideshow::removeFile(const QVariantList& indexPath)
{
    // Retrieve the entry and its index
    const QVariantMap entry = m_pFileListModel->data(indexPath).toMap();
    int index = m_pFileListModel->indexOf(entry);

    // Remove the entry
    m_pFileListModel->removeAt(index);

    // Emit change signal
    emit fileListModelChanged();
}

// Launch file viewer for preview
void AirPlaySlideshow::viewFile(const QVariantList& indexPath)
{
    // Check if we have a valid index
    if( !indexPath.isEmpty() )
    {
        // Retrieve the entry and relevant fields
        const QVariantMap entry = m_pFileListModel->data(indexPath).toMap();
        QUrl url = entry.value("url").toUrl();
        QString type = entry.value("type").toString();

        // Create an invocation request
        InvokeRequest request;
        request.setTarget("sys.pictures.card.previewer");
        request.setAction("bb.action.VIEW");
        request.setUri(url);
        request.setMimeType(type);

        // Send the invocation request
        InvokeTargetReply* pReply = m_pInvokeManager->invoke(request);
        pReply->deleteLater();
    }
}

// Property accessor methods
DataModel* AirPlaySlideshow::recordListModel() const
{
    return m_pRecordListModel;
}

// Property accessor methods
DataModel* AirPlaySlideshow::fileListModel() const
{
    return m_pFileListModel;
}

// Property accessor methods
AirPlayDevice* AirPlaySlideshow::device() const
{
    return m_pDevice;
}

// Property accessor methods
DevicePropertiesViewer* AirPlaySlideshow::devicePropertiesViewer() const
{
    return m_pDevicePropertiesViewer;
}

// Property accessor methods
SlideshowViewer* AirPlaySlideshow::slideshowViewer() const
{
    return m_pSlideshowViewer;
}
