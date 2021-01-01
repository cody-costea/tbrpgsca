/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef CABILITY_H
#define CABILITY_H

#include "trrole.h"

#define TR_ABILITY_DATA \
    TR_NR lv_rq: 8, m_qty: 12, r_qty: 12; \
    char *sound;
    //TR_BYTE melee: 1, steal: 1, absorb: 1, missable: 1, trg_all: 1, trg_side: 1, trg_self: 1;

typedef struct tr_ability {
    TR_ROLE_DATA
    TR_ABILITY_DATA
} TrAbility;

#endif // CABILITY_H
