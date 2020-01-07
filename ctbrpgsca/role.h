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

namespace tbrpgsca
{

    class Actor;
    class State;
    class Scene;

    class Role
    {
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
    public:
        static QString HpTxt;
        static QString MpTxt;
        static QString RpTxt;

        int getId() const;
        QString getName() const;
        QString getSprite() const;
        int getDamageElement() const;
        int getMaximumHp() const;
        int getMaximumMp() const;
        int getMaximumRp() const;
        int getCurrentHp() const;
        int getCurrentMp() const;
        int getCurrentRp() const;
        bool isReviving() const;
        bool isRanged() const;

        QList<State*> getStatesList() const;
        int getStateDuration(State& state) const;
        bool hasState(State& state) const;
        int getStatesSize() const;

        Role& damage(QString& ret, Actor* const user, Actor& target, int const dmg, bool const percent);

        bool operator==(Role& role) const;
    protected:
        QString name,* sprite;
        int id, hp, mp, sp, mHp, mMp, mSp, dmgType, flags;
        QMap<State*, int>* stateDur;

        Role& damage(QString& ret, Scene* scene, Actor* const absorber, Actor& target, int dmg, bool const percent);

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
