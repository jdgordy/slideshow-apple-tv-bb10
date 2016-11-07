/*
 * BonjourBrowser.hpp
 *
 *  Created on: Apr 8, 2013
 *      Author: jgordy
 */

#ifndef BONJOURBROWSER_HPP_
#define BONJOURBROWSER_HPP_

#include <QtCore/QObject>
#include <QtCore/QSocketNotifier>
#include <QtCore/QList>
#include "BonjourRecord.hpp"
#include "dns_sd.h"

//
// BonjourBrowser
//
class BonjourBrowser : public QObject
{
    Q_OBJECT;

public:

    // Constructor / destructor
    BonjourBrowser(QObject* pParent = NULL);
    virtual ~BonjourBrowser();

    // Start browsing for services
    void browseForServiceType(const QString& serviceType);

    // Stop browsing
    void stopBrowsing();

    // Retrieve current records
    QList<BonjourRecord> currentRecords() const;

    // Retrieve current browsing type
    QString serviceType() const;

Q_SIGNALS:

    // Emitted when records change
    void browserRecordsChanged(const QList<BonjourRecord>& recordList);

    // Emitted on error
    void error(DNSServiceErrorType err);

protected Q_SLOTS:

    // Handler for socket read notification
    void bonjourSocketReadyRead();

protected:

    // Callback function for browser replies
    static void DNSSD_API bonjourBrowseReply(DNSServiceRef,
                DNSServiceFlags, quint32, DNSServiceErrorType,
                const char*, const char*, const char*, void*);

    // Service reference
    DNSServiceRef m_serviceRef;
    QSocketNotifier* m_pSocket;

    // Service type and corresponding records
    QString m_serviceType;
    QList<BonjourRecord> m_recordList;
};

#endif /* BONJOURBROWSER_HPP_ */
