/*
 * AirPlayCommand.hpp
 *
 *  Created on: Apr 27, 2013
 *      Author: jgordy
 */

#ifndef AIRPLAYCOMMAND_HPP_
#define AIRPLAYCOMMAND_HPP_

#include <QtCore/QSharedPointer>
#include <QtNetwork/QHostAddress>
#include <QtCore/QUrl>

// Forward class declarations
class AirPlayConnection;

//
// AirPlayCommand
//
class AirPlayCommand
{
public:

    // Constructor / destructor
    AirPlayCommand(const QString& commandStatus);
    virtual ~AirPlayCommand();

    // Get command status
    QString getCommandStatus() const;

    // Execute command
    virtual bool execute(AirPlayConnection* pConnection) = 0;

protected:

    // Command status to return on execution
    QString m_commandStatus;
};

// Declare command list
typedef QList<QSharedPointer<AirPlayCommand> > AirPlayCommandList;

//
// AirPlayOpenConnectionCommand
//
class AirPlayOpenConnectionCommand : public AirPlayCommand
{
public:

    // Constructor / destructor
    AirPlayOpenConnectionCommand(const QHostAddress& address, int port, const QString& password, const QString& commandStatus);

    // Execute command
    bool execute(AirPlayConnection* pConnection);

protected:

    // Command parameters
    QHostAddress    m_address;
    int             m_port;
    QString         m_password;
};

//
// AirPlayCloseConnectionCommand
//
class AirPlayCloseConnectionCommand : public AirPlayCommand
{
public:

    // Constructor / destructor
    AirPlayCloseConnectionCommand(const QString& commandStatus);

    // Execute command
    bool execute(AirPlayConnection* pConnection);
};

//
// AirPlaySendPhotoCommand
//
class AirPlaySendPhotoCommand : public AirPlayCommand
{
public:

    // Constructor / destructor
    AirPlaySendPhotoCommand(const QUrl& url, const QString& contentType, int transition, const QString& commandStatus);

    // Execute command
    bool execute(AirPlayConnection* pConnection);

protected:

    // Command parameters
    QUrl    m_url;
    QString m_contentType;
    int     m_transition;
};

//
// AirPlayUploadPhotoCommand
//
class AirPlayUploadPhotoCommand : public AirPlayCommand
{
public:

    // Constructor / destructor
    AirPlayUploadPhotoCommand(const QString& assetID, const QUrl& url, const QString& contentType, const QString& commandStatus);

    // Execute command
    bool execute(AirPlayConnection* pConnection);

protected:

    // Command parameters
    QString m_assetID;
    QUrl    m_url;
    QString m_contentType;
};

//
// AirPlayDisplayPhotoCommand
//
class AirPlayDisplayPhotoCommand : public AirPlayCommand
{
public:

    // Constructor / destructor
    AirPlayDisplayPhotoCommand(const QString& assetID, int transition, const QString& commandStatus);

    // Execute command
    bool execute(AirPlayConnection* pConnection);

protected:

    // Command parameters
    QString m_assetID;
    int     m_transition;
};

#endif /* AIRPLAYCOMMAND_HPP_ */
