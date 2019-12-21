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
#include "ability.h"

using namespace tbrpgsca;

QString Ability::MissesTxt = ", but misses";
QString Ability::SuffersTxt = ", %s suffers";
QString Ability::ReflectTxt = ", reflected by %s";
QString Ability::StolenTxt = ", obtaining %s from %s";

inline int Ability::getUsesRegen() const
{
    return this->rQty;
}

inline int Ability::getMaximumUses() const
{
    return this->mQty;
}

inline int Ability::getAttributeIncrement() const
{
    return this->attrInc;
}

inline int Ability::getRequiredLevel() const
{
    return this->lvRq;
}

inline int Ability::getDamageType() const
{
    return this->dmgType;
}

inline State& Ability::getRemovedState(int const n) const
{
    return *(this->rStates->at(n));
}

inline bool Ability::hasRemovedState(State& state) const
{
    QVector<State*>* rStates = this->rStates;
    return rStates != nullptr && rStates->contains(&state);
}

inline int Ability::getRemovedStatesSize() const
{
    QVector<State*>* rStates = this->rStates;
    return rStates == nullptr ? 0 : rStates->size();
}

inline bool Ability::isStealing() const
{
    return (this->flags & FLAG_STEAL) == FLAG_STEAL;
}

inline bool Ability::isAbsorbing() const
{
    return (this->flags & FLAG_ABSORB) == FLAG_ABSORB;
}

inline bool Ability::isOnlyMelee() const
{
    return (this->flags & FLAG_MELEE) == FLAG_MELEE;
}

inline bool Ability::targetsSide() const
{
    return (this->flags & FLAG_TRG_SIDE) == FLAG_TRG_SIDE;
}

inline bool Ability::targetsSelf() const
{
    return (this->flags & FLAG_TRG_SELF) == FLAG_TRG_SELF;
}

inline bool Ability::targetsAll() const
{
    return (this->flags & FLAG_TRG_ALL) == FLAG_TRG_ALL;
}

Ability::Ability(int const id, QString& name, QString& sprite, bool const steal, bool const range, bool const melee, int const lvRq,
                 int const hpC, int const mpC, int const spC, int const dmgType, int const attrInc, int const hpDmg, int const mpDmg, int const spDmg,
                 int const trg, int const elm,int const mQty, int const rQty, bool const absorb, bool const revive, QVector<State*>* const aStates,
                 QVector<State*>* const rStates)
    : Role(id, name, sprite, hpC, mpC, spC, hpDmg, mpDmg, spDmg, elm, range, revive, aStates)
{
    this->elm = elm;
    this->lvRq = lvRq;
    this->mQty = mQty;
    this->rQty = rQty;
    this->dmgType = dmgType;
    this->attrInc = attrInc;
    this->rStates = rStates;
    int flags = this->flags;
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
