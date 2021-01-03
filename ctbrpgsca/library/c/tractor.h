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

#endif // CACTOR_H
