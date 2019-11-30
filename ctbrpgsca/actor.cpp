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

Actor* Actor::setCurrentHp(const int hp)
{
    this->hp = hp;
    return this;
}

Actor* Actor::setCurrentMp(const int mp)
{
    this->mp = mp;
    return this;
}

Actor* Actor::setCurrentRp(const int sp)
{
    this->sp = sp;
    return this;
}

Actor* Actor::setMaximumHp(const int mHp)
{
    this->mHp = mHp;
    return this;
}

Actor* Actor::setMaximumMp(const int mMp)
{
    this->mMp = mMp;
    return this;
}

Actor* Actor::setMaximumRp(const int mMp)
{
    this->mSp = mMp;
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
