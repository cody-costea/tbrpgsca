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
        PROP_FLAG_GET(isRanged, Attribute::Range, public)
        PROP_FLAG_GET(isReviving, Attribute::Revive, public)
        PROP_FIELD_GET_CUSTOM(currentHp, int, public, _role_data->_hp)
        PROP_FIELD_GET_CUSTOM(currentMp, int, public, _role_data->_mp)
        PROP_FIELD_GET_CUSTOM(currentRp, int, public, _role_data->_sp)
        PROP_FIELD_GET_CUSTOM(maximumHp, int, public, _role_data->_m_hp)
        PROP_FIELD_GET_CUSTOM(maximumMp, int, public, _role_data->_m_mp)
        PROP_FIELD_GET_CUSTOM(maximumRp, int, public, _role_data->_m_sp)
        PROP_FIELD_GET_CUSTOM(dmgType, int, public, _role_data->_dmg_type)
        PROP_FIELD_GET_CUSTOM(name, QString, public, _role_data->_name)
        PROP_FIELD_GET_CUSTOM(id, int, public, _role_data->_id)
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

        Q_INVOKABLE void damage(QString& ret, Actor* const user, Actor& target, int const dmg, bool const percent);

        bool operator==(Role& role) const;
    protected:
        class RoleData : public QSharedData
        {
        private:
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

        void damage(QString& ret, Scene* const scene, Actor* const absorber, Actor& target, int dmg, bool const percent);

        Role(int const id, QString& name, QString& sprite, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
             int const mMp, int const mSp, int const element, bool const range, bool const revive, QMap<State*, int>* const states);

        Role(Role& role);

        ~Role();

        friend class Actor;
        friend class Ability;
        friend class Costume;
        friend class State;
        friend class Scene;
    };

}

Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Role::Attributes)
Q_DECLARE_OPERATORS_FOR_FLAGS(tbrpgsca::Role::Elements)
#endif // ROLE_H
