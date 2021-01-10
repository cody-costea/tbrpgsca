/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef CACTOR_H
#define CACTOR_H

#include "trcostume.h"

#define TR_ACTOR_DATA \
    tr_nr side: 8, init: 8, c_lv: 8, m_lv: 8, c_xp: 30, m_xp: 30, ai_player: 1, random_ai: 1, new_items: 1, equip_dmg: 1; \
    TrIndexVector equipment;

typedef struct tr_actor {
    TR_ROLE_DATA
    TR_COSTUME_DATA
    //struct tr_costume stats;
    TR_ACTOR_DATA
} TrActor;

void tr_actor_recover(TrActor *const actor, char *ret, TrScene *const scene);
void tr_actor_apply_states(TrActor *const actor, char *ret, TrScene *const scene, const tr_bool consume);
tr_byte tr_actor_unequip_item(TrActor *const actor, TrScene *const scene, const TrCostume *item);
void tr_actor_refresh_costumes(TrActor *const actor, char *const ret, TrScene *const scene);
const TrCostume *tr_actor_unequip_pos(TrActor *const actor, TrScene *const scene, const tr_byte pos);
const TrCostume *tr_actor_equip_item(TrActor *const actor, TrScene *const scene, const tr_byte pos, const TrCostume *const item);
void tr_actor_switch_costume(TrActor *const actor, char *const ret, TrScene *const scene, const TrCostume *const oldCostume, const TrCostume *const newCostume);
void tr_actor_remove_states(TrActor *const actor, char *const ret, TrScene *const scene, const tr_bool remove);
void tr_actor_update_states(TrActor *const actor, const tr_bool remove, char *const ret, TrScene *const scene, const TrIndexVector states, const tr_bool includeWithDur);
void tr_actor_update_skills(TrActor *const actor, const tr_bool remove, const TrIndexVector skills);
void tr_actor_update_attributes(TrActor *const actor, const tr_bool remove, TrScene *const scene, const TrCostume *costume);
void tr_actor_update_resistance(TrActor *const actor, const tr_bool remove, const TrIndexMap elm_res, const TrIndexMap st_res);
void tr_actor_set_current_experience(TrActor *const actor, TrScene *const scene, const tr_nr xp);
void tr_actor_set_current_level(TrActor *const actor, TrScene *const scene, const tr_nr level);
void tr_actor_set_race(TrActor *const actor, TrScene *const scene, const TrCostume *const race);
void tr_actor_set_job(TrActor *const actor, TrScene *const scene, const TrCostume *const job);
void tr_actor_set_agility(TrActor *const actor, const tr_nr agi, TrScene *const scene);
void tr_actor_level_up(TrActor *const actor, TrScene *const scene);

void tr_actor_set_current_hp(TrActor *const actor, char *const ret, TrScene *const scene, const tr_nr hp, const tr_bool survive);
void tr_actor_set_current_mp(TrActor *const actor, const tr_nr mp);
void tr_actor_set_current_sp(TrActor *const actor, const tr_nr sp);
void tr_actor_set_maximum_hp(TrActor *const actor, const tr_nr hp);
void tr_actor_set_maximum_mp(TrActor *const actor, const tr_nr mp);
void tr_actor_set_maximum_sp(TrActor *const actor, const tr_nr sp);

#endif // CACTOR_H
