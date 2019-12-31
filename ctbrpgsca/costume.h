/*
Copyright (C) AD 2013-2019 Claudiu-Stefan Costea

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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
        #define FLAG_AUTOMATE 4
        #define FLAG_CONFUSE 8
        #define FLAG_CONVERT 16
        #define FLAG_REFLECT 32
        #define FLAG_SHAPE_SHIFT 64
        #define FLAG_STUN 128
    public:
        static QString CausesTxt;

        inline int getAttack() const;
        inline int getDefense() const;
        inline int getSpirit() const;
        inline int getWisdom() const;
        inline int getAgility() const;

        inline bool isAutomated() const;
        inline bool isConfused() const;
        inline bool isConverted() const;
        inline bool isCountering() const;
        inline bool isShapeShifted() const;
        inline bool isReflecting() const;        
        inline bool isStunned() const;

        inline int getMaximumActions() const;
        inline int getElementResistance(int const element) const;
        inline int getStateResistance(State* const state) const;

        inline Ability& getAddedSkill(int const n) const;
        inline bool hasAddedSkill(Ability& skill) const;
        inline int getAddedSkillsSize() const;

        inline Ability& getCounterSkill(int const n) const;
        inline bool hasCounterSkill(Ability& skill) const;
        inline int getCounterSkillsSize() const;

        inline Costume& adopt(QString& ret, Actor& actor);
        inline Costume& abandon(QString& ret, Actor& actor);
        inline Costume& apply(QString& ret, Actor& actor);

        Costume(int const id, QString& name, QString* sprite, bool const shapeShift, int const elm, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
                int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range, bool const automate,
                bool const confuse, bool const reflect, bool const revive, QMap<int, int>* const res, QMap<State*, int>* const states, QVector<Ability*>* const skills,
                QVector<Ability*>* const counters, QMap<State*, int>* const stRes);

        Costume(Costume& costume);

        ~Costume();
    protected:
        int atk, def, spi, wis, agi, mActions;
        QVector<Ability*>* aSkills,* counters;
        QMap<State*, int>* stRes;
        QMap<int, int>* res;

        Costume& adopt(QString* ret, Scene* const scene, Actor& actor, bool const addStates);
        Costume& abandon(QString* ret, Scene* const scene, Actor& actor, bool const delStates);
        Costume& apply(QString& ret, Scene* const scene, Actor& actor);

        Costume(int const id, QString& name, QString* sprite, bool const shapeShift, int const elm, int const hpDmg, int const mpDmg, int const spDmg, int const mHp,
                int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun, bool const range, bool const automate,
                bool const confuse, bool const reflect, bool const revive, QMap<int, int>* const res, QMap<State*, int>* const states, QVector<Ability*>* const skills,
                QVector<Ability*>* const counters, QMap<State*, int>* const stRes);

        friend class Actor;
        friend class Ability;
        friend class State;
        friend class Scene;
    };
}

#endif // COSTUME_H
