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
#include "actor.h"

using namespace tbrpgsca;

Actor* Actor::setHpMod(const int hp)
{
    this->mHp = hp;
    return this;
}

Actor* Actor::setMpMod(const int mp)
{
    this->mMp = mp;
    return this;
}

Actor* Actor::setSpMod(const int sp)
{
    this->mSp = sp;
    return this;
}

Actor* Actor::setName(const QString& value)
{
    this->name = value;
    return this;
}

Actor* Actor::setSprite(const QString& value)
{
    this->sprite = value;
    return this;
}

int Role::getId() const
{
    return this->id;
}

Actor* Actor::setRanged(const bool range)
{
    int flags = this->flags;
    if (range != ((flags & FLAG_RANGE) == FLAG_RANGE))
    {
        this->flags = flags ^ FLAG_RANGE;
    }
    return this;
}

Actor* Actor::setReviving(const bool revive)
{
    int flags = this->flags;
    if (revive != ((flags & FLAG_REVIVE) == FLAG_REVIVE))
    {
        this->flags = flags ^ FLAG_REVIVE;
    }
    return this;
}

Actor::Actor()
{

}
