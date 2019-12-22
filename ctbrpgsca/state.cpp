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

inline bool State::isStunning() const
{
    return (this->flags & FLAG_STUN) == FLAG_STUN;
}

inline State& State::inflict(QString &ret, Actor &actor, const bool always, const bool indefinite)
{
    return this->inflict(ret, nullptr, actor, always, indefinite);
}

State& State::inflict(QString& ret, Scene* scene, Actor& actor, const bool always, const bool indefinite)
{
    State& state = *this;
    QMap<State*, int>* stRes = actor.stRes;
    int const stateRes = state.sRes;
    if (always || stateRes < 0 || ((std::rand() % 10) > ((stRes == nullptr ? 0 : stRes->value(this, 0)) + stateRes)))
    {
        state.adopt(actor);
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
            actor.updateAttributes(false, scene, state);
            actor.updateResistance(false, state.res, state.stRes);
            actor.updateSkills(false, *(state.aSkills));
            state.blockSkills(actor, false);
            QString* sprite = state.sprite;
            if (sprite != nullptr && sprite->size() > 0)
            {
                actor.sprite = sprite;
                actor.setShapeShifted(true);
                //TODO: actor.resetSprites();
            }
            //state.apply(ret, scene, actor);
        }
    }
    return state;
}

State::State(int const id, QString& name, QString& sprite, int const dur, int const sRes, int const elm, int const hpDmg, int const mpDmg, int const spDmg,
             int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi, bool const stun, bool const range,
             bool const automate, bool const confuse, bool const reflect, bool const revive, QMap<int, int>* const res, QMap<State*, int>* const stRes,
             QVector<Ability*>* const aSkills, QVector<Ability*>* const rSkills)
    : Costume(id, name, sprite, elm, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi, range, automate, confuse, reflect, revive, res, aSkills, stRes)
{
    this->dur = dur;
    this->sRes = sRes;
    this->rSkills = rSkills;
    int flags = this->flags;
    if (stun)
    {
        flags |= FLAG_STUN;
    }
    this->flags = flags;
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
