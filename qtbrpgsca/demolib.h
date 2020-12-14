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

    class DemoLib : public QObject
    {
        //Q_DECLARE_TR_FUNCTIONS(DemoLib)
        Q_OBJECT
        public:
            QVector<State*>& getStates();
            QVector<Actor*>& getPlayers();
            QVector<QList<Ability>*>& getAbilities();
            QVector<QMap<State*, int>*>& getStateMasks();
            QVector<QVector<Actor*>*>& getEnemies();
            QVector<Ability*>& getPartyItems();
            QVector<Costume*>& getRaces();
            QVector<Costume*>& getJobs();

            DemoLib();

            ~DemoLib();
        protected:
            QVector<Actor*> players;
            QVector<QVector<Actor*>*> enemies;
            QVector<QMap<State*, int>*> stateMasks;
            QVector<QList<Ability>*> abilities;
            QVector<Costume*> races, jobs;
            QVector<Ability*> pItems;
            QVector<State*> states;
    };

}

#endif // DEMOLIB_H
