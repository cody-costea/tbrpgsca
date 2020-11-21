/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "ability.h"
#include "costume.h"
#include "actor.h"
#include "state.h"
#include "scene.h"
#include "role.h"

#include <QStringBuilder>

using namespace tbrpgsca;

QString Ability::MissesTxt = ", but misses %1";
QString Ability::ReflectTxt = ", reflected by %1";
QString Ability::StolenTxt = ", obtaining %1 from %2";

int Ability::removedStateDuration(State& state) const
{
    QMap<State*, int>* aStates = this->_ability_data->_r_states;
    return aStates == NIL ? 0 : aStates->value(&state, 0);
}

QList<State*> Ability::removedStatesList() const
{
    QMap<State*, int>* aStates = this->_ability_data->_r_states;
    return aStates == NIL ? QList<State*>() : aStates->keys();
}

bool Ability::removedState(State& state) const
{
    QMap<State*, int>* aStates = this->_ability_data->_r_states;
    return aStates && aStates->contains(&state);
}

int Ability::removedStatesSize() const
{
    QMap<State*, int>* aStates = this->_ability_data->_r_states;
    return aStates == NIL ? 0 : aStates->size();
}

void Ability::replenish(Actor& user)
{
    Ability& ability = *this;
    int const mQty = ability.maximumUses();
    if (mQty > 0)
    {
        auto& userData = user._actor_data;
        QMap<Ability*, int>* usrSkills = userData->_skills_cr_qty;
        if (usrSkills == NIL)
        {
            usrSkills = new QMap<Ability*, int>();
            userData->_skills_cr_qty = usrSkills;
        }
        usrSkills->operator[](this) = mQty;
    }

}

bool Ability::canPerform(Actor* const actor)
{
    assert(actor);
    QMap<Ability*, int>* skillsQty = actor->_actor_data->_skills_cr_qty;
    return this->maximumMp() <= actor->maximumMp() && this->maximumHp() < actor->maximumHp() && this->maximumRp() <= actor->maximumRp()
            && actor->level() >= this->requiredLevel() && (skillsQty == NIL || skillsQty->value(this, 1) > 0);
}

void Ability::execute(QString& ret, Actor& user, Actor& target, bool applyCosts)
{
    return this->execute(ret, NIL, user, &target, applyCosts);
}

void Ability::execute(QString& ret, Scene* const scene, Actor& user, Actor* target, bool const applyCosts)
{
    assert(target);
    Ability& ability = *this;
    int const dmgType = ability.dmgType() | user.dmgType();
    if (target != &user && ((dmgType & target->reflectDmgType()) != 0))
    {
        ret += Ability::ReflectTxt.arg(target->name());
        target = &user;
    }
    auto& userData = user._actor_data;
    {
        auto& usrCostData = user._costume_data;
        auto& trgCostData = target->_costume_data;
        int canMiss = ability.canMiss() ? 4 : 0, def = 0, i = 0, dmg = 0, usrAgi = usrCostData->_agi,
                trgAgi = trgCostData->_agi, trgSpi = trgCostData->_spi, usrWis = usrCostData->_wis;
        if ((dmgType & Role::Element::Attack) == Role::Element::Attack)
        {
            dmg += usrCostData->_atk;
            def += trgCostData->_def;
            ++i;
        }
        if ((dmgType & Role::Element::Defense) == Role::Element::Defense)
        {
            dmg += usrCostData->_def;
            def += trgCostData->_def;
            ++i;
        }
        if ((dmgType & Role::Element::Spirit) == Role::Element::Spirit)
        {
            dmg += usrCostData->_spi;
            def += trgCostData->_wis;
            ++i;
        }
        if ((dmgType & Role::Element::Wisdom) == Role::Element::Wisdom)
        {
            dmg += usrWis;
            def += trgSpi;
            ++i;
        }
        if ((dmgType & Role::Element::Agility) == Role::Element::Agility)
        {
            dmg += usrAgi;
            def += trgAgi;
            if (canMiss > 0)
            {
                canMiss = 3;
            }
            ++i;
        }
        trgAgi = ((trgAgi + trgSpi) / 2) / 3, usrAgi = (usrAgi + usrWis) / 2;
        if (canMiss == 0 || target == &user || ((canMiss = (std::rand() % usrAgi / 2)
                + (usrAgi / canMiss)) > trgAgi - (std::rand() % trgAgi)))
        {
            if (canMiss > (trgAgi * 2) + (std::rand() % trgAgi))
            {
                dmg = (dmg * 2) + (dmg / 2); //TODO: add text for critical
            }
            if (i != 0)
            {
                //def += std::rand() % (def / 2);
                //dmg += std::rand() % (dmg / 2);
                dmg = (ability.attributeIncrement() + (dmg / i)) - ((def / i) / 2);
                if (dmg < 0)
                {
                    dmg = 0;
                }
            }
            ability.damage(ret, scene, (ability.isAbsorbing() ? &user : NIL), *target, dmg, false);
            {
                QMap<State*, int>* aStates = ability._role_data->_state_dur;
                if (aStates)
                {
                    auto const last = aStates->cend();
                    for (auto it = aStates->cbegin(); it != last; ++it)
                    {
                        it.key()->inflict(&ret, scene, &user, *target, it.value(), user.partySide() == target->partySide());
                    }
                }
            }
            {
                QMap<State*, int>* stateDur = target->_role_data->_state_dur;
                if (stateDur)
                {
                    QMap<State*, int>* rStates = ability._ability_data->_r_states;
                    if (rStates)
                    {
                        auto const rLast = rStates->cend();
                        for (auto rIt = rStates->cbegin(); rIt != rLast; ++rIt)
                        {
                            int const rDur = rIt.value();
                            if (rDur > STATE_END_DUR)
                            {
                                State* const rState = rIt.key();
                                auto const last = stateDur->cend();
                                for (auto it = stateDur->cbegin(); it != last; ++it)
                                {
                                    State* const aState = it.key();
                                    if (aState == rState)
                                    {
                                        if (it.value() > STATE_END_DUR)
                                        {
                                            rState->disable(&ret, scene, *target, rDur, false);
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (ability.isStealing())
            {
                QMap<Ability*, int>* usrItems = userData->_items;
                //if (usrItems)
                {
                    int trgItemsSize;
                    QMap<Ability*, int>* trgItems = target->_actor_data->_items;
                    if (trgItems && trgItems != usrItems && (trgItemsSize = trgItems->size()) > 0
                            && (((std::rand() % 12) + usrCostData->_agi / 4) > 4 + trgCostData->_agi / 3))
                    {
                        int const itemId = std::rand() % trgItemsSize;
                        //if (itemId < trgItemsSize)
                        //{
                            int trgItemQty = 0;
                            Ability* stolen = NIL;
                            //Ability* stolen = trgItems->keys().at(itemId);
                            {
                                i = 0;
                                auto const last = trgItems->cend();
                                for (auto it = trgItems->cbegin(); it != last; ++it, ++i)
                                {
                                    if (i == itemId)
                                    {
                                        stolen = it.key();
                                        trgItemQty = it.value();
                                        break;
                                    }
                                }
                            }
                            if (trgItemQty > 0)
                            {
                                if (usrItems == NIL)
                                {
                                    usrItems = new QMap<Ability*, int>();
                                    user.setNewItems(true);
                                    userData->_items = usrItems;
                                }
                                usrItems->operator[](stolen) = usrItems->value(stolen, 0) + 1;
                                if (trgItemQty < 2)
                                {
                                    trgItems->remove(stolen);
                                }
                                else
                                {
                                    trgItems->operator[](stolen) = trgItemQty - 1;
                                }
                                ret += Ability::StolenTxt.arg(stolen->name(), target->name());
                            }
                        //}
                    }
                }
            }
        }
        else
        {
            ret += Ability::MissesTxt.arg(target->name());
        }
    }
    if (applyCosts)
    {
        user.setCurrentRp(user.currentRp() - ability.maximumRp());
        user.setCurrentMp(user.currentMp() - ability.maximumMp());
        user.setCurrentHp(user.currentHp() - ability.maximumHp(), &ret, scene, false);
        int mQty = ability.maximumUses();
        if (mQty > 0)
        {
            QMap<Ability*, int>* usrSkillsQty = userData->_skills_cr_qty;
            if (usrSkillsQty == NIL)
            {
                usrSkillsQty = new QMap<Ability*, int>();
                userData->_skills_cr_qty = usrSkillsQty;
            }
            usrSkillsQty->operator[](this) = (usrSkillsQty->value(this, mQty) - 1);
        }
    }

}

Ability::AbilityData::~AbilityData()
{
    QString* const sound = this->_sound;
    if (sound)
    {
        this->_sound = NIL;
        delete sound;
    }
}

Ability::Ability(int const id, QString name, QString sprite, QString sound, bool const steal, bool const range, bool const melee, bool const canMiss, int const lvRq,
                 int const hpC, int const mpC, int const spC, int const dmgType, int const attrInc, int const hpDmg, int const mpDmg, int const spDmg, int const trg,
                 int const elm, int const mQty, int const rQty, bool const absorb, bool const revive, QMap<State*, int>* const aStates, QMap<State*, int>* const rStates)
    : Role(id, name, sprite, hpDmg, mpDmg, spDmg, hpC, mpC, spC, (elm | dmgType), range, hpDmg < 0 && revive, aStates)
{
    QSharedDataPointer<AbilityData> dataPtr(new AbilityData);
    dataPtr->_sound = sound.length() > 0 ? new QString(sound) : NIL;
    dataPtr->_attr_inc = attrInc;
    dataPtr->_lv_rq = lvRq;
    dataPtr->_m_qty = mQty;
    dataPtr->_r_qty = rQty;
    dataPtr->_r_states = rStates;
    int flags = this->playFlags();
    if (canMiss)
    {
        flags |= Attribute::Missable;
    }
    if (melee)
    {
        flags |= Attribute::Melee;
    }
    if (steal)
    {
        flags |= Attribute::Steal;
    }
    if (absorb)
    {
        flags |= Attribute::Absorb;
    }
    flags |= trg;
    this->setPlayFlags(flags);
    this->_ability_data = dataPtr;
}

Ability::Ability(Ability& ability) : Role(ability)
{
    this->_ability_data = ability._ability_data;
}

Ability::~Ability()
{
    QString* const sound = this->_ability_data->_sound;
    if (sound)
    {
        delete sound;
    }
}
