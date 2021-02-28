/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef ROLE_H
#define ROLE_H


#include <QString>
#include <QVector>

#include "play.h"

namespace tbrpgsca
{

    class Actor;
    class Ailment;
    class Scene;

    class Role : public Play
    {
#define DMG_RND 3

        Q_OBJECT
        QML_ELEMENT

        PROP_FLAG(Role, Ranged, ranged, Attribute::RANGE, inline, public, public)
        PROP_FLAG(Role, Reviving, revives, Attribute::REVIVE, inline, public, public)
        PROP_READONLY_FIELD(Role, DatabaseId, databaseId, int, inline, public, roleData()._id)
        PROP_FIELD(Role, CurrentHp, currentHp, int, virtual inline, public, public, roleData()._hp)
        PROP_FIELD(Role, CurrentMp, currentMp, int, virtual inline, public, public, roleData()._mp)
        PROP_FIELD(Role, CurrentRp, currentRp, int, virtual inline, public, public, roleData()._sp)
        PROP_FIELD(Role, MaximumHp, maximumHp, int, virtual inline, public, public, roleData()._m_hp)
        PROP_FIELD(Role, MaximumMp, maximumMp, int, virtual inline, public, public, roleData()._m_mp)
        PROP_FIELD(Role, MaximumRp, maximumRp, int, virtual inline, public, public, roleData()._m_sp)
        //PROP_READONLY_FIELD(Role, DatabaseId, databaseId, Role::Roleint, inline, public, roleData()._id)
        PROP_FIELD(Role, DamageType, damageType, int, inline, public, public, roleData()._dmg_type)
        //PROP_REF(Role, Sprite, sprite, QString, inline, public, public, roleData()._sprite)
        PROP_FIELD(Role, Name, name, QString, inline, public, public, roleData()._name)
    public:
        enum Attribute {
            REVIVE = 1,
            RANGE = 2
        };
        Q_DECLARE_FLAGS(Attributes, Attribute)
        Q_FLAG(Attributes)

        enum Element {
            ATTACK = 1,
            DEFENSE = 2,
            SPIRIT = 4,
            WISDOM = 8,
            AGILITY = 16,
            FIRE = 32,
            WATER = 64,
            WIND = 128,
            EARTH = 256,
            PSYCHIC = 512,
            LIGHT = 1024,
            ELECTRIC = 2048,
            ICE = 4096,
        };
        Q_DECLARE_FLAGS(Elements, Element)
        Q_FLAG(Elements)

        static QString HpTxt;
        static QString MpTxt;
        static QString RpTxt;
        static QString SuffersTxt;
        static QString ResistTxt;

        QString sprite() const;
        /*QList<Ailment*> statesList() const;
        int stateDuration(Ailment& state) const;
        bool hasState(Ailment& state) const;
        int statesSize() const;*/

        inline bool operator==(const Role& role) const
        {
            return this->databaseId() == role.databaseId();
        }
        inline bool operator!=(const Role& role) const
        {
            return this->databaseId() != role.databaseId();
        }
        inline bool operator<=(const Role& role) const
        {
            return this->databaseId() <= role.databaseId();
        }
        inline bool operator>=(const Role& role) const
        {
            return this->databaseId() >= role.databaseId();
        }
        inline bool operator<(const Role& role) const
        {
            return this->databaseId() < role.databaseId();
        }
        inline bool operator>(const Role& role) const
        {
            return this->databaseId() > role.databaseId();
        }

        Q_INVOKABLE void damage(QString& ret, Actor* const user, Actor& target, int const dmg, bool const percent) const;
    protected:
        class RoleSheet : public PlaySheet
        {
        public:
            virtual ~RoleSheet();

        protected:
            QString _name,* _sprite;
            int _id, _hp, _mp, _sp, _m_hp, _m_mp, _m_sp, _dmg_type;
            QList<Ailment>* _a_states;

            RoleSheet(int const id, QString& name, QString& sprite, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
                      int const mMp, int const mSp, int const element, bool const range, bool const revive, QList<Ailment>* const aStates);

            friend class Ability;
            friend class Costume;
            friend class Ailment;
            friend class Actor;
            friend class Scene;
            friend class State;
            friend class Role;
        };

        inline RoleSheet& roleMutData()
        {
            return (*static_cast<RoleSheet*>(this->_play_data.data()));
        }

        inline const RoleSheet& roleData() const
        {
            return (*static_cast<const RoleSheet*>(this->_play_data.data()));
        }

        Role(int const id, QString& name, QString& sprite, int const hpDmg, int const mpDmg, int const spDmg, int const mHp, int const mMp,
             int const mSp, int const element, bool const range, bool const revive, QList<Ailment>* const states, QObject* const parent = NIL);

        Role(int const id, QString&& name, QString&& sprite, int const hpDmg, int const mpDmg, int const spDmg, int const mHp, int const mMp,
             int const mSp, int const element, bool const range, bool const revive, QList<Ailment>* const states, QObject* const parent = NIL);

        explicit Role(QObject* const parent, RoleSheet* const roleDataPtr);

        explicit Role(QObject* const parent = NIL);

        Role(const Role&& role);

        Role(const Role& role);

        virtual ~Role();

        friend class Actor;
        friend class Ability;
        friend class Costume;
        friend class Ailment;
        friend class Scene;
        friend class State;
    };

}

//Q_DECLARE_METATYPE(tbrpgsca::Role)
Q_DECLARE_METATYPE(tbrpgsca::Role*)
Q_DECLARE_INTERFACE(tbrpgsca::Role, "com.codycostea.tbrpgsca.Role")

Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Role::Attributes)
Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Role::Elements)
#endif // ROLE_H
