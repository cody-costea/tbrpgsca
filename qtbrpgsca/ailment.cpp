/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "ailment.h"
#include "ability.h"
#include "costume.h"
#include "actor.h"
#include "scene.h"
#include "role.h"

#include <QStringBuilder>

using namespace tbrpgsca;

int Ailment::removedSkillId(int const n) const
{
    return (this->stateData()._r_skills->at(n));
}

bool Ailment::hasRemovedSkillId(const int skill) const
{
    auto aSkills = this->stateData()._r_skills;
    return aSkills && aSkills->contains(skill);
}

int Ailment::removedSkillsIdsSize() const
{
    auto aSkills = this->stateData()._r_skills;
    return aSkills == NIL ? 0 : aSkills->size();
}

void Ailment::blockSkills(Actor& actor, const bool remove) const
{
    auto rSkills = this->stateData()._r_skills;
    if (rSkills)
    {
        //auto& actorData = actor._actor_data;
        auto& aSkills = *actor.costumeData()._a_skills;
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
        return;
    }
    if (stateDur > Ailment::EndDur)
    {
        int stateRes;
        QMap<int, int>* stRes;
        if (always || (stateRes = this->resistance()) < 0 || ((std::rand() % 10) > (((stRes = target.costumeData()._st_res)
                == NIL ? 0 : stRes->value(this->databaseId(), 0) + stRes->value(int(0), 0)) + stateRes)))
        {
            int trgStatesSize;
            auto& trgRoleData = target.roleMutData();
            QList<Ailment>* trgStates = trgRoleData._a_states;
            if (trgStates == NIL)
            {
                trgStates = new QList<Ailment>();
                trgRoleData._a_states = trgStates;
                trgStatesSize = 0;
            }
            else
            {
                trgStatesSize = trgStates->size();
                QList<Ailment>* const rStates = this->roleData()._a_states;
                if (rStates)
                {
                    for (auto& rState : *rStates)
                    {
                        int rDur = rState.duration();
                        /*if (rDur == 0 || rDur <= Ailment::EndDur)
                        {
                            rDur = rState.maximumDuration();
                        }*/
                        for (int i = 0; i < trgStatesSize; ++i)
                        {
                            //Ailment* const aState = it.key();
                            Ailment& aState = static_cast<Ailment&>(trgStates->operator[](i));
                            if (aState.databaseId() == rState.databaseId())
                            {
                                int const aDur = aState.duration();//maximumDuration();
                                if (aDur > Ailment::EndDur)
                                {
                                    if (aDur < 0 && rDur > aDur)
                                    {
                                        return;
                                    }
                                    aState.disable(ret, target, rDur, false);
                                    if (rDur > 0 && aDur > 0)
                                    {
                                        stateDur -= rDur;//aDur < rDur ? aDur : rDur;
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
            Ailment* state = NIL;
            //auto crState = static_cast<Ailment*>(this); //TODO: instantiate new State from Ailment when necessary;
            for (int i = 0; i < trgStatesSize; ++i)
            {
                Ailment& crState = static_cast<Ailment&>(trgStates->operator[](i));
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
                    crDur = state->duration();//trgStates->value(this, Ailment::EndDur);
                    if (crDur == Ailment::EndDur)
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
                    trgStates->append(Ailment(this)); //TODO: rewrite in a better way;
                    state = &static_cast<Ailment&>(trgStates->operator[](trgStatesSize));
                    state->adopt(ret, target, false, false);
                }
                state->setDuration(stateDur);
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

void Ailment::remove(QString* const ret, Actor& actor) const
{
    const Ailment& state = *this;
    state.blockSkills(actor, true);
    state.adopt(ret, actor, false, true);
    if (this->isConverted() && (!actor.isConverted()))
    {
        actor.setPartySide(static_cast<int>(actor.actorData()._old_side));
    }
}

bool Ailment::disable(Actor& actor, int const dur, const bool remove)
{
    return this->disable(NIL, actor, remove, dur);
}

bool Ailment::disable(QString* const ret, Actor& actor, int dur, const bool remove)
{
    if (dur == 0)
    {
        //dur = this->maximumDuration();
        return false;
    }
    if (dur > Ailment::EndDur)
    {
        QList<Ailment>* sDur = actor.roleData()._a_states;
        if (sDur == NIL)
        {
            return true;
        }
        else
        {
            int const crDur = this->duration();//sDur->value(this, Ailment::EndDur);
            //if (dur == -2 || (crDur > -2 && (dur == -1 || crDur > -1)))
            if (crDur > -1 || dur <= crDur)
            {
                if (dur > 0 /*&& crDur > 0*/ && crDur > dur)
                {
                    //sDur->operator[](this) = crDur - dur;
                    this->setDuration(crDur - dur);
                    return false;
                }
                else
                {
                    if (remove)
                    {
                        sDur->removeOne(*this);
                    }
                    else
                    {
                        //sDur->operator[](this) = Ailment::EndDur;
                        this->setDuration(Ailment::EndDur);
                    }
                    if (crDur > Ailment::EndDur)
                    {
                        this->remove(ret, actor);
                    }
                    return true;
                }
            }
            return crDur <= Ailment::EndDur;
        }
    }
    else
    {
        return false;
    }
}

void Ailment::alter(QString& ret, Actor& actor, const bool consume)
{
    return this->alter(&ret, actor, consume);
}

void Ailment::alter(QString* const ret, Actor& actor, const bool consume)
{
    //QList<Ailment>* sDur = actor.roleData()._a_states;
    //if (sDur /*&& actor.hp > 0*/)
    {
        int const d = this->duration();//sDur->value(this, Ailment::EndDur);
        if (consume)
        {
            if (d > 0)
            {
                //sDur->operator[](this) = d - 1;
                this->setDuration(d - 1);
            }
        }
        else if (d == 0)
        {
            //sDur->operator[](this) = Ailment::EndDur;
            this->setDuration(Ailment::EndDur);
            this->remove(ret, actor);
        }
    }

}

void Ailment::apply(QString& ret, Actor& actor) const
{
    this->Costume::apply(ret, actor);
    const_cast<Ailment*>(this)->alter(&ret, actor, true);
}

void Ailment::abandon(QString& ret, Actor& actor) const
{
    this->remove(&ret, actor);
}

Ailment::AilmentSheet::AilmentSheet(int const id, QString& name, QString& sprite, bool const shapeShift, int const sRes, int const mActions, int const elm, int const blockedSkillTypes,
                                    int const hpDmg, int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis,
                                    int const agi, bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko,
                                    bool const invincible, bool const revive, QList<Ability>* const aSkills, QList<Ability>* const counters, QVector<int>* const rSkills,
                                    QList<Ailment>* const states, QMap<int, int>* const stRes, QMap<int, int>* const res)
    : CostumeSheet(id, name, sprite, shapeShift, mActions, elm, blockedSkillTypes, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi,
                   wis, agi, stun, range, automate, confuse, reflect, ko, invincible, revive, aSkills, counters, states, stRes, res)
{
    this->_r_skills = rSkills;
    this->_s_res = sRes;
    //this->_crt_dur = 0;
    if (convert)
    {
        this->_play_flags = this->_play_flags | Costume::Attribute::CONVERT;
    }
}

Ailment::Ailment(int const id, QString& name, QString& sprite, bool const shapeShift, int const dur, int const sRes, int const mActions, int const elm, int const blockedSkillTypes,
                 int const hpDmg, int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
                 bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko, bool const invincible, bool const revive,
                 QList<Ability>* const aSkills, QList<Ability>* const counters, QVector<int>* const rSkills, QList<Ailment>* const states, QMap<int, int>* const stRes, QMap<int, int>* const res,
                 QObject* const parent)
    : Costume(parent, new AilmentSheet(id, name, sprite, shapeShift,sRes, mActions, elm, blockedSkillTypes, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis,
                                       agi, stun, range, automate, confuse, convert, reflect, ko, invincible, revive, aSkills, counters, rSkills, states, stRes, res))
{
    this->_dur = dur;
}

Ailment::Ailment(int const id, QString&& name, QString&& sprite, bool const shapeShift, int const dur, int const sRes, int const mActions, int const elm, int const blockedSkillTypes,
                 int const hpDmg, int const mpDmg, int const spDmg, int const mHp, int const mMp, int const mSp, int const atk, int const def, int const spi, int const wis, int const agi,
                 bool const stun, bool const range, bool const automate, bool const confuse, bool const convert, bool const reflect, bool const ko, bool const invincible, bool const revive, QList<Ability>* const aSkills, QList<Ability>* const counters, QVector<int>* const rSkills, QList<Ailment>* const states,
                 QMap<int, int>* const stRes, QMap<int, int>* const res, QObject* const parent)
    : Ailment(id, name, sprite, shapeShift, dur, sRes, mActions, elm, blockedSkillTypes, hpDmg, mpDmg, spDmg, mHp, mMp, mSp, atk, def, spi, wis, agi,
              stun, range, automate, confuse, convert, reflect, ko, invincible, revive, aSkills, counters, rSkills, states, stRes, res, parent) {}

Ailment::Ailment(QObject* const parent) : Ailment(0, QString(), QString(), false, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, false, false,
                                                  false, false, false, false, NIL, NIL, NIL, NIL, NIL, NIL, parent) {}

Ailment::Ailment(const Ailment&& state) : Ailment(static_cast<const Ailment&>(state)) {}

Ailment::Ailment(const Ailment& state) : Costume(state) {}

Ailment::AilmentSheet::~AilmentSheet() {}

Ailment::~Ailment() {}
