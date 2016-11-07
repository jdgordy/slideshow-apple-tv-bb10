#ifndef AIRPLAYSLIDESHOW_HPP_
#define AIRPLAYSLIDESHOW_HPP_

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QVariantList>
#include <QtCore/QVariantMap>
#include <QtNetwork/QHostInfo>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include "BonjourRecord.hpp"
#include "dns_sd.h"

// Forward class declarations
namespace bb
{
    namespace cascades
    {
        class Application;
        class DataModel;
        class ArrayDataModel;
        class LocaleHandler;
    }
}
class BonjourBrowser;
class BonjourResolver;
class AirPlayDevice;
class DevicePropertiesViewer;
class SlideshowViewer;
class QTranslator;

//
// AirPlaySlideshow
//
class AirPlaySlideshow : public QObject
{
    Q_OBJECT

    // The model that provides the list of service records
    Q_PROPERTY(bb::cascades::DataModel* recordListModel READ recordListModel NOTIFY recordListModelChanged);

    // The model that provides the list of files
    Q_PROPERTY(bb::cascades::DataModel* fileListModel READ fileListModel NOTIFY fileListModelChanged);

    // The currently selected device
    Q_PROPERTY(AirPlayDevice* device READ device NOTIFY deviceChanged);

    // The viewer object for the current device
    Q_PROPERTY(DevicePropertiesViewer* devicePropertiesViewer READ devicePropertiesViewer CONSTANT);

    // The slideshow viewer object
    Q_PROPERTY(SlideshowViewer* slideshowViewer READ slideshowViewer CONSTANT);

public:

    // Constructor / destructor
    AirPlaySlideshow(bb::cascades::Application* app);
    virtual ~AirPlaySlideshow();

    // Set or retrieve configuration parameters
    Q_INVOKABLE void setParameter(const QString& parameter, const QString& value);
    Q_INVOKABLE QString getParameter(const QString& parameter, const QString& defaultValue);

    // Set device and trigger resolution
    Q_INVOKABLE void setDevice(const QVariantList& indexPath);

    // Add item(s) to slideshow file list
    Q_INVOKABLE void addFileList(const QStringList& fileList);

    // Remove one or multiple items from slideshow file list
    Q_INVOKABLE void removeFiles(const QVariantList& selectionList);

    // Launch file viewer for preview
    Q_INVOKABLE void viewFile(const QVariantList& indexPath);

    // Set password
    Q_INVOKABLE void setPassword(const QString& password);

    // Set slideshow transition
    Q_INVOKABLE void setTransition(int transition);

    // View device record
    Q_INVOKABLE void viewDevice(const QVariantList& indexPath);

    // Configure slideshow
    Q_INVOKABLE void configureSlideshow();

Q_SIGNALS:

    // Property change notification signals
    void deviceChanged();
    void recordListModelChanged();
    void fileListModelChanged();

protected Q_SLOTS:

    // Handler for browser replies
    void handleBrowserRecordsChanged(const QList<BonjourRecord>& recordList);

    // Handler for resolver replies
    void handleRecordResolved(const QString& fullName, const QHostInfo& hostInfo, int port, const QVariantMap& txtRecord);

    // Handler for error replies
    void handleError(DNSServiceErrorType err);

    // Handler for invocation messages
    void handleInvoke(const bb::system::InvokeRequest& request);

    // Handler for system language change event
    void handleSystemLanguageChanged();

protected:

    // Add item to file list, checking for duplicates
    void addFileEntry(const QVariantMap& entry);

    // Remove item from file list
    void removeFile(const QVariantList& indexPath);

    // Property accessor methods
    bb::cascades::DataModel* recordListModel() const;
    bb::cascades::DataModel* fileListModel() const;
    AirPlayDevice* device() const;
    DevicePropertiesViewer* devicePropertiesViewer() const;
    SlideshowViewer* slideshowViewer() const;

    // Property values
    bb::cascades::ArrayDataModel*   m_pRecordListModel;
    bb::cascades::ArrayDataModel*   m_pFileListModel;
    AirPlayDevice*                  m_pDevice;
    DevicePropertiesViewer*         m_pDevicePropertiesViewer;
    SlideshowViewer*                m_pSlideshowViewer;

    // Invocation manager
    bb::system::InvokeManager*  m_pInvokeManager;

    // Slideshow properties
    QString             m_password;
    int                 m_transition;

    // Bonjour service browser and resolver
    BonjourBrowser*     m_pBrowser;
    BonjourResolver*    m_pResolver;

    // Translator and locale handler objects
    QTranslator*                    m_pTranslator;
    bb::cascades::LocaleHandler*    m_pLocaleHandler;
};

#endif /* AIRPLAYSLIDESHOW_HPP_ */
