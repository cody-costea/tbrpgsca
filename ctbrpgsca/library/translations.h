/*
Copyright (C) AD 2013-2021 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

#define LANG_CODE LANG_EN

#define LANG_EN 0
#define LANG_RO 1

#if LANG_CODE == LANG_RO

//TODO: Romanian translations

#else

//PC RACES:
#define TR_TXT_RACE_ELF "Elf"
#define TR_TXT_RACE_GNOME "Gnome"
#define TR_TXT_RACE_HALF_ORC "Half-Orc"
#define TR_TXT_RACE_HUMAN "Human"

//NPC RACES:
#define TR_TXT_RACE_GOBLIN "Goblin"
#define TR_TXT_RACE_LIZARD "Lizard"
#define TR_TXT_RACE_TROLL "Troll"
#define TR_TXT_RACE_OGRE "Ogre"

//PC JOBS:
#define TR_TXT_JOB_ALCHEMIST "Alchemist"
#define TR_TXT_JOB_BERSERKER "Berserker"
#define TR_TXT_JOB_CRUSADER "Crusader"
#define TR_TXT_JOB_DRAGOON "Dragoon"
#define TR_TXT_JOB_DRUID "Druid"
#define TR_TXT_JOB_GLADIATOR "Gladiator"
#define TR_TXT_JOB_HESYCHAST "Hesychast"
#define TR_TXT_JOB_KNIGHT "Paladin"
#define TR_TXT_JOB_PIRATE "Corsair"
#define TR_TXT_JOB_NINJA "Ninja"
#define TR_TXT_JOB_RANGER "Ranger"
#define TR_TXT_JOB_REAVER "Reaver"
#define TR_TXT_JOB_SHAMAN "Shaman"
#define TR_TXT_JOB_WARMAGE "Warmage"
#define TR_TXT_JOB_WIZARD "Wizard"

//NPC JOBS:
#define TR_TXT_JOB_BARD "Bard"
#define TR_TXT_JOB_CLERIC "Cleric"
#define TR_TXT_JOB_FAIRY "Fairy"
#define TR_TXT_JOB_SORCERESS "Sorceress"
#define TR_TXT_JOB_VALKYRIE "Valkyrie"

//PC NAMES:
#define TR_TXT_ACTOR_CODY "Cody"
#define TR_TXT_ACTOR_GEORGE "George"
#define TR_TXT_ACTOR_STEPHANIE "Stephanie"
#define TR_TXT_ACTOR_VICTORIA "Victoria"

//SKILLS:
#define TR_TXT_SKILL_ATTACK "Attack"
#define TR_TXT_SKILL_DEFEND "Defend"
#define TR_TXT_SKILL_HEAL "Heal"
#define TR_TXT_SKILL_RAISE "Raise"

//STATES:
#define TR_TXT_STATE_BERSERK "Berserk"
#define TR_TXT_STATE_CLARITY "Clarity"
#define TR_TXT_STATE_CONFUSION "Confusion"
#define TR_TXT_STATE_CONVERSION "Conversion"
#define TR_TXT_STATE_DIZZINESS "Dizziness"
#define TR_TXT_STATE_POISON "Poison"
#define TR_TXT_STATE_SLEEP "Sleep"
#define TR_TXT_STATE_STUN "Stun"
#define TR_TXT_STATE_REFLECT "Reflect"
#define TR_TXT_STATE_REGEN "Regen"
#define TR_TXT_STATE_VIGOUR "Vigour"
#define TR_TXT_STATE_WEAKNESS "Weakness"

//SCENE:
#define TR_TXT_SCENE_CAUSES ", %1 is affected by %2"
#define TR_TXT_SCENE_DEFEAT "The party has fallen!"
#define TR_TXT_SCENE_ESCAPE "The party has escaped!"
#define TR_TXT_SCENE_FAILED "The party attempted to escape, but failed."
#define TR_TXT_SCENE_FALLS ", %1 falls unconscious"
#define TR_TXT_SCENE_MISSES ", but misses %1"
#define TR_TXT_SCENE_PERFORMS "%1 performs %2"
#define TR_TXT_SCENE_STEALS ", obtaining %1 from %2"
#define TR_TXT_SCENE_SUFFERS ", %1 suffers "
#define TR_TXT_SCENE_REFLECTS ", reflected by %1"
#define TR_TXT_SCENE_RESISTS ", resisted by %1"
#define TR_TXT_SCENE_RISES ", but rises again"
#define TR_TXT_SCENE_VICTORY "The party has won!"

//STATS:
#define TR_TXT_STATS_HP "HP"
#define TR_TXT_STATS_MP "MP"
#define TR_TXT_STATS_SP "RP"

#endif

#endif // TRANSLATIONS_H
