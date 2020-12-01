/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef DEMOLIB_H
#define DEMOLIB_H

#include "actor.h"
#include "ability.h"

#include <QCoreApplication>

namespace tbrpgsca
{

    class DemoLib
    {
         Q_DECLARE_TR_FUNCTIONS(DemoLib)

        public:
            QVector<Actor*>& getPlayers();
            QVector<const State*>& getStates();
            QVector<QVector<const Ability*>*>& getAbilities();
            QVector<QMap<const State*, int>*>& getStateMasks();
            QVector<QVector<Actor*>*>& getEnemies();
            QVector<const Ability*>& getPartyItems();
            QVector<const Costume*>& getRaces();
            QVector<const Costume*>& getJobs();

            DemoLib();

            ~DemoLib();
        protected:
            QVector<Actor*> players;
            QVector<QVector<Actor*>*> enemies;
            QVector<QMap<const State*, int>*> stateMasks;
            QVector<QVector<const Ability*>*> abilities;
            QVector<const Costume*> races, jobs;
            QVector<const Ability*> pItems;
            QVector<const State*> states;
    };

}

#endif // DEMOLIB_H
