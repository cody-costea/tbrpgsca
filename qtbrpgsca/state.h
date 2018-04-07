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

#ifndef STATE_H
#define STATE_H

#include <QString>

namespace qtbrpgsca
{
    class Actor;

    struct State
    {
    public:
        QString name;
        bool inactive, automatic, confusion, reflect;
        int id, mdur, hpm, mpm, spm, atkm, defm, wism, spim, agim, res,* resm;

        State(int id, QString name, bool inactive, bool automatic, bool confusion, int dur, int hpm,
              int mpm, int spm, int atkm, int defm, int wism, int spim, int agim, bool reflect);

        bool inflict(Actor& actor);

        bool remove(Actor& actor);

        ~State();

    };

}

#endif // STATE_H

