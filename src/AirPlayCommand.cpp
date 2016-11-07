/*
 * AirPlayCommand.cpp
 *
 *  Created on: May 1, 2013
 *      Author: jgordy
 */

#include "AirPlayCommand.hpp"
#include "AirPlayConnection.hpp"

// Constructor
AirPlayCommand::AirPlayCommand(const QString& commandStatus) :
    m_commandStatus(commandStatus)
{
}

// Destructor
AirPlayCommand::~AirPlayCommand()
{
}

// Get command status
QString AirPlayCommand::getCommandStatus() const
{
    return m_commandStatus;
}

// Constructor
AirPlayOpenConnectionCommand::AirPlayOpenConnectionCommand(const QHostAddress& address, int port, const QString& password, const QString& commandStatus) :
    AirPlayCommand(commandStatus),
    m_address(address),
    m_port(port),
    m_password(password)
{
}

// Execute command
bool AirPlayOpenConnectionCommand::execute(AirPlayConnection* pConnection)
{
    bool bResult = pConnection->openConnection(m_address, m_port, m_password);
    return bResult;
}

// Constructor
AirPlayCloseConnectionCommand::AirPlayCloseConnectionCommand(const QString& commandStatus) :
    AirPlayCommand(commandStatus)
{
}

// Execute command
bool AirPlayCloseConnectionCommand::execute(AirPlayConnection* pConnection)
{
    bool bResult = pConnection->closeConnection();
    return bResult;
}

// Constructor
AirPlaySendPhotoCommand::AirPlaySendPhotoCommand(const QUrl& url, const QString& contentType, int transition, const QString& commandStatus) :
    AirPlayCommand(commandStatus),
    m_url(url),
    m_contentType(contentType),
    m_transition(transition)
{
}

// Execute command
bool AirPlaySendPhotoCommand::execute(AirPlayConnection* pConnection)
{
    bool bResult = pConnection->sendPhoto(m_url, m_contentType, m_transition);
    return bResult;
}

// Constructor
AirPlayUploadPhotoCommand::AirPlayUploadPhotoCommand(const QString& assetID, const QUrl& url, const QString& contentType, const QString& commandStatus) :
    AirPlayCommand(commandStatus),
    m_assetID(assetID),
    m_url(url),
    m_contentType(contentType)
{
}

// Execute command
bool AirPlayUploadPhotoCommand::execute(AirPlayConnection* pConnection)
{
    bool bResult = pConnection->uploadPhoto(m_assetID, m_url, m_contentType);
    return bResult;
}

// Constructor
AirPlayDisplayPhotoCommand::AirPlayDisplayPhotoCommand(const QString& assetID, int transition, const QString& commandStatus) :
    AirPlayCommand(commandStatus),
    m_assetID(assetID),
    m_transition(transition)
{
}

// Execute command
bool AirPlayDisplayPhotoCommand::execute(AirPlayConnection* pConnection)
{
    bool bResult = pConnection->displayPhoto(m_assetID, m_transition);
    return bResult;
}
