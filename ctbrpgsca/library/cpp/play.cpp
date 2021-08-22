/*
Copyright (C) AD 2013-2021 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "actor.hpp"
#include "state.hpp"

using namespace tbrpgsca;

State _states[12]
{
    State(1, TR_TXT_STATE_REGEN, "", false, 7,-1,0,0, -7,0,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                            false, false, false, false, true, nullptr, false, nullptr, nullptr, nullptr),
    State(2, TR_TXT_STATE_POISON, "", false, 7,0,0,0, 7,0,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                            false, false, false, false, false, nullptr, false, nullptr, nullptr, nullptr),
    State(3, TR_TXT_STATE_CLARITY, "", false, 7,-1,0,0, 0,-7,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                            false, false, false, false, false, nullptr, false, nullptr, nullptr, nullptr),
    State(4, TR_TXT_STATE_DIZZINESS, "", false, 7,0,0,0, 0,7,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                            false, false, false, false, false, nullptr, false, nullptr, nullptr, nullptr),
    State(5, TR_TXT_STATE_VIGOUR, "", false, 7,-1,0,0, 0,0,-7, 0,0,0, 0,0,0,0,0, false, false, false, false,
                            false, false, false, false, false, nullptr, false, nullptr, nullptr, nullptr),
    State(6, TR_TXT_STATE_WEAKNESS, "", false, 7,0,0,0, 0,7,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                            false, false, false, false, false, nullptr, false, nullptr, nullptr, nullptr),
    State(7, TR_TXT_STATE_REFLECT, "", false, 3,-1,0,ELEMENT_LIGHT, 0,0,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                            false, true, false, false, false, nullptr, false, nullptr, nullptr, nullptr),
    State(8, TR_TXT_STATE_CONVERSION, "", false, 5,3,0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false, false, false,
                            true, false, false, false, false, nullptr, false, nullptr, nullptr, nullptr),
    State(9, TR_TXT_STATE_CONFUSION, "", false, 5,3,0,0, 0,0,0, 0,0,0, -1,-1,-1,-1,-1, false, false, false, true,
                            false, false, false, false, false, nullptr, false, nullptr, nullptr, nullptr),
    State(10, TR_TXT_STATE_BERSERK, "", false, 5,1,0,0, 0,0,0, 0,0,0, 5,-3,-3,-3,3, false, false, true, false,
                            false, false, false, false, false, nullptr, true, nullptr, nullptr, nullptr),
    State(11, TR_TXT_STATE_SLEEP, "", false, 3,2,0,0, 0,0,0, 0,0,0, -1,-1,-1,-1,-1, true, false, false, false,
                            false, false, false, false, false, nullptr, false, nullptr, nullptr, nullptr),
    State(12, TR_TXT_STATE_STUN, "", false, 1,1,0,0, 0,0,0, 0,0,0, -1,-1,-1,-1,-1, true, false, false, false,
                            false, false, false, false, false, nullptr, false, nullptr, nullptr, nullptr)

};
inline CmpsVct<const State, uint32_t, 12U> _States = _states;
inline QMap<CmpsPtr<const State>, int> _state_masks[15]
{
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[0], 0}}),
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[1], 0}}),
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[2], 0}}),
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[3], 0}}),
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[4], 0}}),
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[5], 0}}),
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[6], 0}}),
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[7], 0}}),
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[8], 0}}),
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[9], 0}}),
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[10], 0}}),
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[11], 0}}),
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[0], 0}, {&_States[2], 0}, {&_States[4], 0}}),
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[1], 0}, {&_States[3], 0}, {&_States[5], 0}}),
    QMap<CmpsPtr<const State>, int>(std::map<CmpsPtr<const State>, int> {{&_States[7], 0}, {&_States[8], 0}, {&_States[9], 0}})
};
inline CmpsVct<QMap<CmpsPtr<const State>, int>, uint32_t, 15U> _State_Masks(_state_masks);
inline const Ability _std_skills[2]
{
    Ability(3, TR_TXT_SKILL_HEAL, "", "", false, true, false, false, 0, 0,3,0, DMG_TYPE_SPI, -13,0,0, FLAG_TRG_ONE,0, 0,0, false, false, nullptr, nullptr),
    Ability(4, TR_TXT_SKILL_RAISE, "", "", false, true, false, false, 0, 0,10,0, DMG_TYPE_SPI, -7,0,0, FLAG_TRG_ONE | FLAG_TRG_KO,0, 0,0, false, true, nullptr, nullptr)
};
inline CmpsVct<const Ability> _abilities[1]
{
    CmpsVct<const Ability>(_std_skills)
};
inline CmpsVct<CmpsVct<const Ability>> _Abilities(_abilities);
inline const Costume _races[8]
{
    Costume(1, TR_TXT_RACE_ELF, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(2, TR_TXT_RACE_GNOME, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(3, TR_TXT_RACE_HUMAN, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(4, TR_TXT_RACE_HALF_ORC, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(5, TR_TXT_RACE_GOBLIN, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(6, TR_TXT_RACE_LIZARD, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(7, TR_TXT_RACE_OGRE, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(8, TR_TXT_RACE_TROLL, "", false, 0,0, 0,0,0, 35,10,10, 7,7,7,7,7, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr)
}, _jobs[20]
{
    Costume(1, TR_TXT_JOB_ALCHEMIST, "Alchemist", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(2, TR_TXT_JOB_BERSERKER, "Berserker", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(3, TR_TXT_JOB_CRUSADER, "Crusader", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(4, TR_TXT_JOB_PIRATE, "Corsair", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(5, TR_TXT_JOB_DRAGOON, "Dragoon", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(6, TR_TXT_JOB_DRUID, "Druid", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(7, TR_TXT_JOB_HESYCHAST, "Hesychast", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(8, TR_TXT_JOB_KNIGHT, "Knight", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(9, TR_TXT_JOB_GLADIATOR, "Hero", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(10, TR_TXT_JOB_NINJA, "Ninja", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(11, TR_TXT_JOB_RANGER, "Ranger", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(12, TR_TXT_JOB_REAVER, "Reaver", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(13, TR_TXT_JOB_SHAMAN, "Shaman", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(14, TR_TXT_JOB_WARMAGE, "Warmage", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(15, TR_TXT_JOB_WIZARD, "Wizard", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(16, TR_TXT_JOB_CLERIC, "Santa", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(17, TR_TXT_JOB_VALKYRIE, "Valkyrie", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(18, TR_TXT_JOB_SORCERESS, "Sorceress", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(19, TR_TXT_JOB_BARD, "Bard", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr),
    Costume(20, TR_TXT_JOB_FAIRY, "Fairy", false, 0,0, 0,0,0, 0,0,0, 0,0,0,0,0, false, false,
                             false, false, false, false, _Abilities, false, nullptr, nullptr, nullptr)
};
inline CmpsVct<const Costume, uint32_t, 8U> _Races(_races);
inline CmpsVct<const Costume, uint32_t, 20U> _Jobs(_jobs);
inline Actor _enemy[4]
{
    Actor(5, TR_TXT_RACE_GOBLIN, "", (_Races[4]), (_Jobs[9]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr),
    Actor(6, TR_TXT_RACE_LIZARD, "", (_Races[5]), (_Jobs[13]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr),
    Actor(7, TR_TXT_RACE_TROLL, "", (_Races[2]), (_Jobs[12]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr),
    Actor(8, TR_TXT_RACE_OGRE, "", (_Races[6]), (_Jobs[1]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr)
};
inline CmpsVct<Actor> _enemies[1]
{
    CmpsVct<Actor>(_enemy)
};
inline CmpsVct<CmpsVct<Actor>> _Enemies(_enemies);
inline Actor _players[4]
{
    Actor(1, TR_TXT_ACTOR_CODY, "", (_Races[3]), (_Jobs[7]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr),
    Actor(2, TR_TXT_ACTOR_VICTORIA, "", (_Races[0]), (_Jobs[16]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr),
    Actor(4, TR_TXT_ACTOR_STEPHANIE, "", (_Races[2]), (_Jobs[19]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr),
    Actor(3, TR_TXT_ACTOR_GEORGE, "", (_Races[1]), (_Jobs[2]), 1,9, 35,10,10, 7,7,7,7,7, nullptr, nullptr, nullptr)
};
inline CmpsVct<Actor, uint32_t, 4U> _Players(_players);
inline CmpsVct<const Ability> _Party_Items;

/*void Play::FreeDemoMemory()
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
}*/

CmpsVct<const State, uint32_t, 12U> Play::States()
{
    return _States;
}

CmpsVct<QMap<CmpsPtr<const State>, int>, uint32_t, 15U> Play::StateMasks()
{
    return _State_Masks;
}

CmpsVct<CmpsVct<const Ability>> Play::Abilities()
{
    return _Abilities;
}

CmpsVct<const Costume, uint32_t, 8U> Play::Races()
{
    return _Races;
}

CmpsVct<const Costume, uint32_t, 20U> Play::Jobs()
{
    return _Jobs;
}

CmpsVct<CmpsVct<Actor>> Play::Enemies()
{
    return _Enemies;
}

CmpsVct<Actor, uint32_t, 4U> Play::Players()
{
    return _Players;
}
