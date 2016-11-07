/*
 * BonjourRegistrar.hpp
 *
 *  Created on: Apr 8, 2013
 *      Author: jgordy
 */

#ifndef BONJOURREGISTRAR_HPP_
#define BONJOURREGISTRAR_HPP_

#include <QtCore/QObject>
#include <QtCore/QVariantMap>
#include <QtCore/QSocketNotifier>
#include "BonjourRecord.hpp"
#include "dns_sd.h"

//
// BonjourRegistrar
//
class BonjourRegistrar : public QObject
{
    Q_OBJECT;

public:

    // Constructor / destructor
    BonjourRegistrar(QObject* pParent = NULL);
    virtual ~BonjourRegistrar();

    // Register service
    bool registerService(const BonjourRecord& record, quint16 servicePort, const QVariantMap& txtRecord);

    // Unregister service
    void unregisterService();

    // Retrieve registered record
    BonjourRecord registeredRecord() const;

Q_SIGNALS:

    // Emitted when service is registered
    void serviceRegistered(const BonjourRecord& record);

    // Emitted on error
    void error(DNSServiceErrorType err);

protected Q_SLOTS:

    // Handler for socket read notification
    void bonjourSocketReadyRead();

protected:

    // Callback function for registration replies
    static void DNSSD_API bonjourRegisterService(
                DNSServiceRef, DNSServiceFlags,
                DNSServiceErrorType, const char*, const char*,
                const char*, void*);

    // Service records
    DNSServiceRef m_serviceRef;
    QSocketNotifier* m_pSocket;
    BonjourRecord m_record;
};

#endif /* BONJOURBROWSER_HPP_ */
