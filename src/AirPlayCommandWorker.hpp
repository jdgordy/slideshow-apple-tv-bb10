/*
 * AirPlayCommandWorker.hpp
 *
 *  Created on: May 3, 2013
 *      Author: jgordy
 */

#ifndef AIRPLAYCOMMANDWORKER_HPP_
#define AIRPLAYCOMMANDWORKER_HPP_

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QMutex>
#include "AirPlayCommand.hpp"

// Forward class declarations
class AirPlayConnection;

//
// AirPlayCommandWorker
//
class AirPlayCommandWorker : public QObject
{
    Q_OBJECT

public:

    // Constants
    static const int AIRPLAY_COMMAND_STATE_IDLE = 0;
    static const int AIRPLAY_COMMAND_STATE_EXECUTING = 1;
    static const int AIRPLAY_COMMAND_STATE_CANCELLING = 2;

    // Constructor / destructor
    AirPlayCommandWorker(AirPlayConnection* pConnection, QObject* pParent = NULL);
    virtual ~AirPlayCommandWorker();

    // Execute command list
    bool executeCommands(const AirPlayCommandList& commandList);

    // Cancel commands
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

    // Handler for operation completion
    void handleOperationComplete();

    // Handler for operation cancellation
    void handleOperationCancelled();

    // Handler for operation error
    void handleError(const QString& errorDescription);

protected:

    // Execute next command
    bool executeNextCommand();

    // AirPlay connection instance
    AirPlayConnection*  m_pConnection;

    // State and command variables
    QMutex              m_mutex;
    int                 m_state;
    AirPlayCommandList  m_commandList;
};

#endif /* AIRPLAYCOMMANDWORKER_HPP_ */
