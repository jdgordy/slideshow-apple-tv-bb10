/*
 * AirPlayCommandExecutor.cpp
 *
 *  Created on: May 1, 2013
 *      Author: jgordy
 */

#include "AirPlayCommandExecutor.hpp"
#include "AirPlayCommand.hpp"
#include "AirPlayCommandWorker.hpp"
#include <iostream>

// Declare namespaces
using namespace std;

// Constructor
AirPlayCommandExecutor::AirPlayCommandExecutor(AirPlayConnection* pConnection, QObject* pParent) :
    QObject(pParent),
    m_pConnection(pConnection),
    m_pThread(NULL),
    m_pWorker(NULL)
{
}

// Destructor
AirPlayCommandExecutor::~AirPlayCommandExecutor()
{
}

// Start worker
bool AirPlayCommandExecutor::start()
{
    // Check if we are already started
    if( (m_pThread == NULL) && (m_pWorker == NULL) )
    {
        // Create new worker instance and thread
        m_pWorker = new AirPlayCommandWorker(m_pConnection);
        m_pThread = new QThread();

        // Connect the worker completion / error events
        QObject::connect(m_pWorker, SIGNAL(commandStatus(const QString&)), this, SLOT(handleCommandStatus(const QString&)));
        QObject::connect(m_pWorker, SIGNAL(commandsComplete()), this, SLOT(handleCommandsComplete()));
        QObject::connect(m_pWorker, SIGNAL(commandsCancelled()), this, SLOT(handleCommandsCancelled()));
        QObject::connect(m_pWorker, SIGNAL(error(const QString&)), this, SLOT(handleError(const QString&)));

        // Start the worker thread
        m_pWorker->moveToThread(m_pThread);
        m_pThread->start();

        // Log debug info
        cout << "AirPlayCommandExecutor::start" << endl;
        return true;
    }
    else
    {
        // Log error
        cout << "AirPlayCommandExecutor::start -- Already started" << endl;
        return false;
    }
}

// Stop worker
bool AirPlayCommandExecutor::stop()
{
    // Check if we are started
    if( (m_pThread != NULL) && (m_pWorker != NULL) )
    {
        // Stop and release the worker
        m_pWorker->deleteLater();
        m_pWorker = NULL;

        // Stop and release the thread
        m_pThread->quit();
        m_pThread->wait();
        delete m_pThread;
        m_pThread = NULL;

        // Log debug info
        cout << "AirPlayCommandExecutor::stop" << endl;
        return true;
    }
    else
    {
        // Log error
        cout << "AirPlayCommandExecutor::stop -- Not started" << endl;
        return false;
    }
}

// Execute command list
bool AirPlayCommandExecutor::executeCommands(const AirPlayCommandList& commandList)
{
    // Check if we are started
    if( (m_pThread != NULL) && (m_pWorker != NULL) )
    {
        // Execute commands
        if( m_pWorker->executeCommands(commandList) )
        {
            // Log debug info
            cout << "AirPlayCommandExecutor::executeCommands" << endl;
            return true;
        }
        else
        {
            // Log error
            cout << "AirPlayCommandExecutor::executeCommands -- Unable to execute commands" << endl;
            return false;
        }
    }
    else
    {
        // Log error
        cout << "AirPlayCommandExecutor::executeCommands -- Not started" << endl;
        return false;
    }
}

// Cancel commands
bool AirPlayCommandExecutor::cancelCommands()
{
    // Check if we are started
    if( (m_pThread != NULL) && (m_pWorker != NULL) )
    {
        // Cancel commands
        if( m_pWorker->cancelCommands() )
        {
            // Log debug info
            cout << "AirPlayCommandExecutor::cancelCommands" << endl;
            return true;
        }
        else
        {
            // Log error
            cout << "AirPlayCommandExecutor::cancelCommands -- Unable to cancel commands" << endl;
            return false;
        }
    }
    else
    {
        // Log error
        cout << "AirPlayCommandExecutor::cancelCommands -- Not started" << endl;
        return false;
    }
}

// Handler for command status
void AirPlayCommandExecutor::handleCommandStatus(const QString& status)
{
    // Emit command status update
    emit commandStatus(status);
}

// Handler for command completion
void AirPlayCommandExecutor::handleCommandsComplete()
{
    // Emit command completion signal
    emit commandsComplete();
}

// Handler for command cancellation
void AirPlayCommandExecutor::handleCommandsCancelled()
{
    // Emit command cancelled signal
    emit commandsCancelled();
}

// Handler for error
void AirPlayCommandExecutor::handleError(const QString& errorDescription)
{
    // Emit error signal
    emit error(errorDescription);
}
