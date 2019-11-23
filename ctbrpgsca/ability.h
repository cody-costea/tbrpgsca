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
#ifndef ABILITY_H
#define ABILITY_H

#include "role.h"

namespace ctbrpgsca
{

    class Ability : public Role
    {
        #define FLAG_MELEE 4
        #define FLAG_STEAL 8
        #define FLAG_ABSORB 16
        #define FLAG_TRG_ALL 32
        #define FLAG_TRG_SELF 64
        #define FLAG_TRG_FULL 128
    public:
        static int constexpr DmgTypeAtk = 256;
        static int constexpr DmgTypeDef = 512;
        static int constexpr DmgTypeSpi = 1024;
        static int constexpr DmgTypeWis = 2048;
        static int constexpr DmgTypeAgi = 4096;

        Ability();
    private:
        int lvRq, elm, mQty, rQty;
        QVector<State*>* rStates;

        friend class Costume;
        friend class Actor;
        friend class State;
        friend class Scene;
    };

}

#endif // ABILITY_H
