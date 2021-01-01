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

typedef struct tr_costume {
    TR_ROLE_DATA
    TR_COSTUME_DATA
} TrCostume;

#endif // CCOSTUME_H
