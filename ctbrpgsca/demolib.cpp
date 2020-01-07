/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "actor.h"
#include "ability.h"
#include "costume.h"
#include "demolib.h"
#include "state.h"
#include "scene.h"
#include "role.h"

using namespace tbrpgsca;

QVector<State*>& DemoLib::getStates()
{
    QVector<State*>& states = this->states;
    if (states.size() == 0)
    {

    }
    return states;
}

QVector<QVector<Ability*>*>& DemoLib::getAbilities()
{
    QVector<QVector<Ability*>*>& abilities = this->abilities;
    if (abilities.size() == 0)
    {
        QVector<State*>& states = this->getStates();

    }
    return abilities;
}

QVector<Costume*>& DemoLib::getRaces()
{
    QVector<Costume*>& races = this->races;
    if (races.size() == 0)
    {
        QVector<Ability*>* abilities = this->getAbilities().at(0);
        races.append(new Costume(1, tr("Human"), "", false, 0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, nullptr, nullptr, nullptr, nullptr));
    }
    return races;
}

QVector<Costume*>& DemoLib::getJobs()
{
    QVector<Costume*>& jobs = this->jobs;
    if (jobs.size() == 0)
    {
        QVector<QVector<Ability*>*>& abilities = this->getAbilities();
        jobs.append(new Costume(1, tr("Hero"), "hero", false, 0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
    }
    return jobs;
}

QVector<Actor*>& DemoLib::getPlayers()
{
    QVector<Actor*>& players = this->players;
    if (players.size() == 0)
    {
        QVector<Costume*>& jobs = this->getJobs();
        QVector<Costume*>& races = this->getRaces();

        players.append(new Actor(1, tr("Cody"), "", (*races[0]), (*jobs[0]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
    }
    return players;
}


DemoLib::DemoLib()
{
    Scene::EscapeTxt = tr("The party has escaped!");
}


DemoLib::~DemoLib()
{
    for (auto job : this->jobs)
    {
        delete job;
    }
    for (auto race : this->races)
    {
        delete race;
    }
    for (auto actor : this->players)
    {
        delete actor;
    }
    for (auto abilities : this->abilities)
    {
        for (auto ability : (*abilities))
        {
            delete ability;
        }
        delete abilities;
    }
    for (auto enemies : this->enemies)
    {
        for (auto enemy : *enemies)
        {
            delete enemy;
        }
        delete enemies;
    }
    for (auto state : this->states)
    {
        delete state;
    }
}
