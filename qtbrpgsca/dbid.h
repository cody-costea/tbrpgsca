#ifndef DBID_H
#define DBID_H

#include "play.h"

namespace tbrpgsca
{

    struct DbId
    {
        Q_GADGET
        QML_ELEMENT
    public:
        inline int value() const
        {
            return this->_id;
        }
        inline operator int() const
        {
            return this->value();
        }
        inline bool operator==(const DbId& role) const
        {
            return this->value() == role.value();
        }
        inline bool operator!=(const DbId& role) const
        {
            return this->value() != role.value();
        }
        inline bool operator<=(const DbId& role) const
        {
            return this->value() <= role.value();
        }
        inline bool operator>=(const DbId& role) const
        {
            return this->value() >= role.value();
        }
        inline bool operator<(const DbId& role) const
        {
            return this->value() < role.value();
        }
        inline bool operator>(const DbId& role) const
        {
            return this->value() > role.value();
        }
        inline DbId(const DbId& role) : DbId(role._id) {}
        //inline RoleDbId(const RoleDbId&& role) : RoleDbId(static_cast<const RoleDbId&>(role)) {}
        explicit inline DbId(const int id)
        {
            this->_id = id;
        }
        inline DbId()
        {
            this->_id = 0;
        }
    private:
        int _id;

        friend class Role;
        friend class RoleData;
    };

}

Q_DECLARE_METATYPE(tbrpgsca::DbId*)
Q_DECLARE_INTERFACE(tbrpgsca::DbId, "com.codycostea.tbrpgsca.DbId")

#endif // DBID_H
