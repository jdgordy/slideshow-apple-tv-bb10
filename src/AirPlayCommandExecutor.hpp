/*
 * AirPlayCommandExecutor.hpp
 *
 *  Created on: May 1, 2013
 *      Author: jgordy
 */

#ifndef AIRPLAYCOMMANDEXECUTOR_HPP_
#define AIRPLAYCOMMANDEXECUTOR_HPP_

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QThread>
#include "AirPlayCommand.hpp"

// Forward class declarations
class AirPlayConnection;
class AirPlayCommandWorker;

//
// AirPlayCommandExecutor
//
class AirPlayCommandExecutor : public QObject
{
    Q_OBJECT

public:

    // Constructor / destructor
    AirPlayCommandExecutor(AirPlayConnection* pConnection, QObject* pParent = NULL);
    virtual ~AirPlayCommandExecutor();

    // Start worker
    bool start();

    // Stop worker
    bool stop();

    // Execute command list
    bool executeCommands(const AirPlayCommandList& commandList);

    // Cancel command list
    bool cancelCommands();

Q_SIGNALS:

    // Emitted on command status
    void commandStatus(const QString& status);

    // Emitted on command completion
    void commandsComplete();

    // Emitted on command cancellation
    void commandsCancelled();

    // Emitted on error
    void error(const QString& errorDescription);

protected Q_SLOTS:

    // Handler for command status
    void handleCommandStatus(const QString& status);

    // Handler for command completion
    void handleCommandsComplete();

    // Handler for command cancellation
    void handleCommandsCancelled();

    // Handler for error
    void handleError(const QString& errorDescription);

protected:

    // AirPlay connection instance
    AirPlayConnection*  m_pConnection;

    // Command thread and worker
    QThread*                m_pThread;
    AirPlayCommandWorker*   m_pWorker;
};

#endif /* AIRPLAYCOMMANDEXECUTOR_HPP_ */
