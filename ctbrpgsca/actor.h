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
#ifndef ACTOR_H
#define ACTOR_H

#include "costume.h"

namespace tbrpgsca
{

    class Actor : public Costume
    {
    public:
        Actor* setName(const QString& value);
        Actor* setSprite(const QString& value);
        Actor* setHpMod(const int hp);
        Actor* setMpMod(const int mp);
        Actor* setSpMod(const int sp);
        Actor* setInitMod(const int init);
        Actor* setRanged(const bool range);
        Actor* setReviving(const bool revive);
        Actor* setAddedStates(State** const value);

        Actor();
    };

}

#endif // ACTOR_H
