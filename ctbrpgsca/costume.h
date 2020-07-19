/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef COSTUME_H
#define COSTUME_H

#include "ability.h"
#include "role.h"

#include <QMap>

namespace tbrpgsca
{
    class Costume : public Role
    {
        #define FLAG_ENRAGED 4
        #define FLAG_CONFUSE 8
        #define FLAG_CONVERT 16
        #define FLAG_REFLECT 32
        #define FLAG_SHAPE_SHIFT 64
        #define FLAG_INVINCIBLE 128
        #define FLAG_STUN 256
        #define FLAG_KO 512
    public:
        static QString CausesTxt;

        int offense() const;
        int defense() const;
        int spirit() const;
        int wisdom() const;
        int agility() const;

        bool isEnraged() const;
        bool isConfused() const;
        bool isConverted() const;
        bool isCountering() const;
        bool isShapeShifted() const;
        bool isKnockedOut() const;
        bool isInvincible() const;
        bool isReflecting() const;
        bool isStunned() const;

        int maximumActions() const;
        int elementResistance(int const element) const;
        int stateResistance(State* const state) const;

        Ability& addedSkill(int const n) const;
        bool hasAddedSkill(Ability& skill) const;
        int addedSkillsSize() const;

        Ability& counterSkill(int const n) const;
        bool hasCounterSkill(Ability& skill) const;
        int counterSkillsSize() const;

        void adopt(QString& ret, Actor& actor);
        void abandon(QString& ret, Actor& actor);
        void apply(QString& ret, Actor& actor);

        Costume(int const id, QString name, QString sprite, bool const shapeShift, int const mActions, int const element, int const hpDmg, int const mpDmg, int const spDmg,
                int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range, bool const automate,
                bool const confuse, bool const reflect, bool const invincible, bool const revive, QVector<Ability*>* const skills, QVector<Ability*>* const counters,
                QMap<State*, int>* const states, QMap<State*, int>* const stRes, QMap<int, int>* const res);

        Costume(Costume& costume);

        ~Costume();
    protected:
        int _atk, _def, _spi, _wis, _agi, _m_actions;
        QVector<Ability*>* _a_skills,* _counters;
        QMap<State*, int>* _st_res;
        QMap<int, int>* _res;

        void apply(QString& ret, Scene* const scene, Actor& actor);
        void refresh(QString* const ret, Scene* const scene, Actor& actor, bool const updStates, bool const remove);
        void adopt(QString* const ret, Scene* const scene, Actor& actor, bool const upeStates, bool const rmeove);

        Costume(int const id, QString& name, QString& sprite, bool const shapeShift, int const mActions, int const element, int const hpDmg, int const mpDmg, int const spDmg,
                int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun, bool const range,
                bool const automate, bool const confuse, bool const reflect, bool const ko, bool const invincible, bool const revive, QVector<Ability*>* const skills,
                QVector<Ability*>* const counters, QMap<State*, int>* const states, QMap<State*, int>* const stRes, QMap<int, int>* const res);

        friend class Actor;
        friend class Ability;
        friend class DemoLib;
        friend class State;
        friend class Scene;
    };
}

#endif // COSTUME_H
