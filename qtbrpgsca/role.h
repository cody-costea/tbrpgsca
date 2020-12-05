/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef ROLE_H
#define ROLE_H

#include <play.h>

#include <QString>
#include <QVector>

namespace tbrpgsca
{

    class Actor;
    class State;
    class Scene;

    class Role : public Play
    {
#define DMG_RND 3

        Q_OBJECT
        PROP_FLAG(Role, Ranged, ranged, Attribute::Range, inline, public, public)
        PROP_FLAG(Role, Reviving, revives, Attribute::Revive, inline, public, public)
        PROP_FIELD(Role, CurrentHp, currentHp, int, virtual inline, public, public, _role_data->_hp)
        PROP_FIELD(Role, CurrentMp, currentMp, int, virtual inline, public, public, _role_data->_mp)
        PROP_FIELD(Role, CurrentRp, currentRp, int, virtual inline, public, public, _role_data->_sp)
        PROP_FIELD(Role, MaximumHp, maximumHp, int, virtual inline, public, public, _role_data->_m_hp)
        PROP_FIELD(Role, MaximumMp, maximumMp, int, virtual inline, public, public, _role_data->_m_mp)
        PROP_FIELD(Role, MaximumRp, maximumRp, int, virtual inline, public, public, _role_data->_m_sp)
        PROP_FIELD(Role, DamageType, damageType, int, inline, public, public, _role_data->_dmg_type)
        PROP_FIELD(Role, DatabaseId, databaseId, int, inline, public, protected, _role_data->_id)
        PROP_FIELD(Role, Name, name, QString, inline, public, public, _role_data->_name)
    public:
        enum Attribute {
            Revive = 1,
            Range = 2
        };
        Q_DECLARE_FLAGS(Attributes, Attribute)
        Q_FLAG(Attributes)

        enum Element {
            Attack = 1,
            Defense = 2,
            Spirit = 4,
            Wisdom = 8,
            Agility = 16,
            Fire = 32,
            Water = 64,
            Wind = 128,
            Earth = 256,
            Psychic = 512,
            Light = 1024,
            Electric = 2048,
            Ice = 4096,
        };
        Q_DECLARE_FLAGS(Elements, Element)
        Q_FLAG(Elements)

        static QString HpTxt;
        static QString MpTxt;
        static QString RpTxt;
        static QString SuffersTxt;
        static QString ResistTxt;

        QString sprite() const;
        QList<State*> statesList() const;
        int stateDuration(State& state) const;
        bool hasState(State& state) const;
        int statesSize() const;

        Q_INVOKABLE void damage(QString& ret, Actor* const user, Actor& target, int const dmg, bool const percent) const;

        bool operator==(const Role& role) const;
        bool operator!=(const Role& role) const;
        bool operator<=(const Role& role) const;
        bool operator>=(const Role& role) const;
        bool operator<(const Role& role) const;
        bool operator>(const Role& role) const;
    protected:
        class RoleData : public QSharedData
        {
        public:
            ~RoleData();

        protected:
            QString _name,* _sprite;
            int _id, _hp, _mp, _sp, _m_hp, _m_mp, _m_sp, _dmg_type;
            QMap<State*, int>* _state_dur;

            friend class Ability;
            friend class Costume;
            friend class Actor;
            friend class State;
            friend class Scene;
            friend class Role;
        };

        QSharedDataPointer<RoleData> _role_data;

        Role(int const id, QString& name, QString& sprite, int const hpDmg, int const mpDmg, int const spDmg, int const mHp, int const mMp,
             int const mSp, int const element, bool const range, bool const revive, QMap<State*, int>* const states, QObject* const parent = NIL);

        Role(int const id, QString&& name, QString&& sprite, int const hpDmg, int const mpDmg, int const spDmg, int const mHp, int const mMp,
             int const mSp, int const element, bool const range, bool const revive, QMap<State*, int>* const states, QObject* const parent = NIL);

        explicit Role(QObject* const parent = NIL);

        Role(const Role& role);

        ~Role();

        friend class Actor;
        friend class Ability;
        friend class Costume;
        friend class State;
        friend class Scene;
    };

}

//Q_DECLARE_METATYPE(tbrpgsca::Role)
Q_DECLARE_METATYPE(tbrpgsca::Role*)
Q_DECLARE_INTERFACE(tbrpgsca::Role, "com.codycostea.tbrpgsca.Role")

Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Role::Attributes)
Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Role::Elements)
#endif // ROLE_H
