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
#ifndef STATE_H
#define STATE_H

#include "costume.h"


namespace ctbrpgsca
{

    class State : public Costume
    {
        #define FLAG_STUN 64;
    public:
        static QString CausesTxt;

        State();
    };

}

#endif // STATE_H
