/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef CSCENE_H
#define CSCENE_H

#include "trstate.h"

#define TR_SCENE_DATA \
    TR_FLAGS_DATA \
    tr_nr last_ability: 8, current: 4, original: 4, surprise: 2, status: 2, m_init: 8; \
    TrIndexVector players;

typedef struct tr_scene {
    TR_SCENE_DATA
} TrScene;

void tr_scene_end_turn(TrScene *const scene, char *ret, TrActor *const actor);

void tr_scene_perform(TrScene *const scene, char *ret, TrActor *const user, TrActor *const target, const TrAbility *const ability, const tr_bool item);

void tr_scene_play_ai(TrScene *const scene, char *ret, TrActor *const player);

void tr_scene_check_status(TrScene *const scene, char *ret);
void tr_scene_escape(TrScene *const scene, char *ret);

tr_bool tr_scene_can_target(const TrScene *const scene, TrActor *const user, const TrAbility *const ability, TrActor *const target);

tr_nr tr_scene_get_ai_skill(const TrScene *const scene, TrActor *const user, TrIndexVector *const skills, const tr_nr index, const tr_bool restore);
TrActor* tr_scene_get_guardian(const TrScene *const scene, TrActor *const user, TrActor *const target, const TrAbility *const skill);

#endif // CSCENE_H
