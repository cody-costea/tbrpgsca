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

#include <QSharedPointer>
#include <QStringBuilder>

using namespace tbrpgsca;

QString Ability::MissesTxt = ", but misses %1";
QString Ability::ReflectTxt = ", reflected by %1";
QString Ability::StolenTxt = ", obtaining %1 from %2";

int Ability::removedStateDuration(int state) const
{
    auto aStates = this->_ability_data->_r_states;
    return aStates == NIL ? 0 : aStates->value(state, 0);
}

QList<int> Ability::removedStatesList() const
{
    auto aStates = this->_ability_data->_r_states;
    return aStates == NIL ? QList<int>() : aStates->keys();
}

bool Ability::removedState(int state) const
{
    auto aStates = this->_ability_data->_r_states;
    return aStates && aStates->contains(state);
}

int Ability::removedStatesSize() const
{
    auto aStates = this->_ability_data->_r_states;
    return aStates == NIL ? 0 : aStates->size();
}

void Ability::replenish()
{
    const Ability& ability = *this;
    int const mQty = ability.maximumUses();
    if (mQty != 0)
    {
        /*auto& userData = user._actor_data;
        QMap<const Ability*, int>* usrSkills = userData->_skills_cr_qty;
        if (usrSkills == NIL)
        {
            usrSkills = new QMap<const Ability*, int>();
            userData->_skills_cr_qty = usrSkills;
        }
        usrSkills->operator[](this) = mQty;*/
        this->setProperty("currentUses", QVariant(mQty > 0 ? mQty : 0));
        this->setProperty("regenTurn", QVariant(0));
    }
}

bool Ability::canPerform(Actor* const actor) const
{
    assert(actor);
    //QMap<const Ability*, int>* skillsQty = actor->_actor_data->_skills_cr_qty;
    int const crQty = this->property("currentUses").toInt();
    return this->maximumMp() <= actor->currentMp() && this->maximumHp() < actor->currentHp() && this->maximumRp() <= actor->currentRp()
            && actor->currentLevel() >= this->requiredLevel() && crQty > -1 && (this->maximumUses() == 0 || crQty > 0);
}

void Ability::execute(QString& ret, Actor& user, Actor& target, bool applyCosts)
{
    return this->execute(ret, user, &target, applyCosts);
}

void Ability::execute(QString& ret, Actor& user, Actor* target, bool const applyCosts)
{
    assert(target);
    const Ability& ability = *this;
    int const dmgType = ability.damageType() | user.damageType();
    if (target != &user && ((dmgType & target->reflectType()) != 0))
    {
        ret += Ability::ReflectTxt.arg(target->name());
        target = &user;
    }
    auto& userData = user._actor_data;
    {
        auto& usrCostData = user._costume_data;
        auto& trgCostData = target->_costume_data;
        int canMiss = ability.isMissable() ? 4 : 0, def = 0, i = 0, dmg = 0, usrAgi = usrCostData->_agi,
                trgAgi = trgCostData->_agi, trgSpi = trgCostData->_spi, usrWis = usrCostData->_wis;
        if ((dmgType & Role::Element::ATTACK) == Role::Element::ATTACK)
        {
            dmg += usrCostData->_atk;
            def += trgCostData->_def;
            ++i;
        }
        if ((dmgType & Role::Element::DEFENSE) == Role::Element::DEFENSE)
        {
            dmg += usrCostData->_def;
            def += trgCostData->_def;
            ++i;
        }
        if ((dmgType & Role::Element::SPIRIT) == Role::Element::SPIRIT)
        {
            dmg += usrCostData->_spi;
            def += trgCostData->_wis;
            ++i;
        }
        if ((dmgType & Role::Element::WISDOM) == Role::Element::WISDOM)
        {
            dmg += usrWis;
            def += trgSpi;
            ++i;
        }
        if ((dmgType & Role::Element::AGILITY) == Role::Element::AGILITY)
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
            ability.damage(ret, (ability.isAbsorbing() ? &user : NIL), *target, dmg, false);
            {
                QMap<State*, int>* aStates = ability._role_data->_state_dur;
                if (aStates)
                {
                    auto const last = aStates->cend();
                    for (auto it = aStates->cbegin(); it != last; ++it)
                    {
                        it.key()->inflict(&ret, &user, *target, it.value(), user.partySide() == target->partySide());
                    }
                }
            }
            {
                QMap<State*, int>* stateDur = target->_role_data->_state_dur;
                if (stateDur)
                {
                    auto rStates = ability._ability_data->_r_states;
                    if (rStates)
                    {
                        auto const rLast = rStates->cend();
                        for (auto rIt = rStates->cbegin(); rIt != rLast; ++rIt)
                        {
                            int const rDur = rIt.value();
                            if (rDur > State::EndDur)
                            {
                                auto rState = rIt.key();
                                auto const last = stateDur->cend();
                                for (auto it = stateDur->cbegin(); it != last; ++it)
                                {
                                    State* const aState = it.key();
                                    if (aState->databaseId() == rState)
                                    {
                                        if (it.value() > State::EndDur)
                                        {
                                            aState->disable(&ret, *target, rDur, false);
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
                auto& usrItems = userData->_items;
                //if (usrItems)
                {
                    int trgItemsSize;
                    auto& trgItems = target->_actor_data->_items;
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
                                    usrItems = QSharedPointer<QMap<Ability*, int>>();
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
        user.setCurrentHp(user.currentHp() - ability.maximumHp());
        int mQty = ability.maximumUses();
        if (mQty != 0)
        {
            /*QMap<const Ability*, int>* usrSkillsQty = userData->_skills_cr_qty;
            if (usrSkillsQty == NIL)
            {
                usrSkillsQty = new QMap<const Ability*, int>();
                userData->_skills_cr_qty = usrSkillsQty;
            }
            usrSkillsQty->operator[](this) = (usrSkillsQty->value(this, mQty) - 1);*/
            this->setProperty("currentUses", QVariant(this->property("currentUses").toInt() - 1));
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

Ability::Ability(int const id, QString& name, QString& sprite, QString& sound, bool const steal, bool const range, bool const melee, bool const canMiss, int const lvRq, int const hpC,
                 int const mpC, int const spC, int const dmgType, int const attrInc, int const hpDmg, int const mpDmg, int const spDmg, int const trg, int const elm, int const mQty,
                 int const rQty, bool const absorb, bool const revive, QMap<State*, int>* const aStates, QMap<int, int>* const rStates, QObject* const parent)
    : Role(id, name, sprite, hpDmg, mpDmg, spDmg, hpC, mpC, spC, (elm | dmgType), range, hpDmg < 0 && revive, aStates, parent)
{
    QSharedDataPointer<AbilityData> dataPtr(new AbilityData);
    dataPtr->_sound = sound.isNull() ? NIL : new QString(sound);
    dataPtr->_attr_inc = attrInc;
    dataPtr->_max_qty = mQty;
    dataPtr->_lv_rq = lvRq;
    dataPtr->_reg_qty = rQty;
    dataPtr->_r_states = rStates;
    int flags = this->playFlags();
    //this->_reg_turn = 0;
    //this->_crt_qty = 0;
    if (canMiss)
    {
        flags |= Attribute::MISSABLE;
    }
    if (melee)
    {
        flags |= Attribute::MELEE;
    }
    if (steal)
    {
        flags |= Attribute::STEAL;
    }
    if (absorb)
    {
        flags |= Attribute::ABSORB;
    }
    flags |= trg;
    this->setPlayFlags(flags);
    this->_ability_data = dataPtr;
}

Ability::Ability(int const id, QString&& name, QString&& sprite, QString&& sound, bool const steal, bool const range, bool const melee, bool const canMiss, int const lvRq, int const hpC,
                 int const mpC, int const spC, int const dmgType, int const attrInc, int const hpDmg, int const mpDmg, int const spDmg, int const trg, int const elm, int const mQty,
                 int const rQty, bool const absorb, bool const revive, QMap<State*, int>* const aStates, QMap<int, int>* const rStates, QObject* const parent)
    : Ability(id, name, sprite, sound, steal, range, melee, canMiss, lvRq, hpC, mpC, spC, dmgType, attrInc, hpDmg, mpDmg, spDmg, trg, elm, mQty, rQty, absorb, revive, aStates, rStates, parent) {}

Ability::Ability(QObject* const parent) : Ability(0, QString(), QString(), QString(), false, false, false, false, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, NIL, NIL, parent) {}

Ability::Ability(const Ability& ability) : Role(ability)
{
    this->_ability_data = ability._ability_data;
    //this->_reg_turn = ability._reg_turn;
    //this->_crt_qty = ability._crt_qty;
}

Ability::~Ability()
{
    QString* const sound = this->_ability_data->_sound;
    if (sound)
    {
        delete sound;
    }
}
