/*
 * BonjourBrowser.cpp
 *
 *  Created on: Apr 8, 2013
 *      Author: jgordy
 */

#include "BonjourBrowser.hpp"

// Constructor
BonjourBrowser::BonjourBrowser(QObject* pParent) :
    QObject(pParent),
    m_serviceRef(NULL),
    m_pSocket(NULL)
{
}

// Destructor
BonjourBrowser::~BonjourBrowser()
{
    // Stop browsing
    stopBrowsing();
}

// Browse for services
void BonjourBrowser::browseForServiceType(const QString& serviceType)
{
    // Initialize browser
    DNSServiceErrorType err = DNSServiceBrowse(&m_serviceRef, 0, 0, serviceType.toUtf8().constData(), 0, bonjourBrowseReply, this);
    if( err == kDNSServiceErr_NoError )
    {
        // Retrieve socket file descriptor
        int sockfd = DNSServiceRefSockFD(m_serviceRef);
        if( sockfd != -1 )
        {
            // Create a new socket notifier to listen for data
            m_pSocket = new QSocketNotifier(sockfd, QSocketNotifier::Read, this);
            connect(m_pSocket, SIGNAL(activated(int)), this, SLOT(bonjourSocketReadyRead()));
        }
        else
        {
            // Emit error signal
            emit error(kDNSServiceErr_Invalid);
        }
    }
    else
    {
        // Emit error signal
        emit error(err);
    }
}

// Stop browsing
void BonjourBrowser::stopBrowsing()
{
    // Release the socket
    if( m_pSocket )
    {
        delete m_pSocket;
        m_pSocket = NULL;
    }

    // Release the service reference
    if( m_serviceRef )
    {
        DNSServiceRefDeallocate(m_serviceRef);
        m_serviceRef = 0;
    }

    // Clear the records
    m_recordList.clear();
}


// Retrieve current records
QList<BonjourRecord> BonjourBrowser::currentRecords() const
{
    return m_recordList;
}

// Retrieve current browsing type
QString BonjourBrowser::serviceType() const
{
    return m_serviceType;
}

// Handler for socket read notification
void BonjourBrowser::bonjourSocketReadyRead()
{
    // Process result
    DNSServiceErrorType err = DNSServiceProcessResult(m_serviceRef);
    if( err != kDNSServiceErr_NoError )
    {
        // Emit error signal
        emit error(err);
    }
}

// Callback function for browser replies
void BonjourBrowser::bonjourBrowseReply(
    DNSServiceRef serviceRef, DNSServiceFlags flags, quint32 interfaceIndex, DNSServiceErrorType errorCode,
    const char* serviceName, const char* regType, const char* domain, void* context)
{
    // Retrieve the browser instance
    BonjourBrowser* pBrowser = static_cast<BonjourBrowser*>(context);
    if( errorCode == kDNSServiceErr_NoError )
    {
        // Create a new record
        BonjourRecord record(serviceName, regType, domain);
        if( flags & kDNSServiceFlagsAdd )
        {
            if( !pBrowser->m_recordList.contains(record) )
            {
                pBrowser->m_recordList.append(record);
            }
        }
        else
        {
            pBrowser->m_recordList.removeAll(record);
        }

        // Check if more records coming
        if( !(flags & kDNSServiceFlagsMoreComing))
        {
            // Emit records changed signal
            emit pBrowser->browserRecordsChanged(pBrowser->m_recordList);
        }
    }
    else
    {
        // Emit error signal
        emit pBrowser->error(errorCode);
    }
}
