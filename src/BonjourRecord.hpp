/*
 * BonjourRecord.hpp
 *
 *  Created on: Apr 8, 2013
 *      Author: jgordy
 */

#ifndef BONJOURRECORD_HPP_
#define BONJOURRECORD_HPP_

#include <QtCore/QMetaType>

//
// BonjourRecord
//
class BonjourRecord
{
public:

    // Constructor / destructor
    BonjourRecord();
    BonjourRecord(const QString& name, const QString& serviceType, const QString& domain);
    BonjourRecord(const char* name, const char* serviceType, const char* domain);
    BonjourRecord(const BonjourRecord& other);
    virtual ~BonjourRecord();

    // Assignment operator
    BonjourRecord& operator=(const BonjourRecord& other);

    // Comparison operator
    bool operator==(const BonjourRecord &other) const;

    // Property accessor methods
    QString name() const;
    QString serviceType() const;
    QString domain() const;

protected:

    // Property values
    QString m_name;
    QString m_serviceType;
    QString m_domain;
};

Q_DECLARE_METATYPE(BonjourRecord);

#endif /* BONJOURRECORD_HPP_ */
