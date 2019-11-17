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
#ifndef COSTUME_H
#define COSTUME_H

#include "role.h"

namespace ctbrpgsca
{
    class Ability;

    class Costume : public Role
    {
    public:
        Costume(const int id, QString* const name, const QString* const sprite, const int hp, const int mp, const int sp,
                const int atk, const int def, const int spi, const int wis, const int agi, const int mInit, const bool range,
                QMap<int, int>* const res, Ability** const skills, State** const states, QMap<State*, int>* const stRes);
    };
}

#endif // COSTUME_H
