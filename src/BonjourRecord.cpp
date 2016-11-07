/*
 * BonjourRecord.cpp
 *
 *  Created on: Apr 8, 2013
 *      Author: jgordy
 */

#include "BonjourRecord.hpp"

// Constructor
BonjourRecord::BonjourRecord()
{
}

// Constructor
BonjourRecord::BonjourRecord(const QString& name, const QString& serviceType, const QString& domain) :
    m_name(name),
    m_serviceType(serviceType),
    m_domain(domain)
{
}

// Constructor
BonjourRecord::BonjourRecord(const char* name, const char* serviceType, const char* domain) :
    m_name(QString::fromUtf8(name)),
    m_serviceType(QString::fromUtf8(serviceType)),
    m_domain(QString::fromUtf8(domain))
{
}

// Copy constructor
BonjourRecord::BonjourRecord(const BonjourRecord& other) :
    m_name(other.m_name),
    m_serviceType(other.m_serviceType),
    m_domain(other.m_domain)
{
}

// Destructor
BonjourRecord::~BonjourRecord()
{
}

// Assignment operator
BonjourRecord& BonjourRecord::operator=(const BonjourRecord &other)
{
    // Check for self-assignment
    if( &other != this )
    {
        m_name = other.m_name;
        m_serviceType = other.m_serviceType;
        m_domain = other.m_domain;
    }

    return *this;
}

// Comparison operator
bool BonjourRecord::operator==(const BonjourRecord &other) const
{
    return (m_name == other.m_name) && (m_serviceType == other.m_serviceType) && (m_domain == other.m_domain);
}

// Property methods
QString BonjourRecord::name() const
{
    return m_name;
}

// Property methods
QString BonjourRecord::serviceType() const
{
    return m_serviceType;
}

// Property methods
QString BonjourRecord::domain() const
{
    return m_domain;
}
