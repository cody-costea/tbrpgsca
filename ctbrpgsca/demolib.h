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
            QVector<QVector<Ability*>*>& getAbilities();
            QVector<QVector<Actor*>*>& getEnemies();
            QVector<Ability*>& getPartyItems();
            QVector<Costume*>& getRaces();
            QVector<Costume*>& getJobs();
            QVector<State*>& getStates();

            DemoLib();

            ~DemoLib();
        protected:
            QVector<Actor*> players;
            QVector<QVector<Actor*>*> enemies;
            QVector<QVector<Ability*>*> abilities;
            QVector<Costume*> races, jobs;
            QVector<Ability*> pItems;
            QVector<State*> states;
    };

}

#endif // DEMOLIB_H
