/*
 * AirPlayConnection.cpp
 *
 *  Created on: Apr 5, 2013
 *      Author: jgordy
 */

#include <QtCore/QBuffer>
#include <QtCore/QFile>
#include <QtCore/QMutexLocker>
#include <QtCore/QSize>
#include <QtCore/QUuid>
#include <QtGui/QImage>
#include <QtGui/QImageWriter>
#include <QtNetwork/QNetworkRequest>
#include "AirPlayConnection.hpp"
#include <iostream>

// Declare namespaces
using namespace std;

// Constants
static const QString g_strAirPlayUsername = "AirPlay";
static const QString g_strAirPlayTransitionNone = "None";
static const QString g_strAirPlayTransitionSlideLeft = "SlideLeft";
static const QString g_strAirPlayTransitionSlideRight = "SlideRight";
static const QString g_strAirPlayTransitionDissolve = "Dissolve";

// Map transition
static QString MapTransition(int transition)
{
    QString transitionName;
    switch( transition )
    {
        case AirPlayConnection::AIRPLAY_TRANSITION_SLIDELEFT:
            transitionName = g_strAirPlayTransitionSlideLeft;
            break;
        case AirPlayConnection::AIRPLAY_TRANSITION_SLIDERIGHT:
            transitionName = g_strAirPlayTransitionSlideRight;
            break;
        case AirPlayConnection::AIRPLAY_TRANSITION_DISSOLVE:
            transitionName = g_strAirPlayTransitionDissolve;
            break;
        case AirPlayConnection::AIRPLAY_TRANSITION_NONE:
        default:
            transitionName = g_strAirPlayTransitionNone;
            break;
    }

    return transitionName;
}

// Resize image
static QByteArray ResizeImage(const QByteArray& inputBuffer, int screenWidth, int screenHeight, int quality, bool& bRescaled)
{
	// Default to use no scaling
    QByteArray outputBuffer = inputBuffer;
    bRescaled = false;

    // Construct QImage from source data
    QImage inputImage = QImage::fromData(inputBuffer);
    float screenAspectRatio = (float) screenWidth / (float) screenHeight;
    if( !inputImage.isNull() )
    {
    	// Calculate image aspect ratio
    	float aspectRatio = (float) inputImage.width() / (float) inputImage.height();

    	// Compare image and screen aspect ratios to determine scaling dimension
    	if( aspectRatio > screenAspectRatio )
    	{
    		// Scale to maximum of screen width
    		if( inputImage.width() > screenWidth )
    		{
                // Rescale to screen width
                QImage outputImage = inputImage.scaledToWidth(screenWidth);
                QBuffer imageBuffer;
                QImageWriter imageWriter(&imageBuffer, "jpeg");
                imageWriter.setQuality(quality);
                if( imageWriter.write( outputImage ) )
                {
                	// Retrieve encoded image buffer
                	outputBuffer = imageBuffer.buffer();
                    bRescaled = true;
                }
    		}
    	}
    	else if( aspectRatio < screenAspectRatio )
    	{
    		// Scale to maximum of screen height
            if( inputImage.height() > screenHeight )
            {
                // Rescale to screen height
                QImage outputImage = inputImage.scaledToHeight(screenHeight);
                QBuffer imageBuffer;
                QImageWriter imageWriter(&imageBuffer, "jpeg");
                imageWriter.setQuality(quality);
                if( imageWriter.write( outputImage ) )
                {
                	// Retrieve encoded image buffer
                	outputBuffer = imageBuffer.buffer();
                    bRescaled = true;
                }
            }
    	}
    	else
    	{
    		// Scale to maximum of screen dimensions
    		if( (inputImage.width() > screenWidth) || (inputImage.height() > screenHeight) )
    		{
                // Rescale to screen width and height
                QImage outputImage = inputImage.scaled(screenWidth, screenHeight, Qt::KeepAspectRatio);
                QBuffer imageBuffer;
                QImageWriter imageWriter(&imageBuffer, "jpeg");
                imageWriter.setQuality(quality);
                if( imageWriter.write( outputImage ) )
                {
                	// Retrieve encoded image buffer
                	outputBuffer = imageBuffer.buffer();
                    bRescaled = true;
                }
    		}
    	}
    }

    return outputBuffer;
}

// Constructor
AirPlayConnection::AirPlayConnection(QObject* pParent) :
    QObject(pParent),
    m_port(-1),
    m_pManager(NULL),
    m_pReply(NULL),
    m_bConnected(false),
    m_screenWidth(1280),
    m_screenHeight(720),
    m_quality(75),
    m_bScaleImages(false),
    m_bTransactionPending(false),
    m_bAuthenticationAttempted(false)
{
}

// Destructor
AirPlayConnection::~AirPlayConnection()
{
    // Release the network access manager
    if( m_pManager )
    {
        m_pManager->deleteLater();
        m_pManager = NULL;
    }
}

// Set screen dimensions, quality, and scaling flag
void AirPlayConnection::setScalingOptions(int screenWidth, int screenHeight, int quality, bool scaleImages)
{
    // Lock the mutex
    QMutexLocker lock(&m_mutex);

    // Set dimensions
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_quality = quality;
	m_bScaleImages = scaleImages;
}

// Open server connection
bool AirPlayConnection::openConnection(const QHostAddress& address, int port, const QString& password)
{
    // Lock the mutex
    QMutexLocker lock(&m_mutex);

    // Check if we have a connection
    if( !m_bConnected )
    {
        // Check if a transaction is pending
        if( !m_bTransactionPending )
        {
            // Create the network access manager
            m_pManager = new QNetworkAccessManager(this);

            // Connect the network manager signals to our own handlers
            QObject::connect(m_pManager, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)), this, SLOT(handleAuthenticationRequired(QNetworkReply*, QAuthenticator*)));

            // Generate a new session UUID and extract its string representation
            QUuid sessionID = QUuid::createUuid();
            m_sessionID = sessionID.toString().remove(QChar('{')).remove(QChar('}'));

            // Save the server info
            m_address = address;
            m_port = port;
            m_password = password;

            // Construct the connection request
            QNetworkRequest request;
            constructRequest(request, "/server-info");

            // Submit the request and connect the reply / error signals
            m_pReply = m_pManager->get(request);
            QObject::connect(m_pReply, SIGNAL(finished()), this, SLOT(handleConnectionFinished()));
            QObject::connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleConnectionError(QNetworkReply::NetworkError)));

            // Set the transaction pending flag
            m_bTransactionPending = true;

            // Log debug info
            cout << "AirPlayConnection::openConnection" << endl;
            return true;
        }
        else
        {
            // Log error
            cout << "AirPlayConnection::openConnection - Transaction is already pending" << endl;
            return false;
        }
    }
    else
    {
        // Log error
        cout << "AirPlayConnection::openConnection - Connection is already established" << endl;
        return false;
    }
}

// Close server connection
bool AirPlayConnection::closeConnection()
{
    // Lock the mutex
    QMutexLocker lock(&m_mutex);

    // Check if a transaction is pending
    if( !m_bTransactionPending )
    {
        // Release the network access manager
        if( m_pManager )
        {
            m_pManager->deleteLater();
            m_pManager = NULL;
        }

        // Clear state
        m_bConnected = false;
        m_bTransactionPending = false;
        m_bAuthenticationAttempted = false;
        m_sessionID.clear();

        // Emit the completion signal
        emit operationComplete();

        // Log debug info
        cout << "AirPlayConnection::closeConnection" << endl;
        return true;
    }
    else
    {
        // Log error
        cout << "AirPlayConnection::closeConnection - Transaction is already pending" << endl;
        return false;
    }
}

// Send and display photo
bool AirPlayConnection::sendPhoto(const QUrl& url, const QString& contentType, int transition)
{
    // Lock the mutex
    QMutexLocker lock(&m_mutex);
    QString actualContentType = contentType;

    // Check if we have a connection
    if( m_bConnected )
    {
        // Check if a transaction is pending
        if( !m_bTransactionPending )
        {
            // Open and read the file
            QFile file(url.path());
            if( file.open(QIODevice::ReadOnly) )
            {
                // Read file contents
                QByteArray imageBuffer = file.readAll();
                file.close();

                // Check for scaling
                if( m_bScaleImages )
                {
                	bool bRescaled;
                	imageBuffer = ResizeImage(imageBuffer, m_screenWidth, m_screenHeight, m_quality, bRescaled);
                	if( bRescaled )
                	{
                		// Modify content type
                		actualContentType = "image/jpeg";

                        // Log debug info
                        cout << "AirPlayConnection::sendPhoto - Rescaled image to " << m_screenWidth << "x" << m_screenHeight << ", quality = " << m_quality << endl;
                	}
                }

                // Map transition
                QByteArray transitionName = MapTransition(transition).toStdString().c_str();

                // Construct the request
                QNetworkRequest request;
                constructRequest(request, "/photo");
                request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray(actualContentType.toStdString().c_str()));
                request.setHeader(QNetworkRequest::ContentLengthHeader, imageBuffer.size());
                request.setRawHeader("X-Apple-Transition", transitionName);

                // Submit the request and connect the progress / reply / error signals
                m_pReply = m_pManager->post(request, imageBuffer);
                QObject::connect(m_pReply, SIGNAL(finished()), this, SLOT(handleFinished()));
                QObject::connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleError(QNetworkReply::NetworkError)));

                // Set the transaction pending flag
                m_bTransactionPending = true;

                // Log debug info
                cout << "AirPlayConnection::sendPhoto" << endl;
                return true;
            }
            else
            {
                // Log error
                cout << "AirPlayConnection::sendPhoto - Unable to open file " << url.path().toStdString() << endl;
                return false;
            }
        }
        else
        {
            // Log error
            cout << "AirPlayConnection::sendPhoto - Transaction is already pending" << endl;
            return false;
        }
    }
    else
    {
        // Log error
        cout << "AirPlayConnection::sendPhoto - Connection is not established" << endl;
        return false;
    }
}

// Upload photo for caching
bool AirPlayConnection::uploadPhoto(const QString& assetID, const QUrl& url, const QString& contentType)
{
    // Lock the mutex
    QMutexLocker lock(&m_mutex);
    QString actualContentType = contentType;

    // Check if we have a connection
    if( m_bConnected )
    {
        // Check if a transaction is pending
        if( !m_bTransactionPending )
        {
            // Open and read the file
            QFile file(url.path());
            if( file.open(QIODevice::ReadOnly) )
            {
                // Read file contents
                QByteArray imageBuffer = file.readAll();
                file.close();

                // Check for scaling
                if( m_bScaleImages )
                {
                	bool bRescaled;
                	imageBuffer = ResizeImage(imageBuffer, m_screenWidth, m_screenHeight, m_quality, bRescaled);
                	if( bRescaled )
                	{
                		// Modify content type
                		actualContentType = "image/jpeg";

                        // Log debug info
                        cout << "AirPlayConnection::sendPhoto - Rescaled image to " << m_screenWidth << "x" << m_screenHeight << ", quality = " << m_quality << endl;
                	}
                }

                // Construct the request
                QNetworkRequest request;
                constructRequest(request, "/photo");
                request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray(actualContentType.toStdString().c_str()));
                request.setHeader(QNetworkRequest::ContentLengthHeader, imageBuffer.size());
                request.setRawHeader("X-Apple-AssetAction", "cacheOnly");
                request.setRawHeader("X-Apple-AssetKey", QByteArray(assetID.toStdString().c_str()));

                // Submit the request and connect the progress / reply / error signals
                m_pReply = m_pManager->put(request, imageBuffer);
                QObject::connect(m_pReply, SIGNAL(finished()), this, SLOT(handleFinished()));
                QObject::connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleError(QNetworkReply::NetworkError)));

                // Set the transaction pending flag
                m_bTransactionPending = true;

                // Log debug info
                cout << "AirPlayConnection::uploadPhoto" << endl;
                return true;
            }
            else
            {
                // Log error
                cout << "AirPlayConnection::uploadPhoto - Unable to open file " << url.path().toStdString() << endl;
                return false;
            }
        }
        else
        {
            // Log error
            cout << "AirPlayConnection::uploadPhoto - Transaction is already pending" << endl;
            return false;
        }
    }
    else
    {
        // Log error
        cout << "AirPlayConnection::uploadPhoto - Connection is not established" << endl;
        return false;
    }
}

// Display cached photo
bool AirPlayConnection::displayPhoto(const QString& assetID, int transition)
{
    // Lock the mutex
    QMutexLocker lock(&m_mutex);

    // Check if we have a connection
    if( m_bConnected )
    {
        // Check if a transaction is pending
        if( !m_bTransactionPending )
        {
            // Map transition
            QByteArray transitionName = MapTransition(transition).toStdString().c_str();

            // Construct the request
            QNetworkRequest request;
            constructRequest(request, "/photo");
            request.setRawHeader("X-Apple-AssetAction", "displayCached");
            request.setRawHeader("X-Apple-AssetKey", QByteArray(assetID.toStdString().c_str()));
            request.setRawHeader("X-Apple-Transition", transitionName);

            // Submit the request and connect the progress / reply / error signals
            m_pReply = m_pManager->put(request, QByteArray());
            QObject::connect(m_pReply, SIGNAL(finished()), this, SLOT(handleFinished()));
            QObject::connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleError(QNetworkReply::NetworkError)));

            // Set the transaction pending flag
            m_bTransactionPending = true;

            // Log debug info
            cout << "AirPlayConnection::displayPhoto" << endl;
            return true;
        }
        else
        {
            // Log error
            cout << "AirPlayConnection::displayPhoto - Transaction is already pending" << endl;
            return false;
        }
    }
    else
    {
        // Log error
        cout << "AirPlayConnection::displayPhoto - Connection is not established" << endl;
        return false;
    }
}

// Cancel operation in progress
bool AirPlayConnection::cancelOperation()
{
    // Lock the mutex
    QMutexLocker lock(&m_mutex);

    // Check if we have an operation pending
    if( m_bTransactionPending )
    {
        // Check if a reply instance exists
        if( m_pReply )
        {
            // Unlock the mutex while we abort
            lock.unlock();

            // Abort the operation
            m_pReply->abort();

            // Log debug info
            cout << "AirPlayConnection::cancelOperation" << endl;
            return true;
        }
        else
        {
            // Log error
            cout << "AirPlayConnection::cancelOperation - No reply exists" << endl;
            return false;
        }
    }
    else
    {
        // Log error
        cout << "AirPlayConnection::cancelOperation - No transaction is pending" << endl;
        return false;
    }
}

// Handler for authentication requests
void AirPlayConnection::handleAuthenticationRequired(QNetworkReply* pReply, QAuthenticator* pAuthenticator)
{
    // Check if authentication has been attempted
    if( !m_bAuthenticationAttempted )
    {
        // Supply the username and password
        pAuthenticator->setUser(g_strAirPlayUsername);
        pAuthenticator->setPassword(m_password);

        // Set the flag for next time
        m_bAuthenticationAttempted = true;

        // Log debug info
        cout << "AirPlayConnection::handleAuthenticationRequired - Authentication requested once" << endl;
    }
    else
    {
        // Log debug info
        cout << "AirPlayConnection::handleAuthenticationRequired - Authentication requested twice" << endl;
    }
}

// Handler for connection reply
void AirPlayConnection::handleConnectionFinished()
{
    // Lock the mutex
    QMutexLocker lock(&m_mutex);

    // Retrieve the reply and check for validity
    QNetworkReply* pReply = qobject_cast<QNetworkReply*>(sender());
    if( pReply )
    {
        // Retrieve the network and HTTP response, if present
        QNetworkReply::NetworkError networkError = pReply->error();
        QString errorString = pReply->errorString();
        int httpResponseCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString httpResponseReason(pReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray());

        // Check for completion, cancellation, or error
        if( networkError == QNetworkReply::NoError )
        {
            // Set the connection flag
            m_bConnected = true;

            // Emit the completion signal
            emit operationComplete();

            // Log debug info
            cout << "AirPlayConnection::handleConnectionFinished - HTTP: " << httpResponseCode << " " << httpResponseReason.toStdString() << endl;
        }
        else if( networkError == QNetworkReply::OperationCanceledError )
        {
            // Emit the cancelled signal
            emit operationCancelled();

            // Log debug info
            cout << "AirPlayConnection::handleConnectionFinished - operation cancelled" << endl;
        }
        else
        {
            // Emit the error signal
            emit error(errorString);

            // Log error
            cout << "AirPlayConnection::handleConnectionFinished - HTTP: " << httpResponseCode << " " << httpResponseReason.toStdString() << endl;
            cout << "AirPlayConnection::handleConnectionFinished - Error: " << networkError << " (" << errorString.toStdString() << ")" << endl;
        }

        // Release the reply
        pReply->deleteLater();
    }

    // Mark transaction as completed
    m_pReply = NULL;
    m_bAuthenticationAttempted = false;
    m_bTransactionPending = false;
}

// Handler for transaction reply
void AirPlayConnection::handleFinished()
{
    // Lock the mutex
    QMutexLocker lock(&m_mutex);

    // Retrieve the reply and check for validity
    QNetworkReply* pReply = qobject_cast<QNetworkReply*>(sender());
    if( pReply )
    {
        // Retrieve the network and HTTP response, if present
        QNetworkReply::NetworkError networkError = pReply->error();
        QString errorString = pReply->errorString();
        int httpResponseCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString httpResponseReason(pReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray());

        // Check for error
        if( networkError == QNetworkReply::NoError )
        {
            // Emit the completion signal
            emit operationComplete();

            // Log debug info
            cout << "AirPlayConnection::handleFinished - HTTP: " << httpResponseCode << " " << httpResponseReason.toStdString() << endl;
        }
        else if( networkError == QNetworkReply::OperationCanceledError )
        {
            // Emit the cancelled signal
            emit operationCancelled();

            // Log debug info
            cout << "AirPlayConnection::handleFinished - operation canceled" << endl;
        }
        else
        {
            // Emit the error signal
            emit error(errorString);

            // Log error
            cout << "AirPlayConnection::handleFinished - HTTP: " << httpResponseCode << " " << httpResponseReason.toStdString() << endl;
            cout << "AirPlayConnection::handleFinished - Error: " << networkError << " (" << errorString.toStdString() << ")" << endl;
        }

        // Release the reply
        pReply->deleteLater();
    }

    // Mark transaction as completed
    m_pReply = NULL;
    m_bAuthenticationAttempted = false;
    m_bTransactionPending = false;
}

// Handler for connection error
void AirPlayConnection::handleConnectionError(QNetworkReply::NetworkError networkError)
{
    // Retrieve the reply and check for validity
    QNetworkReply* pReply = qobject_cast<QNetworkReply*>(sender());
    if( pReply )
    {
        // Log error
        cout << "AirPlayConnection::handleConnectionError - Error: " << networkError << " (" << pReply->errorString().toStdString() << ")" << endl;
    }
}

// Handler for network error
void AirPlayConnection::handleError(QNetworkReply::NetworkError networkError)
{
    // Retrieve the reply and check for validity
    QNetworkReply* pReply = qobject_cast<QNetworkReply*>(sender());
    if( pReply )
    {
        // Log error
        cout << "AirPlayConnection::handleError - Error: " << networkError << " (" << pReply->errorString().toStdString() << ")" << endl;
    }
}

// Construct network request
void AirPlayConnection::constructRequest(QNetworkRequest& request, const QString& type)
{
    // Generate the HTTP request
    QUrl url("http://" + m_address.toString() + ":" + QString::number(m_port) + type);
    request.setUrl(url);
    request.setRawHeader("User-Agent", "MediaControl/1.0");
    request.setRawHeader("X-Apple-Session-ID", QByteArray(m_sessionID.toStdString().c_str()));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    request.setHeader(QNetworkRequest::ContentLengthHeader, 0);
}
