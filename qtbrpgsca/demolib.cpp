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
#include "ailment.h"
#include "scene.h"
#include "role.h"

using namespace tbrpgsca;

QVector<Ailment*>& DemoLib::getStates()
{
    QVector<Ailment*>& states = this->states;
    if (states.size() == 0)
    {
        states.append(new Ailment(1, tr("Regen"), "", false, 7,-1,0,0, -7,0,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                false, false, false, false, true, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new Ailment(2, tr("Poison"), "", false, 7,0,0,0, 7,0,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new Ailment(3, tr("Clarity"), "", false, 7,-1,0,0, 0,-7,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new Ailment(4, tr("Dizziness"), "", false, 7,0,0,0, 0,7,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new Ailment(5, tr("Vigour"), "", false, 7,-1,0,0, 0,0,-7, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new Ailment(6, tr("Weakness"), "", false, 7,0,0,0, 0,7,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new Ailment(7, tr("Reflect"), "", false, 3,-1,0,Role::Element::LIGHT, 0,0,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                false, true, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new Ailment(8, tr("Conversion"), "", false, 5,3,0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                true, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new Ailment(9, tr("Confusion"), "", false, 5,3,0,0, 0,0,0, 0,0,0, -1,-1,-1,-1,-1, false, false, false, true,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new Ailment(10, tr("Berserk"), "", false, 5,1,0,0, 0,0,0, 0,0,0, 5,-3,-3,-3,3, false, false, true, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new Ailment(11, tr("Sleep"), "", false, 3,2,0,0, 0,0,0, 0,0,0, -1,-1,-1,-1,-1, true, false, false, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new Ailment(12, tr("Stun"), "", false, 1,1,0,0, 0,0,0, 0,0,0, -1,-1,-1,-1,-1, true, false, false, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));

    }
    return states;
}

QVector<QMap<Ailment*, int>*>& DemoLib::getStateMasks()
{
    QVector<QMap<Ailment*, int>*>& stateMasks = this->stateMasks;
    if (stateMasks.size() == 0)
    {
        QVector<Ailment*>& states = this->getStates();
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[0], 0}}));
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[1], 0}}));
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[2], 0}}));
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[3], 0}}));
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[4], 0}}));
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[5], 0}}));
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[6], 0}}));
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[7], 0}}));
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[8], 0}}));
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[9], 0}}));
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[10], 0}}));
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[11], 0}}));
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[0], 0}, {states[2], 0}, {states[4], 0}}));
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[1], 0}, {states[3], 0}, {states[5], 0}}));
        stateMasks.append(new QMap<Ailment*, int>(std::map<Ailment*, int> {{states[7], 0}, {states[8], 0}, {states[9], 0}}));
    }
    return stateMasks;
}

QVector<QList<Ability>*>& DemoLib::getAbilities()
{
    auto& abilities = this->abilities;
    if (abilities.size() == 0)
    {
        //QVector<QMap<State*, int>*>& stateMasks = this->getStateMasks();
        auto skillSet = new QList<Ability>();
        skillSet->append(Ability(1, tr("Attack"), "", "", false, false, false, true, 0, 0,0,0, Role::Element::ATTACK,0, 10,0,-3, Ability::Attribute::TARGET_ONE,0, -1,-1, false, false, nullptr, NIL));
        skillSet->append(Ability(2, tr("Defend"), "", "", false, false, false, false, 0, 0,0,0, Role::Element::DEFENSE,0, 0,-1,-2, Ability::Attribute::TARGET_SELF,0, -1,-1, false, false, nullptr, nullptr));
        abilities.append(skillSet);
        skillSet = new QList<Ability>();
        skillSet->append(Ability(3, tr("Heal"), "", "", false, true, false, false, 0, 0,3,0, Role::Element::SPIRIT,0, -13,0,0, Ability::Attribute::TARGET_ONE,0, -1,-1, false, true, nullptr, nullptr));
        abilities.append(skillSet);
    }
    return abilities;
}

QVector<Costume*>& DemoLib::getRaces()
{
    QVector<Costume*>& races = this->races;
    if (races.size() == 0)
    {
        auto abilities = this->getAbilities().at(0);
        races.append(new Costume(1, tr("Elf"), "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, nullptr, nullptr, nullptr, nullptr));
        races.append(new Costume(2, tr("Gnome"), "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, nullptr, nullptr, nullptr, nullptr));
        races.append(new Costume(3, tr("Human"), "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, nullptr, nullptr, nullptr, nullptr));
        races.append(new Costume(4, tr("Half-Orc"), "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, nullptr, nullptr, nullptr, nullptr));
        races.append(new Costume(5, tr("Goblin"), "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, nullptr, nullptr, nullptr, nullptr));
        races.append(new Costume(6, tr("Lizard"), "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, nullptr, nullptr, nullptr, nullptr));
        races.append(new Costume(7, tr("Ogre"), "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, nullptr, nullptr, nullptr, nullptr));
        races.append(new Costume(8, tr("Troll"), "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, nullptr, nullptr, nullptr, nullptr));
    }
    return races;
}

QVector<Costume*>& DemoLib::getJobs()
{
    QVector<Costume*>& jobs = this->jobs;
    if (jobs.size() == 0)
    {
        auto& abilities = this->getAbilities();
        jobs.append(new Costume(1, tr("Alchemist"), "Alchemist", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(2, tr("Berserker"), "Berserker", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(3, tr("Crusader"), "Crusader", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(4, tr("Corsair"), "Corsair", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(5, tr("Dragoon"), "Dragoon", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(6, tr("Druid"), "Druid", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(7, tr("Hesychast"), "Hesychast", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(8, tr("Paladin"), "Knight", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(9, tr("Gladiator"), "Hero", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(10, tr("Ninja"), "Ninja", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(11, tr("Ranger"), "Ranger", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(12, tr("Reaver"), "Reaver", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(13, tr("Shaman"), "Shaman", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(14, tr("Warmage"), "Warmage", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(15, tr("Wizard"), "Wizard", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(16, tr("Cleric"), "Santa", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(17, tr("Valkyrie"), "Valkyrie", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(18, tr("Sorceress"), "Sorceress", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
        jobs.append(new Costume(19, tr("Bard"), "Bard", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], nullptr, nullptr, nullptr, nullptr));
    }
    return jobs;
}

QVector<QVector<Actor*>*>& DemoLib::getEnemies()
{
    QVector<QVector<Actor*>*>& enemies = this->enemies;
    if (enemies.size() == 0)
    {
        QVector<Costume*>& jobs = this->getJobs();
        QVector<Costume*>& races = this->getRaces();
        QVector<Actor*>* enemy = new QVector<Actor*>();
        /*enemy->append(new Actor(5, tr("Goblin"), "", (*races[4]), (*jobs[9]), 1,9, 1, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
        enemy->append(new Actor(6, tr("Lizard"), "", (*races[5]), (*jobs[13]), 1,9, 1, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
        enemy->append(new Actor(7, tr("Troll"), "", (*races[2]), (*jobs[12]), 1,9, 1, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
        enemy->append(new Actor(8, tr("Ogre"), "", (*races[6]), (*jobs[1]), 1,9, 1, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));*/
        enemies.append(enemy);
    }
    return enemies;
}

QVector<Actor*>& DemoLib::getPlayers()
{
    QVector<Actor*>& players = this->players;
    if (players.size() == 0)
    {
        QVector<Costume*>& jobs = this->getJobs();
        QVector<Costume*>& races = this->getRaces();
        /*players.append(new Actor(1, tr("Cody"), "", (*races[3]), (*jobs[7]), 1,9, 1, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
        players.append(new Actor(2, tr("Victoria"), "", (*races[0]), (*jobs[16]), 1,9, 1, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
        players.append(new Actor(4, tr("Stephanie"), "", (*races[2]), (*jobs[17]), 1,9, 1, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
        players.append(new Actor(3, tr("George"), "", (*races[1]), (*jobs[2]), 1,9, 1, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));*/
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
        /*for (auto ability : (*abilities))
        {
            delete ability;
        }*/
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
    for (auto mask : this->stateMasks)
    {
        delete mask;
    }
    for (auto state : this->states)
    {
        delete state;
    }
}
