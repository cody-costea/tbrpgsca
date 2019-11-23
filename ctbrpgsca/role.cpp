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
#include "role.h"

using namespace ctbrpgsca;

int Role::getHpMod() const
{
    return this->mHp;
}

int Role::getMpMod() const
{
    return this->mMp;
}

int Role::getSpMod() const
{
    return this->mSp;
}

State* Role::getAddedState(int n) const
{
    QVector<State*>* aStates = this->aStates;
    return aStates == nullptr || n < 0 || n >= aStates->size() ? nullptr : aStates->at(n);
}

int Role::getAddedStatesSize() const
{
    QVector<State*>* aStates = this->aStates;
    return aStates == nullptr ? 0 : aStates->size();
}

QString Role::getName() const
{
    return this->name;
}

QString Role::getSprite() const
{
    return this->sprite;
}

int Role::getId() const
{
    return this->id;
}

bool Role::isRanged() const
{
    return (this->flags & FLAG_RANGE) == FLAG_RANGE;
}

bool Role::isReviving() const
{
    return (this->flags & FLAG_REVIVE) == FLAG_REVIVE;
}

Role::Role(const int id, const QString& name, const QString& sprite, const int hpDmg, const int mpDmg, const int spDmg,
           const int mHp, const int mMp, const int mSp, const bool range, const bool revive, QVector<State*>* const aStates)
{
    this->id = id;
    this->name = name;
    this->aStates = aStates;
    this->sprite = sprite;
    this->mHp = mHp;
    this->mHp = mMp;
    this->mSp = mSp;
    this->hp = hpDmg;
    this->mp = mpDmg;
    this->sp = spDmg;
    int flags = revive ? FLAG_REVIVE : 0;
    if (range)
    {
        flags = flags | FLAG_RANGE;
    }
    this->flags = flags;
}

Role::~Role()
{
    //delete this->name;
}
