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
#include "state.h"
#include "ability.h"
#include "actor.h"

#include <QStringBuilder>

using namespace tbrpgsca;

inline int State::getDuration() const
{
    return this->dur;
}

inline int State::getResistance() const
{
    return this->sRes;
}

inline State& State::inflict(QString& ret, Actor& actor, const bool always, const bool indefinite)
{
    return this->inflict(ret, nullptr, actor, always, indefinite);
}

State& State::inflict(QString& ret, Scene* scene, Actor& actor, const bool always, const bool indefinite)
{
    State& state = *this;
    QMap<State*, int>* stRes = actor.stRes;
    int const stateRes = state.sRes;
    if (always || stateRes < 0 || ((std::rand() % 10) > ((stRes == nullptr ? 0 : stRes->value(this, 0) + stRes->value(nullptr, 0)) + stateRes)))
    {
        state.adopt(scene, actor);
        QMap<State*, int>* trgStates = actor.stateDur;
        if (trgStates == nullptr)
        {
            trgStates = new QMap<State*, int>();
            actor.stateDur = trgStates;
        }
        if (indefinite)
        {
            trgStates->operator[](this) = -2;
        }
        else
        {
            int const stateDur = state.dur;
            int const crDur = trgStates->value(this, 0);
            if (crDur < stateDur || (crDur > -1 && stateDur < 0))
            {
                trgStates->operator[](this) = stateDur;
            }
        }
        state.blockSkills(actor, false);
    }
    return state;
}

State& State::remove(Scene* const scene, Actor& actor)
{
    State& state = *this;
    state.blockSkills(actor, true);
    state.abandon(scene, actor);
    return state;
}

inline bool State::disable(Actor& actor, const bool remove, const bool always)
{
    return this->disable(nullptr, actor, remove, always);
}

bool State::disable(Scene* const scene, Actor& actor, const bool remove, const bool always)
{
    QMap<State*, int>* sDur = actor.stateDur;
    if (sDur != nullptr && (always || (sDur->value(this, -2) != -2)))
    {
        if (sDur->value(this, -3) > -3)
        {
            this->remove(scene, actor);
        }
        if (remove)
        {
            sDur->remove(this);
        }
        else
        {
            sDur->operator[](this) = -3;
        }
        return true;
    }
    else
    {
        return false;
    }
}

inline State& State::alter(QString& ret, Actor& actor, const bool consume)
{
    return this->alter(ret, nullptr, actor, consume);
}

State& State::alter(QString& ret, Scene* const scene, Actor& actor, const bool consume)
{
    State& state = *this;
    QMap<State*, int>* sDur = actor.stateDur;
    if (sDur != nullptr /*&& actor.hp > 0*/)
    {
        int const d = sDur->value(this, -3);
        if (consume)
        {
            if (d > 0)
            {
                sDur->operator[](this) = d - 1;
            }
        }
        else if (d == 0)
        {
            state.remove(scene, actor);
            sDur->operator[](this) = -3;
        }
    }
    return state;
}

State& State::blockSkills(Actor& actor, const bool remove)
{
    State& state = *this;
    QVector<Ability*>* rSkills = state.rSkills;
    if (rSkills != nullptr)
    {
        QMap<Ability*, int>* iSkills = actor.skillsCrQty;
        if (remove)
        {
            if (iSkills != nullptr)
            {
                for (Ability* const skill : *rSkills)
                {
                    if (skill->mQty > 0)
                    {
                        iSkills->operator[](skill) = -1 * iSkills->value(skill, 0);
                    }
                    else
                    {
                        iSkills->remove(skill);
                    }
                }
            }
        }
        else
        {
            if (iSkills == nullptr)
            {
                iSkills = new QMap<Ability*, int>();
                actor.skillsCrQty = iSkills;
            }
            for (Ability* const skill : *rSkills)
            {
                iSkills->operator[](skill) = skill->mQty > 0 ? -1 * iSkills->value(skill, 0) : 0;
            }
        }
    }
    return state;
}

State::State(int const id, QString& name, QString* sprite, bool const shapeShift, int const dur, int const sRes, int const elm, int const hpDmg, int const mpDmg,
             int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun,
             bool const range, bool const automate, bool const confuse, bool const reflect, bool const revive, QMap<int, int>* const res, QMap<State*, int>* const stRes,
             QVector<Ability*>* const aSkills, QVector<Ability*>* const counters, QVector<Ability*>* const rSkills)
    : Costume(id, name, sprite, shapeShift, elm, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi, stun, range, automate, confuse, reflect, revive, res, aSkills,
              counters, stRes)
{
    this->dur = dur;
    this->sRes = sRes;
    this->rSkills = rSkills;
}

State::State(State& state) : Costume(state)
{
    this->dur = state.dur;
    this->sRes = state.sRes;
    this->rSkills = state.rSkills;
}

State::~State()
{

}
