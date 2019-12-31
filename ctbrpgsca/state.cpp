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

inline State& State::inflict(QString& ret, Actor& user, Actor& target, int dur, const bool always)
{
    return this->inflict(ret, nullptr, user, target, dur, always);
}

State& State::inflict(QString& ret, Scene* scene, Actor& user, Actor& target, int stateDur, const bool always)
{
    State& state = *this;
    if (stateDur == 0)
    {
        stateDur = state.dur;
        if (stateDur == 0)
        {
            return state;
        }
    }
    if (stateDur > STATE_END_DUR)
    {
        int const stateRes = state.sRes;
        QMap<State*, int>* stRes = target.stRes;
        if (always || stateRes < 0 || ((std::rand() % 10) > ((stRes == nullptr ? 0 : stRes->value(this, 0) + stRes->value(nullptr, 0)) + stateRes)))
        {
            {
                QMap<State*, int>* trgStates = target.stateDur;
                if (trgStates == nullptr)
                {
                    trgStates = new QMap<State*, int>();
                    target.stateDur = trgStates;
                }
                else
                {
                    QMap<State*, int>* rStates = state.stateDur;
                    if (rStates != nullptr)
                    {
                        auto const rLast = rStates->cend();
                        for (auto rIt = rStates->cbegin(); rIt != rLast; ++rIt)
                        {
                            int rDur = rIt.value();
                            State* const rState = rIt.key();
                            if (rDur == 0 || rDur <= STATE_END_DUR)
                            {
                                rDur = rState->dur;
                            }
                            auto const last = trgStates->cend();
                            for (auto it = trgStates->cbegin(); it != last; ++it)
                            {
                                State* const aState = it.key();
                                if (aState == rState)
                                {
                                    int const aDur = it.value();
                                    if (aDur > STATE_END_DUR)
                                    {
                                        if (aDur < 0 && rDur > aDur)
                                        {
                                            return state;
                                        }
                                        rState->disable(ret, scene, target, rDur, false);
                                        if (rDur > 0 && aDur > 0)
                                        {
                                            stateDur -= aDur < rDur ? aDur : rDur;
                                            if (stateDur < 1)
                                            {
                                                return state;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                int const crDur = trgStates->value(this, -3);
                if (crDur == STATE_END_DUR)
                {
                    state.adopt(&ret, scene, target, false);
                    trgStates->operator[](this) = stateDur;
                    state.blockSkills(target, false);
                }
                else if ((crDur > -1 && crDur < stateDur) || (stateDur < 0 && stateDur < crDur))
                {
                    trgStates->operator[](this) = stateDur;
                }
            }
            if (this->isConverted())
            {
                if (target.oldSide == -1)
                {
                    target.oldSide = static_cast<signed char>(target.side);
                }
                target.side = user.side;
            }
        }
    }
    return state;
}

State& State::remove(QString& ret, Scene* const scene, Actor& actor)
{
    State& state = *this;
    state.blockSkills(actor, true);
    state.abandon(&ret, scene, actor, false);
    if (this->isConverted() && (!actor.isConverted()))
    {
        actor.side = static_cast<int>(actor.oldSide);
        actor.oldSide = -1;
    }
    return state;
}

inline bool State::disable(QString& ret, Actor& actor, int const dur, const bool remove)
{
    return this->disable(ret, nullptr, actor, remove, dur);
}

bool State::disable(QString& ret, Scene* const scene, Actor& actor, int dur, const bool remove)
{
    if (dur == 0)
    {
        dur = this->dur;
    }
    if (dur > STATE_END_DUR)
    {
        QMap<State*, int>* sDur = actor.stateDur;
        if (sDur == nullptr)
        {
            return true;
        }
        else
        {
            int crDur = sDur->value(this, STATE_END_DUR);
            //if (dur == -2 || (crDur > -2 && (dur == -1 || crDur > -1)))
            if (crDur > -1 || (dur <= crDur))
            {
                if (dur > 0 && crDur > 0 && crDur > dur)
                {
                    sDur->operator[](this) = crDur - dur;
                    return false;
                }
                else
                {
                    if (crDur > STATE_END_DUR)
                    {
                        this->remove(ret, scene, actor);
                    }
                    if (remove)
                    {
                        sDur->remove(this);
                    }
                    else
                    {
                        sDur->operator[](this) = STATE_END_DUR;
                    }
                    return true;
                }
            }
            return crDur <= STATE_END_DUR;
        }
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
        int const d = sDur->value(this, STATE_END_DUR);
        if (consume)
        {
            if (d > 0)
            {
                sDur->operator[](this) = d - 1;
            }
        }
        else if (d == 0)
        {
            state.remove(ret, scene, actor);
            sDur->operator[](this) = STATE_END_DUR;
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
             bool const range, bool const automate, bool const confuse, bool const reflect, bool const revive, QMap<int, int>* const res, QMap<State*, int>* const states,
             QMap<State*, int>* const stRes, QVector<Ability*>* const aSkills, QVector<Ability*>* const counters, QVector<Ability*>* const rSkills)
    : Costume(id, name, sprite, shapeShift, elm, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi, stun, range, automate, confuse, reflect, revive, res, states,
              aSkills, counters, stRes)
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
