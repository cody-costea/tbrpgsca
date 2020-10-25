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

#define FLAG_REVIVE 1
#define FLAG_RANGE 2
#define DMG_TYPE_ATK 1
#define DMG_TYPE_DEF 2
#define DMG_TYPE_SPI 4
#define DMG_TYPE_WIS 8
#define DMG_TYPE_AGI 16
#define ELEMENT_FIRE 32
#define ELEMENT_WATER 64
#define ELEMENT_THUNDER 128
#define ELEMENT_EARTH 256
#define ELEMENT_PSYCHIC 512
#define ELEMENT_LIGHT 1024

        Q_OBJECT
        PROP_FLAG_GET(isRanged, FLAG_RANGE, public)
        PROP_FLAG_GET(isReviving, FLAG_REVIVE, public)
        PROP_FIELD_GET_CUSTOM(currentHp, int, public, _hp)
        PROP_FIELD_GET_CUSTOM(currentMp, int, public, _mp)
        PROP_FIELD_GET_CUSTOM(currentRp, int, public, _sp)
        PROP_FIELD_GET_CUSTOM(maximumHp, int, public, _m_hp)
        PROP_FIELD_GET_CUSTOM(maximumMp, int, public, _m_mp)
        PROP_FIELD_GET_CUSTOM(maximumRp, int, public, _m_sp)
        PROP_FIELD_GET_CUSTOM(dmgType, int, public, _dmg_type)
        PROP_FIELD_GET_CUSTOM(name, QString, public, _name)
        PROP_FIELD_GET_CUSTOM(id, int, public, _id)
    public:
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
        QString _name,* _sprite;
        int _id, _hp, _mp, _sp, _m_hp, _m_mp, _m_sp, _dmg_type;
        QMap<State*, int>* _state_dur;

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
#endif // ROLE_H
