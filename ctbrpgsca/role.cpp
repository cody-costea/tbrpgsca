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

Role* Role::setHpMod(const int hp)
{
    this->mHp = hp;
    return this;
}

int Role::getMpMod() const
{
    return this->mMp;
}

Role* Role::setMpMod(const int mp)
{
    this->mMp = mp;
    return this;
}

int Role::getSpMod() const
{
    return this->mSp;
}

Role* Role::setSpMod(const int sp)
{
    this->mSp = sp;
    return this;
}

int Role::getInitMod() const
{
    return this->mInit;
}

Role* Role::setInitMod(const int init)
{
    this->mInit = init;
    return this;
}

State** Role::getAddedStates() const
{
    return this->aStates;
}

Role* Role::setAddedStates(State** const  value)
{
    this->aStates = value;
    return this;
}

QString Role::getName() const
{
    return this->name;
}

Role* Role::setName(const QString& value)
{
    this->name = value;
    return this;
}

QString Role::getSprite() const
{
    return this->sprite;
}

Role* Role::setSprite(const QString& value)
{
    this->sprite = value;
    return this;
}

int Role::getId() const
{
    return this->id;
}

Role* Role::setId(const int value)
{
    this->id = value;
    return this;
}

bool Role::isRanged() const
{
    return (this->flags & FLAG_RANGE) == FLAG_RANGE;
}

Role* Role::setRanged(const bool range)
{
    int flags = this->flags;
    if (range != ((flags & FLAG_RANGE) == FLAG_RANGE))
    {
        this->flags = flags ^ FLAG_RANGE;
    }
    return this;
}

bool Role::isReviving() const
{
    return (this->flags & FLAG_REVIVE) == FLAG_REVIVE;
}

Role* Role::setReviving(const bool revive)
{
    int flags = this->flags;
    if (revive != ((flags & FLAG_REVIVE) == FLAG_REVIVE))
    {
        this->flags = flags ^ FLAG_REVIVE;
    }
    return this;
}

Role::Role(const int id, const QString& name, const QString& sprite, const int mHp, const int mMp,
           const int mSp, const int mInit, const bool range, const bool revive, State** const aStates)
{
    this->flags = 0;
    this->id = id;
    this->name = name;
    this->sprite = sprite;
    this->mHp = mHp;
    this->mHp = mMp;
    this->mSp = mSp;
    this->mInit = mInit;
    this->aStates = aStates;
    this->setReviving(revive);
    this->setRanged(range);
}

Role::~Role()
{
    //delete this->name;
}
