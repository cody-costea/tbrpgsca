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

    struct State
    {
    public:
        QString name;
        bool inactive, automatic, confusion, reflect;
        int id, dur, mdur, hpm, mpm, spm, atkm, defm, wism, spim, agim, res,* resm;

        State(int id, QString name, bool inactive, bool automatic, bool confusion, int dur, int hpm,
              int mpm, int spm, int atkm, int defm, int wism, int spim, int agim, bool reflect);

        bool inflict();

        bool remove();

        ~State();

        static const int STATESN = 11;

        static State* GET_STATES();

        static const bool STATE_NONE[STATESN + 1];
        static const bool STATE_POISON[STATESN + 1];
        static const bool STATE_REGEN[STATESN + 1];
        static const bool STATE_DIZZINESS[STATESN + 1];
        static const bool STATE_MADNESS[STATESN + 1];
        static const bool STATE_CLARITY[STATESN + 1];
        static const bool STATE_WEAKNESS[STATESN + 1];
        static const bool STATE_VIGOUR[STATESN + 1];
        static const bool STATE_TPOISON[STATESN + 1];
        static const bool STATE_TREGEN[STATESN + 1];
        static const bool STATE_REVIVE[STATESN + 1];
        static const bool STATE_RVREGEN[STATESN + 1];
        static const bool STATE_BERSERK[STATESN + 1];
        static const bool STATE_CONFUSION[STATESN + 1];
        static const bool STATE_CONFCLARITY[STATESN + 1];
        static const bool STATE_CONFTREGEN[STATESN + 1];
        static const bool STATE_CURE[STATESN + 1];
        static const bool STATE_SLEEP[STATESN + 1];
        static const bool STATE_STUN[STATESN + 1];
        static const bool STATE_CONFSLEEP[STATESN + 1];
        static const bool STATE_DIZZYSTUN[STATESN + 1];
        static const bool STATE_REFLECT[STATESN + 1];

    };

}

#endif // STATE_H

