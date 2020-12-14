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

int State::removedSkillId(int const n) const
{
    return (this->_state_data->_r_skills->at(n));
}

bool State::hasRemovedSkillId(const int skill) const
{
    auto aSkills = this->_state_data->_r_skills;
    return aSkills && aSkills->contains(skill);
}

int State::removedSkillsIdsSize() const
{
    auto aSkills = this->_state_data->_r_skills;
    return aSkills == NIL ? 0 : aSkills->size();
}

void State::inflict(QString& ret, Actor* user, Actor& target, int dur, const bool always)
{
    return this->inflict(&ret, user, target, dur, always);
}

void State::inflict(QString* const ret, Actor* const user, Actor& target, int stateDur, const bool always)
{
    if (stateDur == 0)
    {
        stateDur = this->duration();
        if (stateDur == 0)
        {
            return;
        }
    }
    if (stateDur > State::EndDur)
    {
        int stateRes;
        QMap<int, int>* stRes;
        if (always || (stateRes = this->resistance()) < 0 || ((std::rand() % 10) > (((stRes = target._costume_data->_st_res)
                == NIL ? 0 : stRes->value(this->databaseId(), 0) + stRes->value(int(0), 0)) + stateRes)))
        {
            QSharedDataPointer<RoleData>& trgRoleData = target._role_data;
            QMap<State*, int>* trgStates = trgRoleData->_state_dur;
            if (trgStates == NIL)
            {
                trgStates = new QMap<State*, int>();
                trgRoleData->_state_dur = trgStates;
            }
            else
            {
                QMap<State*, int>* const rStates = this->_role_data->_state_dur;
                if (rStates)
                {
                    auto const rLast = rStates->cend();
                    for (auto rIt = rStates->cbegin(); rIt != rLast; ++rIt)
                    {
                        int rDur = rIt.value();
                        State* const rState = rIt.key();
                        if (rDur == 0 || rDur <= State::EndDur)
                        {
                            rDur = rState->duration();
                        }
                        auto const last = trgStates->cend();
                        for (auto it = trgStates->cbegin(); it != last; ++it)
                        {
                            State* const aState = it.key();
                            if (aState == rState)
                            {
                                int const aDur = it.value();
                                if (aDur > State::EndDur)
                                {
                                    if (aDur < 0 && rDur > aDur)
                                    {
                                        return;
                                    }
                                    rState->disable(ret, target, rDur, false);
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
            this->blockSkills(target, false);
            int const crDur = trgStates->value(this, State::EndDur);
            if (crDur == State::EndDur)
            {
                this->adopt(ret, target, false, false);
                trgStates->operator[](this) = stateDur;
            }
            else if ((crDur > -1 && crDur < stateDur) || (stateDur < 0 && stateDur < crDur))
            {
                trgStates->operator[](this) = stateDur;
            }
            if (user && this->isConverted() && target.partySide() != user->partySide())
            {
                target.setPartySide(user->partySide());
            }
        }
    }

}

void State::remove(QString* const ret, Actor& actor) const
{
    const State& state = *this;
    state.blockSkills(actor, true);
    state.adopt(ret, actor, false, true);
    if (this->isConverted() && (!actor.isConverted()))
    {
        actor.setPartySide(static_cast<int>(actor._actor_data->_old_side));
    }

}

bool State::disable(Actor& actor, int const dur, const bool remove)
{
    return this->disable(NIL, actor, remove, dur);
}

bool State::disable(QString* const ret, Actor& actor, int dur, const bool remove)
{
    if (dur == 0)
    {
        dur = this->duration();
    }
    if (dur > State::EndDur)
    {
        QMap<State*, int>* sDur = actor._role_data->_state_dur;
        if (sDur == NIL)
        {
            return true;
        }
        else
        {
            int const crDur = sDur->value(this, State::EndDur);
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
                        sDur->operator[](this) = State::EndDur;
                    }
                    if (crDur > State::EndDur)
                    {
                        this->remove(ret, actor);
                    }
                    return true;
                }
            }
            return crDur <= State::EndDur;
        }
    }
    else
    {
        return false;
    }
}

void State::alter(QString& ret, Actor& actor, const bool consume)
{
    return this->alter(&ret, actor, consume);
}

void State::alter(QString* const ret, Actor& actor, const bool consume)
{
    State& state = *this;
    QMap<State*, int>* sDur = actor._role_data->_state_dur;
    if (sDur /*&& actor.hp > 0*/)
    {
        int const d = sDur->value(this, State::EndDur);
        if (consume)
        {
            if (d > 0)
            {
                sDur->operator[](this) = d - 1;
            }
        }
        else if (d == 0)
        {
            sDur->operator[](this) = State::EndDur;
            state.remove(ret, actor);
        }
    }

}

void State::blockSkills(Actor& actor, const bool remove) const
{
    auto rSkills = this->_state_data->_r_skills;
    if (rSkills)
    {
        auto actorData = actor._actor_data;
        auto iSkills = actorData->_skills_cr_qty;
        if (remove)
        {
            if (iSkills)
            {
                auto const last = iSkills->cend();
                auto const begin = iSkills->cbegin();
                for (auto rSkill : *rSkills)
                {
                    for (auto it = begin; it != last; ++it)
                    {
                        auto iSkill = it.key();
                        if (iSkill->databaseId() == rSkill)
                        {
                            if (iSkill->maximumUses() > 0)
                            {
                                iSkills->operator[](iSkill) = -1 * iSkills->value(iSkill, 0);
                            }
                            else
                            {
                                iSkills->remove(iSkill);
                            }
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            if (iSkills == NIL)
            {
                iSkills = new QMap<const Ability*, int>();
                actorData->_skills_cr_qty = iSkills;
            }
            auto& aSkills = *(actor._costume_data->_a_skills);
            for (auto rSkill : *rSkills)
            {
                for (auto aSkill : aSkills)
                {
                    if (aSkill->databaseId() == rSkill)
                    {
                        iSkills->operator[](aSkill) = aSkill->maximumUses() > 0 ? -1 * iSkills->value(aSkill, 0) : 0;
                        break;
                    }
                }
            }
        }
    }

}

State::State(int const id, QString& name, QString& sprite, bool const shapeShift, int const dur, int const sRes, int const mActions, int const elm, int const hpDmg,
             int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
             bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko, bool const invincible,
             bool const revive, QList<Ability*>* const aSkills, QList<Ability*>* const counters, QVector<int>* const rSkills, QMap<State*, int>* const states,
             QMap<int, int>* const stRes, QMap<int, int>* const res, QObject* const parent)
    : Costume(id, name, sprite, shapeShift, mActions, elm, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi, stun, range, automate, confuse, reflect, ko,
              invincible, revive, aSkills, counters, states, stRes, res, parent)
{
    QSharedDataPointer<StateData> stateData(new StateData);
    stateData->_r_skills = rSkills;
    stateData->_s_res = sRes;
    stateData->_dur = dur;
    if (convert)
    {
        this->setPlayFlags(this->playFlags() | Costume::Attribute::CONVERT);
    }
    this->_state_data = stateData;
}

State::State(int const id, QString&& name, QString&& sprite, bool const shapeShift, int const dur, int const sRes, int const mActions, int const elm, int const hpDmg,
             int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
             bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko, bool const invincible,
             bool const revive, QList<Ability*>* const aSkills, QList<Ability*>* const counters, QVector<int>* const rSkills, QMap<State*, int>* const states,
             QMap<int, int>* const stRes, QMap<int, int>* const res, QObject* const parent)
    : State(id, name, sprite, shapeShift, dur, sRes, mActions, elm, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi, stun, range, automate, confuse, convert,
            reflect, ko, invincible, revive, aSkills, counters, rSkills, states, stRes, res, parent) {}

State::State(QObject* const parent) : State(0, QString(), QString(), false, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, false, false,
                                            false, false, false, false, NIL, NIL, NIL, NIL, NIL, NIL, parent) {}

State::State(const State& state) : Costume(state)
{
    this->_state_data = state._state_data;
}

State::StateData::~StateData() {}

State::~State() {}
