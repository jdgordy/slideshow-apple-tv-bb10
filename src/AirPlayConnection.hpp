/*
 * AirPlayConnection.hpp
 *
 *  Created on: Apr 5, 2013
 *      Author: jgordy
 */

#ifndef AIRPLAYCONNECTION_HPP_
#define AIRPLAYCONNECTION_HPP_

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QMutex>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QAuthenticator>

//
// AirPlayConnection
//
class AirPlayConnection : public QObject
{
    Q_OBJECT

public:

    // Constants
    static const int AIRPLAY_TRANSITION_NONE = 0;
    static const int AIRPLAY_TRANSITION_SLIDELEFT = 1;
    static const int AIRPLAY_TRANSITION_SLIDERIGHT = 2;
    static const int AIRPLAY_TRANSITION_DISSOLVE = 3;

    // Constructor / destructor
    AirPlayConnection(QObject* pParent = NULL);
    virtual ~AirPlayConnection();

    // Set screen dimensions, quality, and scaling flag
    void setScalingOptions(int screenWidth, int screenHeight, int quality, bool scaleImages);

    // Open server connection
    bool openConnection(const QHostAddress& address, int port, const QString& password);

    // Close server connection
    bool closeConnection();

    // Send and display photo
    bool sendPhoto(const QUrl& url, const QString& contentType, int transition);

    // Upload photo for caching
    bool uploadPhoto(const QString& assetID, const QUrl& url, const QString& contentType);

    // Display cached photo
    bool displayPhoto(const QString& assetID, int transition);

    // Cancel operation in progress
    bool cancelOperation();

Q_SIGNALS:

    // Emitted on operation completion
    void operationComplete();

    // Emitted on operation cancellation
    void operationCancelled();

    // Emitted on error
    void error(const QString& errorDescription);

protected Q_SLOTS:

    // Handler for authentication requests
    void handleAuthenticationRequired(QNetworkReply* pReply, QAuthenticator* pAuthenticator);

    // Handler for connection completion
    void handleConnectionFinished();

    // Handler for transaction completion
    void handleFinished();

    // Handler for connection error
    void handleConnectionError(QNetworkReply::NetworkError networkError);

    // Handler for network reply error
    void handleError(QNetworkReply::NetworkError networkError);

protected:

    // Construct network request
    void constructRequest(QNetworkRequest& request, const QString& type);

    // Server configuration
    QHostAddress    m_address;
    int             m_port;
    QString         m_password;

    // Server connection state
    QMutex                  m_mutex;
    QNetworkAccessManager*  m_pManager;
    QNetworkReply*          m_pReply;
    QString                 m_sessionID;
    bool                    m_bConnected;

    // Screen dimensions and rescaling flag
    int     m_screenWidth;
    int	    m_screenHeight;
    int	    m_quality;
    bool    m_bScaleImages;

    // Transaction state
    bool    m_bTransactionPending;
    bool    m_bAuthenticationAttempted;
};

#endif /* AIRPLAYCONNECTION_HPP_ */
