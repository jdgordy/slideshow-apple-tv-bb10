/*
 * BonjourRegistrar.cpp
 *
 *  Created on: Apr 8, 2013
 *      Author: jgordy
 */

#include "BonjourRegistrar.hpp"

// Process and save the TXT record
static QByteArray createTxtRecord(const QVariantMap& txtRecord)
{
    QByteArray array;

    // Check if we have entries
    if( txtRecord.size() > 0 )
    {
        // Loop over all entries
        foreach( QVariant entry, txtRecord )
        {
            // Concatenate <key, value> pair
            QString key = txtRecord.key(entry);
            QString value = txtRecord.value(key).toString();
            QString keyValue = key + "=" + value;

            // Write the size byte
            array.append((char) keyValue.length());
            array.append(keyValue);
        }
    }
    else
    {
        // Write a single empty string
        array.append((char) 0);
    }

    return array;
}


// Constructor
BonjourRegistrar::BonjourRegistrar(QObject* pParent) :
    QObject(pParent),
    m_serviceRef(NULL),
    m_pSocket(NULL)
{
}

// Destructor
BonjourRegistrar::~BonjourRegistrar()
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
        m_serviceRef = NULL;
    }
}

// Register service
bool BonjourRegistrar::registerService(const BonjourRecord& record, quint16 servicePort, const QVariantMap& txtRecord)
{
    // Check if we're already registered
    if( m_serviceRef )
    {
        return false;
    }

    // Encode TXT record to byte array
    QByteArray txtRecordArray = createTxtRecord(txtRecord);

    // Convert port to network order
    quint16 bigEndianPort = servicePort;
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    bigEndianPort = ((servicePort & 0x00ff) << 8) | ((servicePort & 0xff00) >> 8);
#endif

    // Initialize registration service
    DNSServiceErrorType err = DNSServiceRegister(
        &m_serviceRef, 0, 0,
        record.name().toUtf8().constData(), record.serviceType().toUtf8().constData(), record.domain().isEmpty() ? NULL : record.domain().toUtf8().constData(),
        0, bigEndianPort, txtRecordArray.length(), txtRecordArray.data(), bonjourRegisterService, this);
    if( err == kDNSServiceErr_NoError )
    {
        // Retrieve socket file descriptor
        int sockfd = DNSServiceRefSockFD(m_serviceRef);
        if( sockfd != -1 )
        {
            // Create a new socket notifier to listen for data
            m_pSocket = new QSocketNotifier(sockfd, QSocketNotifier::Read, this);
            connect(m_pSocket, SIGNAL(activated(int)), this, SLOT(bonjourSocketReadyRead()));
            return true;
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
}

// Unregister service
void BonjourRegistrar::unregisterService()
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

    // Clear the record
    m_record = BonjourRecord();
}

// Retrieve registered record
BonjourRecord BonjourRegistrar::registeredRecord() const
{
    return m_record;
}

// Handler for socket read notification
void BonjourRegistrar::bonjourSocketReadyRead()
{
    // Process result
    DNSServiceErrorType err = DNSServiceProcessResult(m_serviceRef);
    if( err != kDNSServiceErr_NoError )
    {
        // Emit error signal
        emit error(err);
    }
}

// Callback function for registration replies
void BonjourRegistrar::bonjourRegisterService(
    DNSServiceRef serviceRef, DNSServiceFlags flags, DNSServiceErrorType errorCode,
    const char* name, const char* regType, const char* domain, void* context)
{
    // Retrieve the registrar instance
    BonjourRegistrar *pRegistrar = static_cast<BonjourRegistrar *>(context);
    if( errorCode == kDNSServiceErr_NoError )
    {
        // Initialize the record
        pRegistrar->m_record = BonjourRecord(QString::fromUtf8(name), QString::fromUtf8(regType), QString::fromUtf8(domain));

        // Emit the registration signal
        emit pRegistrar->serviceRegistered(pRegistrar->m_record);
    }
    else
    {
        // Emit error signal
        emit pRegistrar->error(errorCode);
    }
}
