/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef CAILMENT_H
#define CAILMENT_H

#include "trcostume.h"

#define TR_STATE_DATA \
    TR_NR dur: 16, res: 16;

typedef struct tr_state {
    struct tr_costume mask;
    TR_STATE_DATA
} TrState;

#endif // CAILMENT_H
