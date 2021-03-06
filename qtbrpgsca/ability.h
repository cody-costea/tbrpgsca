/*
Copyright (C) AD 2018 Claudiu-Stefan Costea

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

#include <QString>
#include <state.h>

namespace qtbrpgsca
{
    struct State;

    struct Ability
    {
    public:
        QString name, anim, audio;
        int id, trg, hpc, mpc, spc, lvrq, atki, hpdmg, mpdmg, spdmg,
        dmgtype, element, qty, mqty, rqty, tqty, staten, rstaten;
        bool steal, absorb, range, restore;
        State** state,** rstate;

        Ability(int id, QString name, QString anim, QString audio, bool steal,
                bool range, int lvrq, int hpc, int mpc, int spc, int dmgtype,
                int atkp, int hpdmg, int mpdmg, int spdmg, int trg, int element,
                int mqty, int rqty, bool absorb, bool restore, State** state,
                int staten, State** rstate, int rstaten);

        Ability(int id, QString name, QString anim, QString audio, int hpdmg,
                int mpdmg, int spdmg, int trg, int element, bool restore,
                State** state, int staten, State** rstate, int rstaten);

        Ability();

        ~Ability();
    };

}
#endif // ABILITY_H
