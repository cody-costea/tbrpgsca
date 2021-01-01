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
    TR_BYTE enrage: 1, confuse: 1, convert: 1, cover: 1, shapeshift: 1, invincible: 1, counter: 1, reflect: 1, active: 1, ko: 1; \
    TR_NR atk: 12, def: 12, spi: 12, wis: 12, agi: 12, block_type: 12;

typedef struct tr_costume {
    TR_ROLE_DATA
    TR_COSTUME_DATA
    //TR_NR dmg_type: 12, atk: 12, def: 12, spi: 12, wis: 12, agi: 12, c_hp: 12, c_mp: 10, c_sp: 10, m_hp: 12, m_mp: 10, m_sp: 10, block_type: 12;
    //TR_NR dmg_type: 16, hp_m: 16, mp_m: 15, sp_m: 15, atk: 14, def: 14, spi: 14, wis: 14, agi: 14, block_type: 16;
} TrCostume;

#endif // CCOSTUME_H
