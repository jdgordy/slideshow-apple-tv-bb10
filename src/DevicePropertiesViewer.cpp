/*
 * DevicePropertiesViewer.cpp
 *
 *  Created on: Apr 16, 2013
 *      Author: jgordy
 */

// Default empty project template
#include "DevicePropertiesViewer.hpp"
#include "BonjourResolver.hpp"
#include "AirPlayDevice.hpp"
#include <iostream>

using namespace std;

// Constructor
DevicePropertiesViewer::DevicePropertiesViewer(QObject* pParent) :
    QObject(pParent),
    m_pResolver(new BonjourResolver(this)),
    m_pDevice(NULL)
{
    // Create the resolver instance and connect us for replies and errors
    QObject::connect(m_pResolver, SIGNAL(recordResolved(const QString&, const QHostInfo&, int, const QVariantMap&)), this, SLOT(handleRecordResolved(const QString&, const QHostInfo&, int, const QVariantMap&)));
    QObject::connect(m_pResolver, SIGNAL(error(DNSServiceErrorType)), this, SLOT(handleError(DNSServiceErrorType)));
}

// Destructor
DevicePropertiesViewer::~DevicePropertiesViewer()
{
}

// Set the device record and trigger resolution
void DevicePropertiesViewer::setDeviceRecord(const BonjourRecord& record)
{
    // Clear old device
    if( m_pDevice )
    {
        delete m_pDevice;
        m_pDevice = NULL;
        emit deviceChanged();
    }

    // Start service resolution
    m_pResolver->resolveRecord(record);
}

// Handler for resolver replies
void DevicePropertiesViewer::handleRecordResolved(const QString& fullName, const QHostInfo& hostInfo, int port, const QVariantMap& txtRecord)
{
    // Create and set device
    m_pDevice = new AirPlayDevice(fullName, hostInfo, port, txtRecord, this);
    emit deviceChanged();
}

// Handler for resolver errors
void DevicePropertiesViewer::handleError(DNSServiceErrorType err)
{
    // Log error
    cout << "Received error: " << err << endl;
}

// Property accessor methods
AirPlayDevice* DevicePropertiesViewer::device() const
{
    return m_pDevice;
}
