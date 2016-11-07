/*
 * BonjourResolver.cpp
 *
 *  Created on: Apr 8, 2013
 *      Author: jgordy
 */

#include <QtCore\QStringList>
#include <iostream>
#include "BonjourResolver.hpp"

// Process and save the TXT record
static QVariantMap processTxtRecord(const unsigned char* txtRecord, quint16 txtLength)
{
    QVariantMap txtRecordMap;

    // Loop until we've processed all pairs
    while(txtLength > 0)
    {
        // Read the string size byte
        int stringLength = (int) *txtRecord;
        txtRecord += 1;
        txtLength -= 1;

        // Read the string, parse the <key, value> pair, and add to map
        QString strEntry(QByteArray((const char*) txtRecord, stringLength));
        QStringList strKeyValueList = strEntry.split(QChar('='), QString::KeepEmptyParts);
        if( strKeyValueList.length() == 2 )
        {
            QString strKey = strKeyValueList.first();
            QString strValue = strKeyValueList.last();
            txtRecordMap.insert(strKey, strValue);
        }

        // Update the pointer and length
        txtRecord += stringLength;
        txtLength -= stringLength;
    }

    return txtRecordMap;
}

// Constructor
BonjourResolver::BonjourResolver(QObject* pParent) :
    QObject(pParent),
    m_serviceRef(NULL),
    m_pSocket(NULL),
    m_addressServiceRef(NULL),
    m_pAddressSocket(NULL),
    m_port(-1)
{
}

// Destructor
BonjourResolver::~BonjourResolver()
{
    // Clean up resolver
    cleanupResolve();
}

bool BonjourResolver::resolveRecord(const BonjourRecord& record)
{
    // Check if we're already resolving
    if( m_serviceRef )
    {
        return false;
    }

    // Initialize resolution service
    DNSServiceErrorType err = DNSServiceResolve(&m_serviceRef, 0, 0, record.name().toUtf8().constData(), record.serviceType().toUtf8().constData(), record.domain().toUtf8().constData(), bonjourResolveReply, this);
    if( err == kDNSServiceErr_NoError )
    {
        // Retrieve socket file descriptor
        int sockfd = DNSServiceRefSockFD(m_serviceRef);
        if( sockfd != -1 )
        {
            // Save name
            m_name = record.name();

            // Create a new socket notifier to listen for data
            m_pSocket = new QSocketNotifier(sockfd, QSocketNotifier::Read, this);
            connect(m_pSocket, SIGNAL(activated(int)), this, SLOT(bonjourSocketReadyRead()));
        }
        else
        {
            // Emit error signal
            emit error(kDNSServiceErr_Invalid);
            return false;
        }
    }
    else
    {
        // Emit error signal
        emit error(err);
        return false;
    }

    return true;
}

// Resolve address
bool BonjourResolver::resolveAddress(const QString& hostName)
{
    // Check if we're already resolving
    if( m_addressServiceRef )
    {
        return false;
    }

    // Initialize resolution service
    DNSServiceErrorType err = DNSServiceGetAddrInfo(&m_addressServiceRef, 0, 0, kDNSServiceProtocol_IPv4, hostName.toUtf8().constData(), bonjourAddressResolveReply, this);
    if( err == kDNSServiceErr_NoError )
    {
        // Retrieve socket file descriptor
        int sockfd = DNSServiceRefSockFD(m_addressServiceRef);
        if( sockfd != -1 )
        {
            // Create a new socket notifier to listen for data
            m_pAddressSocket = new QSocketNotifier(sockfd, QSocketNotifier::Read, this);
            connect(m_pAddressSocket, SIGNAL(activated(int)), this, SLOT(bonjourAddressSocketReadyRead()));
        }
        else
        {
            // Emit error signal
            emit error(kDNSServiceErr_Invalid);
            return false;
        }
    }
    else
    {
        // Emit error signal
        emit error(err);
        return false;
    }

    return true;
}

// Handler for socket read notification
void BonjourResolver::bonjourSocketReadyRead()
{
    // Process result
    DNSServiceErrorType err = DNSServiceProcessResult(m_serviceRef);
    if( err != kDNSServiceErr_NoError )
    {
        // Emit error signal
        emit error(err);
    }
}

// Handler for address socket read notification
void BonjourResolver::bonjourAddressSocketReadyRead()
{
    // Process result
    DNSServiceErrorType err = DNSServiceProcessResult(m_addressServiceRef);
    if( err != kDNSServiceErr_NoError )
    {
        // Emit error signal
        emit error(err);
    }
}

// Handler to cleanup resolving
void BonjourResolver::cleanupResolve()
{
    // Release the socket
    if( m_pAddressSocket )
    {
        delete m_pAddressSocket;
        m_pAddressSocket = NULL;
    }

    // Release the service reference
    if( m_addressServiceRef )
    {
        DNSServiceRefDeallocate(m_addressServiceRef);
        m_addressServiceRef = NULL;
    }

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
        m_serviceRef = NULL;
    }

    // Clear the service record
    m_name.clear();
    m_port = -1;
    m_txtRecord.clear();
}

// Callback function for resolution replies
void BonjourResolver::bonjourResolveReply(
    DNSServiceRef serviceRef, DNSServiceFlags flags, quint32, DNSServiceErrorType errorCode,
    const char*, const char* hostTarget, quint16 port, quint16 txtLength,
    const unsigned char* txtRecord, void* context)
{
    // Retrieve the resolver instance
    BonjourResolver* pResolver = static_cast<BonjourResolver*>(context);
    if( errorCode == kDNSServiceErr_NoError )
    {
        // Save port
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        port = ((port & 0x00ff) << 8) | ((port & 0xff00) >> 8);
#endif
        pResolver->m_port = port;

        // Process and save the TXT record
        pResolver->m_txtRecord = processTxtRecord(txtRecord, txtLength);

        // Perform host lookup
        pResolver->resolveAddress(QString::fromUtf8(hostTarget));
    }
    else
    {
        // Emit error signal
        emit pResolver->error(errorCode);
    }
}

// Callback function for address resolution replies
void DNSSD_API BonjourResolver::bonjourAddressResolveReply(
    DNSServiceRef serviceRef, DNSServiceFlags flags, quint32 interfaceIndex, DNSServiceErrorType errorCode,
    const char* hostname, const struct sockaddr* address, quint32, void* context)
{
    // Retrieve the resolver instance
    BonjourResolver* pResolver = static_cast<BonjourResolver*>(context);
    if( errorCode == kDNSServiceErr_NoError )
    {
        // Construct the host info
        QHostInfo hostInfo;
        hostInfo.setHostName(QString::fromUtf8(hostname));
        QList<QHostAddress> addressList;
        addressList.append(QHostAddress(address));
        hostInfo.setAddresses(addressList);

        // Emit the resolution signal
        emit pResolver->recordResolved(pResolver->m_name, hostInfo, pResolver->m_port, pResolver->m_txtRecord);

        // Invoke the cleanup method later
        QMetaObject::invokeMethod(pResolver, "cleanupResolve", Qt::QueuedConnection);
    }
    else
    {
        // Emit error signal
        emit pResolver->error(errorCode);
    }
}
