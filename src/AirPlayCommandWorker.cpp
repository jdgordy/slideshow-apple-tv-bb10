/*
 * AirPlayCommandWorker.cpp
 *
 *  Created on: May 1, 2013
 *      Author: jgordy
 */

#include "AirPlayCommandWorker.hpp"
#include "AirPlayConnection.hpp"
#include <iostream>

// Declare namespaces
using namespace std;

// Constructor
AirPlayCommandWorker::AirPlayCommandWorker(AirPlayConnection* pConnection, QObject* pParent) :
    QObject(pParent),
    m_pConnection(pConnection),
    m_state(AIRPLAY_COMMAND_STATE_IDLE)
{
    // Connect us to the AirPlay connection signals
    QObject::connect(m_pConnection, SIGNAL(operationComplete()), this, SLOT(handleOperationComplete()));
    QObject::connect(m_pConnection, SIGNAL(operationCancelled()), this, SLOT(handleOperationCancelled()));
    QObject::connect(m_pConnection, SIGNAL(error(const QString&)), this, SLOT(handleError(const QString&)));
}

// Destructor
AirPlayCommandWorker::~AirPlayCommandWorker()
{
}

// Execute command list
bool AirPlayCommandWorker::executeCommands(const AirPlayCommandList& commandList)
{
    // Lock the command mutex
    QMutexLocker lock(&m_mutex);

    // Check if we're already busy
    if( m_state == AIRPLAY_COMMAND_STATE_IDLE )
    {
        // Copy the commands
        m_commandList = commandList;

        // Start executing first command
        if( executeNextCommand() )
        {
            // Update state
            m_state = AIRPLAY_COMMAND_STATE_EXECUTING;

            // Log debug info
            cout << "AirPlayCommandWorker::executeCommands" << endl;
            return true;
        }
        else
        {
            // Clear command list
            m_commandList.clear();

            // Log error
            cout << "AirPlayCommandWorker::executeCommands -- Unable to execute command" << endl;
            return false;
        }
    }
    else
    {
        // Log error
        cout << "AirPlayCommandWorker::executeCommands -- Already busy" << endl;
        return false;
    }
}

// Cancel commands
bool AirPlayCommandWorker::cancelCommands()
{
    // Lock the command mutex
    QMutexLocker lock(&m_mutex);

    // Check if we're executing commands
    if( m_state == AIRPLAY_COMMAND_STATE_EXECUTING )
    {
        // Cancel pending command
        if( m_pConnection->cancelOperation() )
        {
            // Clear the command list
            m_commandList.clear();

            // Update state
            m_state = AIRPLAY_COMMAND_STATE_CANCELLING;

            // Log debug info
            cout << "AirPlayCommandWorker::cancelCommands" << endl;
            return true;
        }
        else
        {
            // Log error
            cout << "AirPlayCommandWorker::cancelCommands -- Unable to cancel commands" << endl;
            return false;
        }
    }
    else
    {
        // Log error
        cout << "AirPlayCommandWorker::cancelCommands -- Not executing commands" << endl;
        return false;
    }
}

// Handler for operation completion
void AirPlayCommandWorker::handleOperationComplete()
{
    // Lock the command mutex
    QMutexLocker lock(&m_mutex);

    // Check if we have more commands to execute
    if( !m_commandList.isEmpty() )
    {
        // Execute next command
        if( executeNextCommand() )
        {
            // Log debug info
            cout << "AirPlayCommandWorker::handleOperationComplete" << endl;
        }
        else
        {
            // Clear remaining commands
            m_commandList.clear();

            // Log error
            cout << "AirPlayCommandWorker::handleOperationComplete -- Unable to execute command" << endl;
        }
    }
    else
    {
        // Update the state
        m_state = AIRPLAY_COMMAND_STATE_IDLE;

        // We are finished all commands
        emit commandsComplete();

        // Log debug info
        cout << "AirPlayCommandWorker::handleOperationComplete -- All commands complete" << endl;
    }
}

// Handler for operation cancellation
void AirPlayCommandWorker::handleOperationCancelled()
{
    // Lock the command mutex
    QMutexLocker lock(&m_mutex);

    // Clear the command list
    m_commandList.clear();

    // Update the state
    m_state = AIRPLAY_COMMAND_STATE_IDLE;

    // Emit cancellation signal
    emit commandsCancelled();

    // Log debug info
    cout << "AirPlayCommandWorker::handleOperationCancelled" << endl;
}

// Handler for operation error
void AirPlayCommandWorker::handleError(const QString& errorDescription)
{
    // Lock the command mutex
    QMutexLocker lock(&m_mutex);

    // Clear the command list
    m_commandList.clear();

    // Update the state
    m_state = AIRPLAY_COMMAND_STATE_IDLE;

    // Emit error signal
    emit error(errorDescription);

    // Log debug info
    cout << "AirPlayCommandWorker::handleError -- " << errorDescription.toStdString() << endl;
}

// Execute next command
bool AirPlayCommandWorker::executeNextCommand()
{
    // Retrieve next command
    QSharedPointer<AirPlayCommand> pCommand = m_commandList.takeFirst();
    if( !pCommand.isNull() )
    {
        // Retrieve and emit command status
        emit(commandStatus(pCommand->getCommandStatus()));

        // Execute command
        if( pCommand->execute(m_pConnection) )
        {
            // Log debug info
            cout << "AirPlayCommandWorker::executeNextCommand" << endl;
            return true;
        }
        else
        {
            // Log error
            cout << "AirPlayCommandWorker::executeNextCommand -- Unable to execute command" << endl;
            return false;
        }
    }
    else
    {
        // Log error
        cout << "AirPlayCommandWorker::executeNextCommand -- No commands to execute" << endl;
        return false;
    }
}
