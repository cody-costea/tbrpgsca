/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef CROLE_H
#define CROLE_H

#include "trplay.h"

//#define TR_ROLE_DATA
    //char *name, *sprite; \
    //signed int dmg_type: 16, hp: 24, mp: 16, sp: 16, m_hp: 24, m_mp: 16, m_sp: 16;

typedef struct tr_role {
    struct tr_play;
    char *name, *sprite;
    //TR_BOOL ranged: 1, revives: 1;
    TR_NR dmg_type: 16, hp: 24, mp: 16, sp: 16, m_hp: 24, m_mp: 16, m_sp: 16;
} TrRole;

#endif // CROLE_H
