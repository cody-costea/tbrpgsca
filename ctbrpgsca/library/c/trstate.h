/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef CAILMENT_H
#define CAILMENT_H

#include "trability.h"

#define TR_STATE_DATA \
    tr_nr dur: 16, res: 16;

typedef struct tr_state {
    struct tr_costume mask;
    TR_STATE_DATA
} TrState;

void tr_state_alter(const TrCostume *const costume, char *ret, TrActor *const actor, const tr_bool consume);
void tr_state_inflict(const TrCostume *const costume, char *ret, TrActor *const user, TrActor *const target, const tr_nr dur, const tr_bool always);
tr_bool tr_state_disable(const TrCostume *const costume, TrActor *const actor, const tr_nr dur, const tr_bool remove);

#endif // CAILMENT_H
