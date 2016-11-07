/*
 * SlideshowViewer.cpp
 *
 *  Created on: Apr 16, 2013
 *      Author: jgordy
 */

// Default empty project template
#include "SlideshowViewer.hpp"
#include "AirPlayConnection.hpp"
#include "AirPlayCommandExecutor.hpp"
#include "AirPlayCommand.hpp"
#include "AirPlayDevice.hpp"
#include <iostream>

using namespace std;

// Constructor
SlideshowViewer::SlideshowViewer(QObject* pParent) :
    QObject(pParent),
    m_bOperationPending(false),
    m_bHasNextPicture(false),
    m_bHasPreviousPicture(false),
    m_pConnection(new AirPlayConnection(this)),
    m_pExecutor(new AirPlayCommandExecutor(m_pConnection, this)),
    m_pDevice(NULL),
    m_currentFileIndex(-1),
    m_transition(AirPlayConnection::AIRPLAY_TRANSITION_NONE),
    m_state(SLIDESHOW_STATE_STOPPED)
{
    // Start the AirPlay command executor
    m_pExecutor->start();

    // Connect the executor completion / error events
    QObject::connect(m_pExecutor, SIGNAL(commandStatus(const QString&)), this, SLOT(handleCommandStatus(const QString&)));
    QObject::connect(m_pExecutor, SIGNAL(commandsComplete()), this, SLOT(handleCommandsComplete()));
    QObject::connect(m_pExecutor, SIGNAL(commandsCancelled()), this, SLOT(handleCommandsCancelled()));
    QObject::connect(m_pExecutor, SIGNAL(error(const QString&)), this, SLOT(handleError(const QString&)));
}

// Destructor
SlideshowViewer::~SlideshowViewer()
{
    // Stop the AirPlay command executor
    m_pExecutor->stop();
}

// Set the device and password
void SlideshowViewer::setDevice(AirPlayDevice* pDevice, const QString& password)
{
    // Save parameters
    m_pDevice = pDevice;
    m_password = password;

    // Emit device changed signal
    emit deviceChanged();
}

// Set file list and transition
void SlideshowViewer::setSlideshowProperties(const QVariantList& fileList, int transition, int screenWidth, int screenHeight, int quality,bool scaleImages)
{
    // Copy slideshow information
    m_fileList = fileList;
    m_transition = transition;

    // Set screen dimensions
    m_pConnection->setScalingOptions(screenWidth, screenHeight, quality, scaleImages);
}

// Start the slideshow
bool SlideshowViewer::startSlideshow()
{
    // Retrieve settings from the device
    QHostAddress address(m_pDevice->address());
    int port = m_pDevice->port();

    // Create command list
    AirPlayCommandList commandList;

    // Create an open connection command
    QString commandStatus = "Connecting to " + m_pDevice->name();
    QSharedPointer<AirPlayCommand> command = QSharedPointer<AirPlayCommand>(new AirPlayOpenConnectionCommand(address, port, m_password, commandStatus));
    commandList.append(command);

    // Check if we have an initial photo
    if( m_fileList.length() > 0 )
    {
        // Set initial file index and retrieve entry fields
        m_currentFileIndex = 0;
        QVariantMap entry = m_fileList.value(m_currentFileIndex).toMap();
        QString assetID = entry.value("assetID").toString();
        QUrl url = entry.value("url").toUrl();
        QString type = entry.value("type").toString();

        // Upload initial photo
        commandStatus = "Caching picture";
        command = QSharedPointer<AirPlayCommand>(new AirPlayUploadPhotoCommand(assetID, url, type, commandStatus));
        commandList.append(command);

        // Display initial photo
        commandStatus = "Displaying picture";
        command = QSharedPointer<AirPlayCommand>(new AirPlayDisplayPhotoCommand(assetID, m_transition, commandStatus));
        commandList.append(command);
    }

    // Check if we have another picture
    if( (m_currentFileIndex + 1) < m_fileList.length() )
    {
        // Retrieve entry fields
        QVariantMap entry = m_fileList.value(m_currentFileIndex + 1).toMap();
        QString assetID = entry.value("assetID").toString();
        QUrl url = entry.value("url").toUrl();
        QString type = entry.value("type").toString();

        // Upload next photo
        commandStatus = "Caching picture";
        command = QSharedPointer<AirPlayCommand>(new AirPlayUploadPhotoCommand(assetID, url, type, commandStatus));
        commandList.append(command);
    }

    // Execute commands
    if( m_pExecutor->executeCommands(commandList) )
    {
        // Update state
        m_state = SLIDESHOW_STATE_STARTING;

        // Emit operation pending changed signal
        m_bOperationPending = true;
        emit operationPendingChanged();

        // Emit current picture changed signal
        emit currentPictureChanged();

        // Update next picture flag
        m_bHasNextPicture = (m_currentFileIndex < (m_fileList.length() - 1));
        emit hasNextPictureChanged();

        // Update previous picture flag
        m_bHasPreviousPicture = (m_currentFileIndex > 0);
        emit hasPreviousPictureChanged();

        return true;
    }
    else
    {
        // Update state
        m_state = SLIDESHOW_STATE_ERROR;

        return false;
    }
}

// Stop the slideshow
bool SlideshowViewer::stopSlideshow()
{
    // Create command list
    AirPlayCommandList commandList;
    QString commandStatus = "Disconnecting from " + m_pDevice->name();
    QSharedPointer<AirPlayCommand> command = QSharedPointer<AirPlayCommand>(new AirPlayCloseConnectionCommand(commandStatus));
    commandList.append(command);

    // Execute commands
    m_pExecutor->executeCommands(commandList);

    // Update state
    m_state = SLIDESHOW_STATE_STOPPED;

    // Reset the picture
    m_currentFileIndex = -1;

    // Update next picture flag
    m_bHasNextPicture = false;
    emit hasNextPictureChanged();

    // Update previous picture flag
    m_bHasPreviousPicture = false;
    emit hasPreviousPictureChanged();

    return true;
}

// Show next picture
bool SlideshowViewer::showNextPicture()
{
    // Create command list
    AirPlayCommandList commandList;
    QSharedPointer<AirPlayCommand> command;
    QString commandStatus;

    // Check if we have another picture
    if( (m_currentFileIndex + 1) < m_fileList.length() )
    {
        // Increment file index
        m_currentFileIndex++;

        // Retrieve entry fields
        QVariantMap entry = m_fileList.value(m_currentFileIndex).toMap();
        QString assetID = entry.value("assetID").toString();
        QUrl url = entry.value("url").toUrl();
        QString type = entry.value("type").toString();

        // Display current photo
        commandStatus = "Displaying next picture";
        command = QSharedPointer<AirPlayCommand>(new AirPlayDisplayPhotoCommand(assetID, m_transition, commandStatus));
        commandList.append(command);
    }

    // Check if we have another picture
    if( (m_currentFileIndex + 1) < m_fileList.length() )
    {
        // Retrieve entry fields
        QVariantMap entry = m_fileList.value(m_currentFileIndex + 1).toMap();
        QString assetID = entry.value("assetID").toString();
        QUrl url = entry.value("url").toUrl();
        QString type = entry.value("type").toString();

        // Upload next photo
        commandStatus = "Caching picture";
        command = QSharedPointer<AirPlayCommand>(new AirPlayUploadPhotoCommand(assetID, url, type, commandStatus));
        commandList.append(command);
    }

    // Execute commands
    if( m_pExecutor->executeCommands(commandList) )
    {
        // Update state and emit status
        m_state = SLIDESHOW_STATE_SENDING_PHOTO;

        // Emit operation pending changed signal
        m_bOperationPending = true;
        emit operationPendingChanged();

        // Emit current picture changed signal
        emit currentPictureChanged();

        // Update next picture flag
        m_bHasNextPicture = (m_currentFileIndex < (m_fileList.length() - 1));
        emit hasNextPictureChanged();

        // Update previous picture flag
        m_bHasPreviousPicture = (m_currentFileIndex > 0);
        emit hasPreviousPictureChanged();

        return true;
    }
    else
    {
        // Update state
        m_state = SLIDESHOW_STATE_ERROR;

        return false;
    }
}

// Show previous picture
bool SlideshowViewer::showPreviousPicture()
{
    // Create command list
    AirPlayCommandList commandList;
    QSharedPointer<AirPlayCommand> command;
    QString commandStatus;

    // Check if we have previous picture
    if( m_currentFileIndex > 0 )
    {
        // Decrement file index
        m_currentFileIndex--;

        // Retrieve entry fields
        QVariantMap entry = m_fileList.value(m_currentFileIndex).toMap();
        QString assetID = entry.value("assetID").toString();
        QUrl url = entry.value("url").toUrl();
        QString type = entry.value("type").toString();

        // Display current photo
        commandStatus = "Displaying previous picture";
        command = QSharedPointer<AirPlayCommand>(new AirPlayDisplayPhotoCommand(assetID, m_transition, commandStatus));
        commandList.append(command);
    }

    // Check if we have previous picture
    if( m_currentFileIndex > 0 )
    {
        // Retrieve entry fields
        QVariantMap entry = m_fileList.value(m_currentFileIndex - 1).toMap();
        QString assetID = entry.value("assetID").toString();
        QUrl url = entry.value("url").toUrl();
        QString type = entry.value("type").toString();

        // Upload previous photo
        commandStatus = "Caching picture";
        command = QSharedPointer<AirPlayCommand>(new AirPlayUploadPhotoCommand(assetID, url, type, commandStatus));
        commandList.append(command);
    }

    // Execute commands
    if( m_pExecutor->executeCommands(commandList) )
    {
        // Update state
        m_state = SLIDESHOW_STATE_SENDING_PHOTO;

        // Emit operation pending changed signal
        m_bOperationPending = true;
        emit operationPendingChanged();

        // Emit current picture changed signal
        emit currentPictureChanged();

        // Update next picture flag
        m_bHasNextPicture = (m_currentFileIndex < (m_fileList.length() - 1));
        emit hasNextPictureChanged();

        // Update previous picture flag
        m_bHasPreviousPicture = (m_currentFileIndex > 0);
        emit hasPreviousPictureChanged();

        return true;
    }
    else
    {
        // Update state
        m_state = SLIDESHOW_STATE_ERROR;

        return false;
    }
}

// Cancel operation in progress
bool SlideshowViewer::cancelOperation()
{
    // Cancel any pending operations
    if( !m_pExecutor->cancelCommands() )
    {
        // Update state and emit status
        m_state = SLIDESHOW_STATE_ERROR;

        // Emit operation pending changed signal
        m_bOperationPending = false;
        emit operationPendingChanged();

        // Emit cancellation signal
        emit operationCancelled();

        return false;
    }

    return true;
}

// Handler for command status
void SlideshowViewer::handleCommandStatus(const QString& status)
{
    // Update and emit status
    m_status = status;
    emit statusChanged();
}

// Handler for command completion
void SlideshowViewer::handleCommandsComplete()
{
    // Update state
    m_state = SLIDESHOW_STATE_PAUSED;

    // Emit operation pending changed signal
    m_bOperationPending = false;
    emit operationPendingChanged();

    // Emit completion signal
    emit operationComplete();
}

// Handler for command cancellation
void SlideshowViewer::handleCommandsCancelled()
{
    // Update state
    m_state = SLIDESHOW_STATE_PAUSED;

    // Emit operation pending changed signal
    m_bOperationPending = false;
    emit operationPendingChanged();

    // Emit cancellation signal
    emit operationCancelled();
}

// Handler for error
void SlideshowViewer::handleError(const QString& errorDescription)
{
    // Update state
    m_state = SLIDESHOW_STATE_ERROR;

    // Emit operation pending changed signal
    m_bOperationPending = false;
    emit operationPendingChanged();

    // Emit error signal
    emit error(errorDescription);
}

// Property accessor methods
AirPlayDevice* SlideshowViewer::device() const
{
    return m_pDevice;
}

// Property accessor methods
QVariant SlideshowViewer::currentPicture() const
{
    // Check if the current file is valid
    if( m_currentFileIndex >= 0 )
    {
        return m_fileList.value(m_currentFileIndex);
    }
    else
    {
        return QVariant();
    }
}

// Property accessor methods
QString SlideshowViewer::status() const
{
    return m_status;
}

// Property accessor methods
bool SlideshowViewer::operationPending() const
{
     return m_bOperationPending;
}

// Property accessor methods
bool SlideshowViewer::hasNextPicture() const
{
    return m_bHasNextPicture;
}

// Property accessor methods
bool SlideshowViewer::hasPreviousPicture() const
{
    return m_bHasPreviousPicture;
}
