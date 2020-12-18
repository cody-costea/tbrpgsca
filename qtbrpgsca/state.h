/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef STATE_H
#define STATE_H

#include "ailment.h"

namespace tbrpgsca
{

    class State : public Ailment
    {
        Q_OBJECT
        QML_ELEMENT
        PROP_FIELD(State, CurrentDuration, currentDuration, int, inline, public, public, _cr_dur)
    public:
        ~State();
    protected:
        int _cr_dur;

        explicit State(const Ailment& ailment);
        explicit State(QObject* const parent = NIL);
        explicit State(const State& state);

        friend class Actor;
        friend class Costume;
        friend class SkillsModel;
        friend class ItemsModel;
        friend class Ailment;
        friend class Scene;
    };

}

//Q_DECLARE_METATYPE(tbrpgta::State)
Q_DECLARE_METATYPE(tbrpgsca::State*)
Q_DECLARE_INTERFACE(tbrpgsca::State, "com.codycostea.tbrpgta.State")

#endif // STATE_H
