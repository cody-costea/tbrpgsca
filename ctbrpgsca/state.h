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
        inline int getDuration() const;
        inline int getResistance() const;

        inline int getRemovedSkillsSize() const;
        inline bool hasRemovedSkill(Ability& skill) const;
        inline Ability& getRemovedSkill(int const n) const;

        inline State& alter(QString& ret, Actor& actor, bool const consume);
        inline State& inflict(QString& ret, Actor& user, Actor& target, int dur, bool const always);
        inline bool disable(QString& ret, Actor& actor, int const dur, bool const remove);
        State& blockSkills(Actor& actor, bool const remove);

        State(int const id, QString& name, QString* sprite, bool const shapeShift, int const dur, int const sRes, int const elm, int const hpDmg, int const mpDmg,
              int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun,
              bool const range, bool const automate, bool const confuse, bool const reflect, bool const revive, QMap<int, int>* const res, QMap<State*, int>* const states,
              QMap<State*, int>* const stRes, QVector<Ability*>* const aSkills, QVector<Ability*>* const counters, QVector<Ability*>* const rSkills);

        State(State& state);

        ~State();
    protected:
        QVector<Ability*>* rSkills;
        int dur, sRes;

        State& alter(QString& ret, Scene* const scene, Actor& actor, bool const consume);
        State& inflict(QString& ret, Scene* const scene, Actor& user, Actor& target, int dur, bool const always);
        bool disable(QString& ret, Scene* const scene, Actor& actor, int dur, bool const remove);
        State& remove(QString& ret, Scene* const scene, Actor& actor);

        friend class Actor;
        friend class Ability;
        friend class Scene;
    };

}

#endif // STATE_H
