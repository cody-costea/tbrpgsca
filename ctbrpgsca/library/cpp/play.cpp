/*
Copyright (C) AD 2013-2021 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "actor.hpp"
#include "state.hpp"

using namespace tbrpgsca;

void Play::FreeDemoMemory()
{
    auto& jobs = Play::_Jobs;
    for (auto job : qAsConst(jobs))
    {
        delete job;
    }
    jobs.clear();
    auto& races = Play::_Races;
    for (auto race : qAsConst(races))
    {
        delete race;
    }
    races.clear();
    auto& players = Play::_Players;
    for (auto actor : qAsConst(players))
    {
        delete actor;
    }
    players.clear();
    auto& abilitiesList = Play::_Abilities;
    for (auto abilities : qAsConst(abilitiesList))
    {
        for (auto ability : qAsConst(*abilities))
        {
            delete ability;
        }
        //abilities->clear();
        delete abilities;
    }
    abilitiesList.clear();
    auto& enemiesList = Play::_Enemies;
    for (auto enemies : qAsConst(enemiesList))
    {
        for (auto enemy : qAsConst(*enemies))
        {
            delete enemy;
        }
        //enemies->clear();
        delete enemies;
    }
    enemiesList.clear();
    auto& stateMasks = Play::_State_Masks;
    for (auto mask : qAsConst(stateMasks))
    {
        delete mask;
    }
    stateMasks.clear();
    auto& states = Play::_States;
    for (auto state : qAsConst(states))
    {
        delete state;
    }
    states.clear();
}


QVector<const State*>& Play::States()
{
    QVector<const State*>& states = Play::_States;
    if (states.size() == 0)
    {
        states.append(new State(1, TR_TXT_STATE_REGEN, "", false, 7,-1,0,0, -7,0,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                false, false, false, false, true, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new State(2, TR_TXT_STATE_POISON, "", false, 7,0,0,0, 7,0,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new State(3, TR_TXT_STATE_CLARITY, "", false, 7,-1,0,0, 0,-7,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new State(4, TR_TXT_STATE_DIZZINESS, "", false, 7,0,0,0, 0,7,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new State(5, TR_TXT_STATE_VIGOUR, "", false, 7,-1,0,0, 0,0,-7, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new State(6, TR_TXT_STATE_WEAKNESS, "", false, 7,0,0,0, 0,7,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new State(7, TR_TXT_STATE_REFLECT, "", false, 3,-1,0,ELEMENT_LIGHT, 0,0,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                false, true, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new State(8, TR_TXT_STATE_CONVERSION, "", false, 5,3,0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                                true, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new State(9, TR_TXT_STATE_CONFUSION, "", false, 5,3,0,0, 0,0,0, 0,0,0, -1,-1,-1,-1,-1, false, false, false, true,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new State(10, TR_TXT_STATE_BERSERK, "", false, 5,1,0,0, 0,0,0, 0,0,0, 5,-3,-3,-3,3, false, false, true, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new State(11, TR_TXT_STATE_SLEEP, "", false, 3,2,0,0, 0,0,0, 0,0,0, -1,-1,-1,-1,-1, true, false, false, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr));
        states.append(new State(12, TR_TXT_STATE_STUN, "", false, 1,1,0,0, 0,0,0, 0,0,0, -1,-1,-1,-1,-1, true, false, false, false,
                                false, false, false, false, false, nullptr, nullptr, nullptr, nullptr, nullptr));

    }
    return states;
}

QVector<QMap<const State*, int>*>& Play::StateMasks()
{
    QVector<QMap<const State*, int>*>& stateMasks = Play::_State_Masks;
    if (stateMasks.size() == 0)
    {
        QVector<const State*>& states = Play::States();
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[0], 0}}));
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[1], 0}}));
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[2], 0}}));
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[3], 0}}));
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[4], 0}}));
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[5], 0}}));
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[6], 0}}));
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[7], 0}}));
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[8], 0}}));
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[9], 0}}));
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[10], 0}}));
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[11], 0}}));
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[0], 0}, {states[2], 0}, {states[4], 0}}));
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[1], 0}, {states[3], 0}, {states[5], 0}}));
        stateMasks.append(new QMap<const State*, int>(std::map<const State*, int> {{states[7], 0}, {states[8], 0}, {states[9], 0}}));
    }
    return stateMasks;
}

QVector<QVector<const Ability*>*>& Play::Abilities()
{
    QVector<QVector<const Ability*>*>& abilities = Play::_Abilities;
    if (abilities.size() == 0)
    {
        QVector<QMap<const State*, int>*>& stateMasks = Play::StateMasks();
        QVector<const Ability*>* skillSet = new QVector<const Ability*>();
        skillSet->append(new Ability(1, TR_TXT_SKILL_ATTACK, "", "", false, false, false, true, 0, 0,0,0, DMG_TYPE_ATK, 10,0,-3, FLAG_TRG_ONE,0, 0,0, false, false, nullptr, stateMasks[10]));
        skillSet->append(new Ability(2, TR_TXT_SKILL_DEFEND, "", "", false, false, false, false, 0, 0,0,0, DMG_TYPE_DEF, 0,-1,-2, FLAG_TRG_SELF,0, 0,0, false, false, nullptr, nullptr));
        abilities.append(skillSet);
        skillSet = new QVector<const Ability*>();
        skillSet->append(new Ability(3, TR_TXT_SKILL_HEAL, "", "", false, true, false, false, 0, 0,3,0, DMG_TYPE_SPI, -13,0,0, FLAG_TRG_ONE,0, 0,0, false, false, nullptr, nullptr));
        skillSet->append(new Ability(4, TR_TXT_SKILL_RAISE, "", "", false, true, false, false, 0, 0,10,0, DMG_TYPE_SPI, -7,0,0, FLAG_TRG_ONE | FLAG_TRG_KO,0, 0,0, false, true, nullptr, nullptr));
        abilities.append(skillSet);
    }
    return abilities;
}

QVector<const Costume*>& Play::Races()
{
    QVector<const Costume*>& races = Play::_Races;
    if (races.size() == 0)
    {
        QVector<const Ability*>* abilities = Play::Abilities().at(0);
        races.append(new Costume(1, TR_TXT_RACE_ELF, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, false, nullptr, nullptr, nullptr));
        races.append(new Costume(2, TR_TXT_RACE_GNOME, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, false, nullptr, nullptr, nullptr));
        races.append(new Costume(3, TR_TXT_RACE_HUMAN, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, false, nullptr, nullptr, nullptr));
        races.append(new Costume(4, TR_TXT_RACE_HALF_ORC, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, false, nullptr, nullptr, nullptr));
        races.append(new Costume(5, TR_TXT_RACE_GOBLIN, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, false, nullptr, nullptr, nullptr));
        races.append(new Costume(6, TR_TXT_RACE_LIZARD, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, false, nullptr, nullptr, nullptr));
        races.append(new Costume(7, TR_TXT_RACE_OGRE, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, false, nullptr, nullptr, nullptr));
        races.append(new Costume(8, TR_TXT_RACE_TROLL, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                                 false, false, false, false, abilities, false, nullptr, nullptr, nullptr));
    }
    return races;
}

QVector<const Costume*>& Play::Jobs()
{
    QVector<const Costume*>& jobs = Play::_Jobs;
    if (jobs.size() == 0)
    {
        QVector<QVector<const Ability*>*>& abilities = Play::Abilities();
        jobs.append(new Costume(1, TR_TXT_JOB_ALCHEMIST, "Alchemist", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(2, TR_TXT_JOB_BERSERKER, "Berserker", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(3, TR_TXT_JOB_CRUSADER, "Crusader", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(4, TR_TXT_JOB_PIRATE, "Corsair", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(5, TR_TXT_JOB_DRAGOON, "Dragoon", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(6, TR_TXT_JOB_DRUID, "Druid", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(7, TR_TXT_JOB_HESYCHAST, "Hesychast", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(8, TR_TXT_JOB_KNIGHT, "Knight", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(9, TR_TXT_JOB_GLADIATOR, "Hero", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(10, TR_TXT_JOB_NINJA, "Ninja", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(11, TR_TXT_JOB_RANGER, "Ranger", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(12, TR_TXT_JOB_REAVER, "Reaver", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(13, TR_TXT_JOB_SHAMAN, "Shaman", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(14, TR_TXT_JOB_WARMAGE, "Warmage", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(15, TR_TXT_JOB_WIZARD, "Wizard", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(16, TR_TXT_JOB_CLERIC, "Santa", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(17, TR_TXT_JOB_VALKYRIE, "Valkyrie", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(18, TR_TXT_JOB_SORCERESS, "Sorceress", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(19, TR_TXT_JOB_BARD, "Bard", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
        jobs.append(new Costume(20, TR_TXT_JOB_FAIRY, "Fairy", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                                 false, false, false, false, abilities[1], false, nullptr, nullptr, nullptr));
    }
    return jobs;
}

QVector<QVector<Actor*>*>& Play::Enemies()
{
    QVector<QVector<Actor*>*>& enemies = Play::_Enemies;
    if (enemies.size() == 0)
    {
        QVector<const Costume*>& jobs = Play::Jobs();
        QVector<const Costume*>& races = Play::Races();
        QVector<Actor*>* enemy = new QVector<Actor*>();
        enemy->append(new Actor(5, TR_TXT_RACE_GOBLIN, "", (*races[4]), (*jobs[9]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
        enemy->append(new Actor(6, TR_TXT_RACE_LIZARD, "", (*races[5]), (*jobs[13]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
        enemy->append(new Actor(7, TR_TXT_RACE_TROLL, "", (*races[2]), (*jobs[12]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
        enemy->append(new Actor(8, TR_TXT_RACE_OGRE, "", (*races[6]), (*jobs[1]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
        enemies.append(enemy);
    }
    return enemies;
}

QVector<Actor*>& Play::Players()
{
    QVector<Actor*>& players = Play::_Players;
    if (players.size() == 0)
    {
        QVector<const Costume*>& jobs = Play::Jobs();
        QVector<const Costume*>& races = Play::Races();
        players.append(new Actor(1, TR_TXT_ACTOR_CODY, "", (*races[3]), (*jobs[7]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
        players.append(new Actor(2, TR_TXT_ACTOR_VICTORIA, "", (*races[0]), (*jobs[16]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
        players.append(new Actor(4, TR_TXT_ACTOR_STEPHANIE, "", (*races[2]), (*jobs[19]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
        players.append(new Actor(3, TR_TXT_ACTOR_GEORGE, "", (*races[1]), (*jobs[2]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr));
    }
    return players;
}

Play::Play(int const flags)
{
    this->_play_flags = flags;
}

Play::Play() : Play(0) {}

Play::~Play() {}
