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

int Ailment::removedSkillId(int const n) const
{
    return (this->_state_data->_r_skills->at(n));
}

bool Ailment::hasRemovedSkillId(const int skill) const
{
    auto aSkills = this->_state_data->_r_skills;
    return aSkills && aSkills->contains(skill);
}

int Ailment::removedSkillsIdsSize() const
{
    auto aSkills = this->_state_data->_r_skills;
    return aSkills == NIL ? 0 : aSkills->size();
}

void Ailment::blockSkills(Actor& actor, const bool remove) const
{
    auto rSkills = this->_state_data->_r_skills;
    if (rSkills)
    {
        //auto& actorData = actor._actor_data;
        auto& aSkills = *actor._costume_data->_a_skills;
        //auto iSkills = actorData->_skills_cr_qty;
        if (remove)
        {
            /*if (iSkills)
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
            }*/
            for (int const rSkillId : *rSkills)
            {
                for (Ability& aSkill : aSkills)
                {
                    if (aSkill.databaseId() == rSkillId)
                    {
                        const QVariant& crQty = aSkill.property("currentUses");
                        if (!crQty.isNull())
                        {
                            int const crQtyInt = crQty.toInt();
                            aSkill.setProperty("currentUses", crQtyInt == -1 ? QVariant() : QVariant((crQtyInt + 2) * -1));
                        }
                    }
                }
            }
        }
        else
        {
            for (int const rSkillId : *rSkills)
            {
                for (Ability& aSkill : aSkills)
                {
                    if (aSkill.databaseId() == rSkillId)
                    {
                        aSkill.setProperty("currentUses", QVariant(aSkill.maximumUses() == 0 ? -1 : ((aSkill.property("currentUses").toInt() * -1) - 2)));
                    }
                }
            }
        }
    }

}

void Ailment::inflict(QString& ret, Actor* user, Actor& target, int dur, const bool always)
{
    return this->inflict(&ret, user, target, dur, always);
}

void Ailment::inflict(QString* const ret, Actor* const user, Actor& target, int stateDur, const bool always)
{
    if (stateDur == 0)
    {
        stateDur = this->maximumDuration();
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
            int trgStatesSize;
            QSharedDataPointer<RoleData>& trgRoleData = target._role_data;
            QList<Ailment>* trgStates = trgRoleData->_a_states;
            if (trgStates == NIL)
            {
                trgStates = new QList<Ailment>();
                trgRoleData->_a_states = trgStates;
                trgStatesSize = 0;
            }
            else
            {
                trgStatesSize = trgStates->size();
                QList<Ailment>* const rStates = this->_role_data->_a_states;
                if (rStates)
                {
                    //auto const rLast = rStates->cend();
                    //for (auto rIt = rStates->cbegin(); rIt != rLast; ++rIt)
                    for (auto& rState : *rStates)
                    {
                        int rDur = rState.maximumDuration();//rIt.value();
                        //Ailment* const rState = rIt.key();
                        if (rDur == 0 || rDur <= State::EndDur)
                        {
                            rDur = rState.maximumDuration();
                        }
                        //auto const last = trgStates->cend();
                        //for (auto it = trgStates->cbegin(); it != last; ++it)
                        //for (auto& aState : *trgStates)
                        for (int i = 0; i < trgStatesSize; ++i)
                        {
                            //Ailment* const aState = it.key();
                            State& aState = static_cast<State&>(trgStates->operator[](i));
                            if (aState.databaseId() == rState.databaseId())
                            {
                                int const aDur = aState.maximumDuration();//it.value();
                                if (aDur > State::EndDur)
                                {
                                    if (aDur < 0 && rDur > aDur)
                                    {
                                        return;
                                    }
                                    aState.disable(ret, target, rDur, false);
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
            State* state = NIL;
            //auto crState = static_cast<State*>(this); //TODO: instantiate new State from Ailment when necessary;
            for (int i = 0; i < trgStatesSize; ++i)
            {
                State& crState = static_cast<State&>(trgStates->operator[](i));
                if (crState.databaseId() == this->databaseId())
                {
                    state = &crState;
                    break;
                }
            }
            do
            {
                int crDur;
                if (state)
                {
                    crDur = state->currentDuration();//trgStates->value(this, State::EndDur);
                    if (crDur == State::EndDur)
                    {
                        state->adopt(ret, target, false, false);
                        //trgStates->operator[](this) = stateDur;
                    }
                    else if (!((crDur > -1 && crDur < stateDur) || (stateDur < 0 && stateDur < crDur)))
                    {
                        break;
                    }
                }
                else
                {
                    trgStates->append(State(this)); //TODO: rewrite in a better way;
                    state = &static_cast<State&>(trgStates->operator[](trgStatesSize));
                    state->adopt(ret, target, false, false);
                }
                state->setCurrentDuration(stateDur);
            }
            while (false);
            state->blockSkills(target, false);
            if (user && this->isConverted() && target.partySide() != user->partySide())
            {
                target.setPartySide(user->partySide());
            }
        }
    }

}

Ailment::Ailment(int const id, QString& name, QString& sprite, bool const shapeShift, int const dur, int const sRes, int const mActions, int const elm, int const hpDmg,
             int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
             bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko, bool const invincible,
             bool const revive, QList<Ability>* const aSkills, QList<Ability>* const counters, QVector<int>* const rSkills, QList<Ailment>* const states,
             QMap<int, int>* const stRes, QMap<int, int>* const res, QObject* const parent)
    : Costume(id, name, sprite, shapeShift, mActions, elm, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi, stun, range, automate, confuse, reflect, ko,
              invincible, revive, aSkills, counters, states, stRes, res, parent)
{
    QSharedDataPointer<AilmentData> stateData(new AilmentData);
    stateData->_r_skills = rSkills;
    stateData->_s_res = sRes;
    this->_max_dur = dur;
    //this->_crt_dur = 0;
    if (convert)
    {
        this->setPlayFlags(this->playFlags() | Costume::Attribute::CONVERT);
    }
    this->_state_data = stateData;
}

Ailment::Ailment(int const id, QString&& name, QString&& sprite, bool const shapeShift, int const dur, int const sRes, int const mActions, int const elm, int const hpDmg,
             int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
             bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko, bool const invincible,
             bool const revive, QList<Ability>* const aSkills, QList<Ability>* const counters, QVector<int>* const rSkills, QList<Ailment>* const states,
             QMap<int, int>* const stRes, QMap<int, int>* const res, QObject* const parent)
    : Ailment(id, name, sprite, shapeShift, dur, sRes, mActions, elm, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi, stun, range, automate, confuse, convert,
            reflect, ko, invincible, revive, aSkills, counters, rSkills, states, stRes, res, parent) {}

Ailment::Ailment(QObject* const parent) : Ailment(0, QString(), QString(), false, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, false, false,
                                            false, false, false, false, NIL, NIL, NIL, NIL, NIL, NIL, parent) {}

Ailment::Ailment(const Ailment& state) : Costume(state)
{
    this->_state_data = state._state_data;
    //this->_crt_dur = state._crt_dur;
}

Ailment::AilmentData::~AilmentData() {}

Ailment::~Ailment() {}
