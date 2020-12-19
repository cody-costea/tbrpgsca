/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef COSTUME_H
#define COSTUME_H

#include "ability.hpp"
#include "role.hpp"

#include <QMap>

namespace tbrpgsca
{
    class Costume : public Role
    {
        #define FLAG_ENRAGED 4
        #define FLAG_CONFUSE 8
        #define FLAG_CONVERT 16
        #define FLAG_COVERING 1024
        #define FLAG_SHAPE_SHIFT 64
        #define FLAG_INVINCIBLE 128
        #define FLAG_COUNTER 2048
        #define FLAG_REFLECT 32
        #define FLAG_STUN 256
        #define FLAG_KO 512

        PROP_FLAG_GET(isStunned, FLAG_STUN, public)
        PROP_FLAG_GET(isKnockedOut, FLAG_KO, public)
        PROP_FLAG_GET(isEnraged, FLAG_ENRAGED, public)
        PROP_FLAG_GET(isConfused, FLAG_CONFUSE, public)
        PROP_FLAG_GET(isCovering, FLAG_COVERING, public)
        PROP_FLAG_GET(isConverted, FLAG_CONVERT, public)
        PROP_FLAG_GET(isCountering, FLAG_COUNTER, public)
        PROP_FLAG_GET(isReflecting, FLAG_REFLECT, public)
        PROP_FLAG_GET(isInvincible, FLAG_INVINCIBLE, public)
        PROP_FLAG_GET(isShapeShifted, FLAG_SHAPE_SHIFT, public)
        //PROP_FIELD_GET_CUSTOM(maxActions, int, public, _m_actions)
        PROP_FIELD_GET_CUSTOM(offense, int, public, _atk)
        PROP_FIELD_GET_CUSTOM(defense, int, public, _def)
        PROP_FIELD_GET_CUSTOM(spirit, int, public, _spi)
        PROP_FIELD_GET_CUSTOM(wisdom, int, public, _wis)
        PROP_FIELD_GET_CUSTOM(agility, int, public, _agi)
    public:
        static QString CausesTxt;

        int elementResistance(int const element) const;
        int stateResistance(State* const state) const;

        const Ability& addedSkill(int const n) const;
        bool hasAddedSkill(const Ability& skill) const;
        int addedSkillsSize() const;

        void adopt(QString& ret, Actor& actor) const;
        void abandon(QString& ret, Actor& actor) const;
        void apply(QString& ret, Actor& actor) const;

        Costume(int const id, QString name, QString sprite, bool const shapeShift, int const element, int const blockedSkills, int const hpDmg, int const mpDmg, int const spDmg,
                int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range, bool const automate,
                bool const confuse, bool const reflect, bool const invincible, bool const revive, QVector<const Ability*>* const skills, bool const counters,
                QMap<const State*, int>* const states, QMap<const State*, int>* const stRes, QMap<int, int>* const res);

        Costume(const Costume& costume);

        ~Costume();
    protected:
        signed int _atk: 16, _def: 16, _spi: 16, _wis: 16, _agi: 16, _b_skill_types: 16;
        QVector<const Ability*>* _a_skills;
        QMap<const State*, int>* _st_res;
        QMap<int, int>* _res;

        void apply(QString& ret, Scene* const scene, Actor& actor) const;
        void refresh(QString* const ret, Scene* const scene, Actor& actor, bool const updStates, bool const remove) const;
        void adopt(QString* const ret, Scene* const scene, Actor& actor, bool const upeStates, bool const rmeove) const;

        Costume(int const id, QString& name, QString& sprite, bool const shapeShift, int const element, int blockedSkills, int const hpDmg, int const mpDmg, int const spDmg,
                int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun, bool const range,
                bool const automate, bool const confuse, bool const reflect, bool const ko, bool const invincible, bool const revive, QVector<const Ability*>* const skills,
                bool const counters, QMap<const State*, int>* const states, QMap<const State*, int>* const stRes, QMap<int, int>* const res);

        friend class Actor;
        friend class Ability;
        friend class DemoLib;
        friend class State;
        friend class Scene;
    };
}

#endif // COSTUME_H
