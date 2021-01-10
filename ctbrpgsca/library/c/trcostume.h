/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef CCOSTUME_H
#define CCOSTUME_H

#include "trrole.h"

#define TR_COSTUME_DATA \
    tr_nr atk: 12, def: 12, spi: 12, wis: 12, agi: 12, block_type: 12;

#define FLAG_ENRAGED 4
#define FLAG_CONFUSE 8
#define FLAG_CONVERT 16
#define FLAG_COVERING 1024
#define FLAG_SHAPE_SHIFT 64
#define FLAG_INVINCIBLE 128
#define FLAG_COUNTER 2048
#define FLAG_REFLECT 32
#define FLAG_STUN 256
#define FLAG_KO 512

typedef struct tr_costume {
    TR_ROLE_DATA
    TR_COSTUME_DATA
} TrCostume;

void tr_costume_apply(const TrCostume *const costume, char *ret, TrScene *const scene, TrActor *const actor);
void tr_costume_refresh(const TrCostume *const costume, TrScene *const scene, TrActor *const actor, const tr_bool upd_states, const tr_bool remove);
void tr_costume_adopt(const TrCostume *const costume, TrScene *const scene, TrActor *const actor, const tr_bool upd_states, const tr_bool rmeove);

#endif // CCOSTUME_H
