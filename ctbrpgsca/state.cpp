/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "state.h"
#include "ability.h"
#include "costume.h"
#include "actor.h"
#include "scene.h"
#include "role.h"

#include <QStringBuilder>

using namespace tbrpgsca;

const Ability& State::removedSkill(int const n) const
{
    return *(this->_r_skills->at(n));
}

bool State::hasRemovedSkill(const Ability& skill) const
{
    QVector<const Ability*>* aSkills = this->_r_skills;
    return aSkills && aSkills->contains(&skill);
}

int State::removedSkillsSize() const
{
    QVector<const Ability*>* aSkills = this->_r_skills;
    return aSkills ? aSkills->size() : 0;
}

void State::inflict(QString& ret, Actor* user, Actor& target, int dur, const bool always) const
{
    return this->inflict(&ret, nullptr, user, target, dur, always);
}

void State::inflict(QString* const ret, Scene* const scene, Actor* const user, Actor& target, int stateDur, const bool always) const
{
    const State& state = *this;
    if (stateDur == 0)
    {
        stateDur = state._dur;
        if (stateDur == 0)
        {
            return;
        }
    }
    if (stateDur > STATE_END_DUR)
    {
        int stateRes;
        QMap<const State*, int>* stRes;
        if (always || (stateRes = state._s_res) < 0 || ((std::rand() % 10) > (((stRes = target._st_res)
                == nullptr ? 0 : stRes->value(this, 0) + stRes->value(nullptr, 0)) + stateRes)))
        {
            QMap<const State*, int>* trgStates = target._state_dur;
            if (trgStates)
            {
                QMap<const State*, int>* const rStates = state._state_dur;
                if (rStates)
                {
                    auto const rLast = rStates->cend();
                    for (auto rIt = rStates->cbegin(); rIt != rLast; ++rIt)
                    {
                        int rDur = rIt.value();
                        const State* const rState = rIt.key();
                        if (rDur == 0 || rDur <= STATE_END_DUR)
                        {
                            rDur = rState->_dur;
                        }
                        auto const last = trgStates->cend();
                        for (auto it = trgStates->cbegin(); it != last; it += 1)
                        {
                            const State* const aState = it.key();
                            if (aState == rState)
                            {
                                int const aDur = it.value();
                                if (aDur > STATE_END_DUR)
                                {
                                    if (aDur < 0 && rDur > aDur)
                                    {
                                        return;
                                    }
                                    rState->disable(ret, scene, target, rDur, false);
                                    if (rDur > 0 && aDur > 0)
                                    {
                                        stateDur -= aDur < rDur ? aDur : rDur;
                                        if (stateDur < 1)
                                        {
                                            return;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                trgStates = new QMap<const State*, int>();
                target._state_dur = trgStates;
            }
            state.blockSkills(target, false);
            int const crDur = trgStates->value(this, STATE_END_DUR);
            if (crDur == STATE_END_DUR)
            {
                state.adopt(ret, scene, target, false, false);
                trgStates->operator[](this) = stateDur;
            }
            else if ((crDur > -1 && crDur < stateDur) || (stateDur < 0 && stateDur < crDur))
            {
                trgStates->operator[](this) = stateDur;
            }
            if (user && this->isConverted() && target._side != user->_side)
            {
                target._side = user->_side;
            }
        }
    }

}

void State::remove(QString* const ret, Scene* const scene, Actor& actor) const
{
    const State& state = *this;
    state.blockSkills(actor, true);
    state.adopt(ret, scene, actor, false, true);
}

bool State::disable(Actor& actor, int const dur, const bool remove) const
{
    return this->disable(nullptr, nullptr, actor, remove, dur);
}

bool State::disable(QString* const ret, Scene* const scene, Actor& actor, int dur, const bool remove) const
{
    if (dur == 0)
    {
        dur = this->_dur;
    }
    if (dur > STATE_END_DUR)
    {
        QMap<const State*, int>* sDur = actor._state_dur;
        if (sDur)
        {
            int const crDur = sDur->value(this, STATE_END_DUR);
            //if (dur == -2 || (crDur > -2 && (dur == -1 || crDur > -1)))
            if (crDur > -1 || dur <= crDur)
            {
                if (dur > 0 /*&& crDur > 0*/ && crDur > dur)
                {
                    sDur->operator[](this) = crDur - dur;
                    return false;
                }
                else
                {
                    if (remove)
                    {
                        sDur->remove(this);
                    }
                    else
                    {
                        sDur->operator[](this) = STATE_END_DUR;
                    }
                    if (crDur > STATE_END_DUR)
                    {
                        this->remove(ret, scene, actor);
                    }
                    return true;
                }
            }
            return crDur <= STATE_END_DUR;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

void State::alter(QString& ret, Actor& actor, const bool consume) const
{
    return this->alter(&ret, nullptr, actor, consume);
}

void State::alter(QString* const ret, Scene* const scene, Actor& actor, const bool consume) const
{
    const State& state = *this;
    QMap<const State*, int>* sDur = actor._state_dur;
    if (sDur /*&& actor.hp > 0*/)
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
            sDur->operator[](this) = STATE_END_DUR;
            state.remove(ret, scene, actor);
        }
    }

}

void State::blockSkills(Actor& actor, const bool remove) const
{
    const State& state = *this;
    QVector<const Ability*>* rSkills = state._r_skills;
    if (rSkills)
    {
        QMap<const Ability*, int>* iSkills = actor._skills_cr_qty;
        if (remove)
        {
            if (iSkills)
            {
                for (const Ability* const skill : *rSkills)
                {
                    if (skill->_m_qty > 0)
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
                iSkills = new QMap<const Ability*, int>();
                actor._skills_cr_qty = iSkills;
            }
            for (const Ability* const skill : *rSkills)
            {
                iSkills->operator[](skill) = skill->_m_qty > 0 ? -1 * iSkills->value(skill, 0) : 0;
            }
        }
    }

}

State::State(int const id, QString name, QString sprite, bool const shapeShift, int const dur, int const sRes, int const mActions, int const elm, int const hpDmg,
             int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
             bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko, bool const invincible,
             bool const revive, QVector<const Ability*>* const aSkills, QVector<const Ability*>* const counters, QVector<const Ability*>* const rSkills,
             QMap<const State*, int>* const states, QMap<const State*, int>* const stRes, QMap<int, int>* const res)
    : Costume(id, name, sprite, shapeShift, mActions, elm, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi, stun, range, automate, confuse, reflect, ko,
              invincible, revive, aSkills, counters, states, stRes, res)
{
    if (convert)
    {
        this->_play_flags |= FLAG_CONVERT;
    }
    this->_dur = dur;
    this->_s_res = sRes;
    this->_r_skills = rSkills;
}

State::State(const State& state) : Costume(state)
{
    this->_dur = state._dur;
    this->_s_res = state._s_res;
    this->_r_skills = state._r_skills;
}

State::~State()
{

}
