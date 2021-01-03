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
    tr_index_vec players;

typedef struct tr_scene {
    TR_SCENE_DATA
} TrScene;

#endif // CSCENE_H
