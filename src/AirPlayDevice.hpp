/*
 * AirPlayDevice.hpp
 *
 *  Created on: Apr 27, 2013
 *      Author: jgordy
 */

#ifndef AIRPLAYDEVICE_HPP_
#define AIRPLAYDEVICE_HPP_

#include <QtCore/QObject>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QHostAddress>

//
// AirPlayDevice
//
class AirPlayDevice : public QObject
{
    Q_OBJECT

    // The data properties of the record that is displayed
    Q_PROPERTY(QString name READ name CONSTANT);
    Q_PROPERTY(QString hostName READ hostName CONSTANT);
    Q_PROPERTY(QString address READ address CONSTANT);
    Q_PROPERTY(int port READ port CONSTANT);
    Q_PROPERTY(QString deviceID READ deviceID CONSTANT);
    Q_PROPERTY(QString model READ model CONSTANT);
    Q_PROPERTY(bool passwordRequired READ passwordRequired CONSTANT);

public:

    // Constructor / destructor
    AirPlayDevice(QObject* pParent = NULL);
    AirPlayDevice(const QString& name, const QHostInfo& hostInfo, int port, const QVariantMap& txtRecord, QObject* pParent = NULL);
    AirPlayDevice(const AirPlayDevice& other);
    virtual ~AirPlayDevice();

    // Get underlying host info
    QHostInfo hostInfo() const;

    // Property accessor methods
    QString name() const;
    QString hostName() const;
    QString address() const;
    int port() const;
    QString deviceID() const;
    QString model() const;
    bool passwordRequired() const;

protected:

    // Property values
    QString         m_name;
    QHostInfo       m_hostInfo;
    int             m_port;
    QString         m_deviceID;
    QString         m_model;
    bool            m_passwordRequired;
};

#endif /* AIRPLAYDEVICE_HPP_ */
