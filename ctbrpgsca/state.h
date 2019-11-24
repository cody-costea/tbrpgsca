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

    class State : public Costume
    {
        #define FLAG_STUN 64;
    public:
        inline int getDuration() const;
        inline int getStateResistance() const;

        inline bool isStunning() const;

        int getRemovedSkillsSize();
        Ability* getRemovedSkill(int n);

        QString inflict(const Actor& actor, bool const always, bool const indefinite);
        void disableSkills(const Actor& actor, bool const remove);
        QString apply(const Actor& actor, bool const consume);

        State(int const id, const QString& name, const QString& sprite, int dur, int sRes, int const hpDmg, int const mpDmg, int const spDmg,
              int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const range,
              bool const automate, bool const confuse, bool const reflect, bool const counter, bool const revive, QMap<int, int>* const res,
              QMap<State*, int>* const stRes, QVector<Ability*>* const aSkills, QVector<Ability*>* const rSkills, QVector<State*>* const rStates);
    private:
        QVector<Ability*>* rSkills;
        int dur, sRes;

        friend class Actor;
        friend class Ability;
    };

}

#endif // STATE_H
