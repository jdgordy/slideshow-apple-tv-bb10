/*
 * AirPlayDevice.cpp
 *
 *  Created on: Apr 27, 2013
 *      Author: jgordy
 */

#include "AirPlayDevice.hpp"

// Constructor
AirPlayDevice::AirPlayDevice(QObject* pParent) :
    QObject(pParent),
    m_name("Not available"),
    m_port(-1),
    m_deviceID("Not available"),
    m_model("Not available"),
    m_passwordRequired(false)
{
}

// Constructor
AirPlayDevice::AirPlayDevice(const QString& name, const QHostInfo& hostInfo, int port, const QVariantMap& txtRecord, QObject* pParent) :
    QObject(pParent),
    m_name(name),
    m_hostInfo(hostInfo),
    m_port(port),
    m_deviceID(txtRecord.value("deviceid", "Not available").toString()),
    m_model(txtRecord.value("model", "Not available").toString()),
    m_passwordRequired(txtRecord.contains("pw"))
{
}

// Copy constructor
AirPlayDevice::AirPlayDevice(const AirPlayDevice& other) :
    QObject(other.parent()),
    m_name(other.m_name),
    m_hostInfo(other.m_hostInfo),
    m_port(other.m_port),
    m_deviceID(other.m_deviceID),
    m_model(other.m_model),
    m_passwordRequired(other.m_passwordRequired)
{
}

// Destructor
AirPlayDevice::~AirPlayDevice()
{
}

// Get underlying host info
QHostInfo AirPlayDevice::hostInfo() const
{
    return m_hostInfo;
}

// Property accessor methods
QString AirPlayDevice::name() const
{
    return m_name;
}

// Property methods
QString AirPlayDevice::hostName() const
{
    return m_hostInfo.hostName();
}

// Property methods
QString AirPlayDevice::address() const
{
    return m_hostInfo.addresses().first().toString();
}

// Property methods
int AirPlayDevice::port() const
{
    return m_port;
}

// Property methods
QString AirPlayDevice::deviceID() const
{
    return m_deviceID;
}

// Property methods
QString AirPlayDevice::model() const
{
    return m_model;
}

// Property methods
bool AirPlayDevice::passwordRequired() const
{
    return m_passwordRequired;
}
