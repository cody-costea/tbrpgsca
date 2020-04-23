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
QString Ability::SuffersTxt = ", %1 suffers";
QString Ability::ReflectTxt = ", reflected by %1";
QString Ability::ResistTxt = ", resisted by %1";
QString Ability::StolenTxt = ", obtaining %1 from %2";

int Ability::getUsesRegen() const
{
    return this->rQty;
}

int Ability::getMaximumUses() const
{
    return this->mQty;
}

int Ability::getAttributeIncrement() const
{
    return this->attrInc;
}

int Ability::getRequiredLevel() const
{
    return this->lvRq;
}

int Ability::getRemovedStateDuration(State& state) const
{
    QMap<State*, int>* aStates = this->rStates;
    return aStates == nullptr ? 0 : aStates->value(&state, 0);
}

QList<State*> Ability::getRemovedStatesList() const
{
    QMap<State*, int>* aStates = this->rStates;
    return aStates == nullptr ? QList<State*>() : aStates->keys();
}

bool Ability::hasRemovedState(State& state) const
{
    QMap<State*, int>* aStates = this->rStates;
    return aStates != nullptr && aStates->contains(&state);
}

int Ability::getRemovedStatesSize() const
{
    QMap<State*, int>* aStates = this->rStates;
    return aStates == nullptr ? 0 : aStates->size();
}

bool Ability::canMiss() const
{
    return (this->flags & FLAG_MISSABLE) == FLAG_MISSABLE;
}

bool Ability::isStealing() const
{
    return (this->flags & FLAG_STEAL) == FLAG_STEAL;
}

bool Ability::isAbsorbing() const
{
    return (this->flags & FLAG_ABSORB) == FLAG_ABSORB;
}

bool Ability::isOnlyMelee() const
{
    return (this->flags & FLAG_MELEE) == FLAG_MELEE;
}

bool Ability::targetsSide() const
{
    return (this->flags & FLAG_TRG_SIDE) == FLAG_TRG_SIDE;
}

bool Ability::targetsSelf() const
{
    return (this->flags & FLAG_TRG_SELF) == FLAG_TRG_SELF;
}

bool Ability::targetsAll() const
{
    return (this->flags & FLAG_TRG_ALL) == FLAG_TRG_ALL;
}

Ability& Ability::replenish(Actor& user)
{
    Ability& ability = *this;
    int const mQty = ability.mQty;
    if (mQty > 0)
    {
        QMap<Ability*, int>* usrSkills = user.skillsCrQty;
        if (usrSkills == nullptr)
        {
            usrSkills = new QMap<Ability*, int>();
            user.skillsCrQty = usrSkills;
        }
        usrSkills->operator[](this) = mQty;
    }
    return ability;
}

bool Ability::canPerform(Actor& actor)
{
    QMap<Ability*, int>* skillsQty = actor.skillsCrQty;
    return this->mMp <= actor.mp && this->mHp < actor.hp && this->mSp <= actor.sp && actor.lv >= this->lvRq
                    && (skillsQty == nullptr || skillsQty->value(this, 1) > 0);
}

Ability& Ability::execute(QString& ret, Actor& user, Actor& target, bool applyCosts)
{
    return this->execute(ret, nullptr, user, &target, applyCosts);
}

Ability& Ability::execute(QString& ret, Scene* const scene, Actor& user, Actor* target, bool const applyCosts)
{
    assert(target != nullptr);
    Ability& ability = *this;
    int const dmgType = ability.dmgType | user.dmgType;
    if (dmgType == DMG_TYPE_WIS && target != &user && target->isReflecting())
    {
        ret = ret % Ability::ReflectTxt.arg(target->name);
        target = &user;
    }
    {
        int canMiss = ability.canMiss() ? 3 : 0, def = 0, i = 0, dmg = 0, usrAgi = user.agi,
                trgAgi = target->agi, trgSpi = target->spi, usrWis = user.wis;
        if ((dmgType & DMG_TYPE_ATK) == DMG_TYPE_ATK)
        {
            dmg += user.atk;
            def += target->def;
            if (canMiss > 0)
            {
                canMiss = 4;
            }
            ++i;
        }
        if ((dmgType & DMG_TYPE_DEF) == DMG_TYPE_DEF)
        {
            dmg += user.def;
            def += target->def;
            ++i;
        }
        if ((dmgType & DMG_TYPE_SPI) == DMG_TYPE_SPI)
        {
            dmg += user.spi;
            def += target->wis;
            ++i;
        }
        if ((dmgType & DMG_TYPE_WIS) == DMG_TYPE_WIS)
        {
            dmg += usrWis;
            def += trgSpi;
            ++i;
        }
        if ((dmgType & DMG_TYPE_AGI) == DMG_TYPE_AGI)
        {
            dmg += usrAgi;
            def += trgAgi;
            if (canMiss > 0)
            {
                canMiss = 2;
            }
            ++i;
        }
        trgAgi = ((trgAgi + trgSpi) / 2) / 3, usrAgi = (usrAgi + usrWis) / 2;
        if (canMiss == 0 || ((canMiss = (std::rand() % usrAgi / 2) + (usrAgi / canMiss))
                    > trgAgi - (std::rand() % trgAgi)) || target == &user)
        {
            if (canMiss > (trgAgi * 2) + (std::rand() % trgAgi))
            {
                dmg = (dmg * 2) + (dmg / 2); //TODO: add text for critical
            }
            if (i != 0)
            {
                def += std::rand() % (def / 2);
                dmg += std::rand() % (dmg / 2);
                dmg = (ability.attrInc + (dmg / i)) - ((def / i) / 2);
                if (dmg < 0)
                {
                    dmg = 0;
                }
            }
            ability.damage(ret, scene, (ability.isAbsorbing() ? &user : nullptr), *target, dmg, false);
            {
                QMap<State*, int>* aStates = ability.stateDur;
                if (aStates != nullptr)
                {
                    auto const last = aStates->cend();
                    for (auto it = aStates->cbegin(); it != last; ++it)
                    {
                        State* const state = it.key();
                        state->inflict(&ret, scene, &user, *target, it.value(), user.side == target->side);
                    }
                }
            }
            {
                QMap<State*, int>* stateDur = target->stateDur;
                if (stateDur != nullptr)
                {
                    QMap<State*, int>* rStates = ability.rStates;
                    if (rStates != nullptr)
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
                QMap<Ability*, int>* usrItems = user.items;
                if (usrItems != nullptr)
                {
                    int trgItemsSize;
                    QMap<Ability*, int>* trgItems = target->items;
                    if (trgItems != nullptr && trgItems != usrItems && (trgItemsSize = trgItems->size()) > 0
                            && (((std::rand() % 12) + user.agi / 4) > 4 + target->agi / 3))
                    {
                        int const itemId = std::rand() % trgItemsSize;
                        if (itemId < trgItemsSize)
                        {
                            int trgItemQty = 0;
                            Ability* stolen = nullptr;
                            //Ability* stolen = trgItems->keys().at(itemId);
                            {
                                int i = 0;
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
                                usrItems->operator[](stolen) = usrItems->value(stolen, 0) + 1;
                                if ((--trgItemQty) == 0)
                                {
                                    trgItems->remove(stolen);
                                }
                                else
                                {
                                    trgItems->operator[](stolen) = trgItemQty;
                                }
                                ret = ret % Ability::StolenTxt.arg(stolen->name, target->name);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            ret = ret % Ability::MissesTxt.arg(target->name);
        }
    }
    if (applyCosts)
    {
        user.setCurrentRp(user.sp - ability.mSp);
        user.setCurrentMp(user.mp - ability.mMp);
        user.setCurrentHp(user.hp - ability.mHp, &ret, scene, false);
        int mQty = ability.mQty;
        if (mQty > 0)
        {
            QMap<Ability*, int>* usrSkillsQty = user.skillsCrQty;
            if (usrSkillsQty == nullptr)
            {
                usrSkillsQty = new QMap<Ability*, int>();
                user.skillsCrQty = usrSkillsQty;
            }
            usrSkillsQty->operator[](this) = (usrSkillsQty->value(this, mQty) - 1);
        }
    }
    return ability;
}

Ability::Ability(int const id, QString name, QString sprite, bool const steal, bool const range, bool const melee, bool const canMiss, int const lvRq, int const hpC,
                 int const mpC, int const spC, int const dmgType, int const attrInc, int const hpDmg, int const mpDmg, int const spDmg, int const trg, int const elm,
                 int const mQty, int const rQty, bool const absorb, bool const revive, QMap<State*, int>* const aStates, QMap<State*, int>* const rStates)
    : Role(id, name, sprite, hpDmg, mpDmg, spDmg, hpC, mpC, spC, (elm | dmgType), range, revive, aStates)
{
    this->lvRq = lvRq;
    this->mQty = mQty;
    this->rQty = rQty;
    this->dmgType = dmgType;
    this->attrInc = attrInc;
    this->rStates = rStates;
    int flags = this->flags;
    if (canMiss)
    {
        flags |= FLAG_MISSABLE;
    }
    if (melee)
    {
        flags |= FLAG_MELEE;
    }
    if (steal)
    {
        flags |= FLAG_STEAL;
    }
    if (absorb)
    {
        flags |= FLAG_ABSORB;
    }
    flags |= trg;
    this->flags = flags;
}

Ability::Ability(Ability& ability) : Role(ability)
{
    this->dmgType = ability.dmgType;
    this->lvRq = ability.lvRq;
    this->mQty = ability.mQty;
    this->rQty = ability.rQty;
    this->dmgType = ability.dmgType;
    this->attrInc = ability.attrInc;
    this->stateDur = ability.stateDur;
    this->rStates = ability.rStates;
}

Ability::~Ability()
{

}
