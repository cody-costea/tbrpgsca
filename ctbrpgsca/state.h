/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef STATE_H
#define STATE_H

#include "costume.h"


namespace tbrpgsca
{
    class Scene;

    class State : public Costume
    {
        #define STATE_END_DUR -3
    public:
        int getDuration() const;
        int getResistance() const;

        int getRemovedSkillsSize() const;
        bool hasRemovedSkill(Ability& skill) const;
        Ability& getRemovedSkill(int const n) const;

        State& alter(QString& ret, Actor& actor, bool const consume);
        State& inflict(QString& ret, Actor* user, Actor& target, int dur, bool const always);
        bool disable(Actor& actor, int const dur, bool const remove);
        State& blockSkills(Actor& actor, bool const remove);

        State(int const id, QString name, QString sprite, bool const shapeShift, int const dur, int const sRes, int const mActions,
              int const elm, int const hpDmg, int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk,
              int const def, int const spi, int const wis, int const agi, bool const stun, bool const range, bool const automate, bool const confuse,
              bool const convert, bool const reflect, bool const ko, bool const invincible, bool const revive, QVector<Ability*>* const aSkills,
              QVector<Ability*>* const counters, QVector<Ability*>* const rSkills, QMap<State*, int>* const states, QMap<State*, int>* const stRes,
              QMap<int, int>* const res);

        State(State& state);

        ~State();
    protected:
        int dur, sRes;
        QVector<Ability*>* rSkills;

        State& alter(QString* const ret, Scene* const scene, Actor& actor, bool const consume);
        State& inflict(QString* const ret, Scene* const scene, Actor* user, Actor& target, int dur, bool const always);
        bool disable(QString* const ret, Scene* const scene, Actor& actor, int dur, bool const remove);
        State& remove(QString* const ret, Scene* const scene, Actor& actor);

        friend class Actor;
        friend class Ability;
        friend class Scene;
    };

}

#endif // STATE_H
