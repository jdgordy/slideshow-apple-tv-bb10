/*
 * BonjourResolver.hpp
 *
 *  Created on: Apr 8, 2013
 *      Author: jgordy
 */

#ifndef BONJOURRESOLVER_HPP_
#define BONJOURRESOLVER_HPP_

#include <QtCore/QObject>
#include <QtCore/QSocketNotifier>
#include <QtCore/QVariantMap>
#include <QtNetwork/QHostInfo>
#include "BonjourRecord.hpp"
#include "dns_sd.h"

//
// BonjourResolver
//
class BonjourResolver : public QObject
{
    Q_OBJECT;

public:

    // Constructor / destructor
    BonjourResolver(QObject* pParent = NULL);
    virtual ~BonjourResolver();

    // Resolve record
    bool resolveRecord(const BonjourRecord& record);

    // Resolve address
    bool resolveAddress(const QString& hostName);

Q_SIGNALS:

    // Emitted when record is resolved
    void recordResolved(const QString& name, const QHostInfo& hostInfo, int port, const QVariantMap& txtRecord);

    // Emitted on error
    void error(DNSServiceErrorType err);

protected Q_SLOTS:

    // Handlers for socket read notifications
    void bonjourSocketReadyRead();
    void bonjourAddressSocketReadyRead();

    // Handler to cleanup resolving
    void cleanupResolve();

protected:

    // Callback function for service resolution replies
    static void DNSSD_API bonjourResolveReply(
        DNSServiceRef, DNSServiceFlags, quint32, DNSServiceErrorType,
        const char*, const char*, quint16, quint16,
        const unsigned char*, void*);

    // Callback function for address resolution replies
    static void DNSSD_API bonjourAddressResolveReply(
        DNSServiceRef, DNSServiceFlags, quint32, DNSServiceErrorType,
        const char*, const struct sockaddr*, quint32, void*);

    // Service records
    DNSServiceRef m_serviceRef;
    QSocketNotifier* m_pSocket;

    // Address resolution records
    DNSServiceRef m_addressServiceRef;
    QSocketNotifier* m_pAddressSocket;

    // Resolved service details
    QString     m_name;
    int         m_port;
    QVariantMap m_txtRecord;
};

#endif /* BONJOURRESOLVER_HPP_ */
