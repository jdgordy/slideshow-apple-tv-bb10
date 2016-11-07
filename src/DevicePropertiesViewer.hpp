/*
 * DevicePropertiesViewer.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: jgordy
 */

#ifndef DEVICEPROPERTIESVIEWER_HPP_
#define DEVICEPROPERTIESVIEWER_HPP_

#include <QtCore/QObject>
#include <QtCore/QVariantList>
#include <QtCore/QVariantMap>
#include <QtNetwork/QHostInfo>
#include "BonjourRecord.hpp"
#include "dns_sd.h"

// Forward class declarations
class AirPlayDevice;
class BonjourResolver;

//
// DevicePropertiesViewer
//
class DevicePropertiesViewer : public QObject
{
    Q_OBJECT

    // The current device
    Q_PROPERTY(AirPlayDevice* device READ device NOTIFY deviceChanged);

public:

    // Constructor / destructor
    DevicePropertiesViewer(QObject* pParent = NULL);
    virtual ~DevicePropertiesViewer();

    // Set the device record and trigger resolution
    void setDeviceRecord(const BonjourRecord& record);

Q_SIGNALS:

    // Property change notification signals
    void deviceChanged();

protected Q_SLOTS:

    // Handler for resolver replies
    void handleRecordResolved(const QString& fullName, const QHostInfo& hostInfo, int port, const QVariantMap& txtRecord);

    // Handler for resolver errors
    void handleError(DNSServiceErrorType err);

protected:

    // Property accessor methods
    AirPlayDevice* device() const;

    // Bonjour resolver instance
    BonjourResolver*    m_pResolver;

    // AirPlay device instance
    AirPlayDevice*      m_pDevice;
};

#endif /* DEVICEPROPERTIESVIEWER_HPP_ */
