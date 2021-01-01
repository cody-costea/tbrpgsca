/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef CROLE_H
#define CROLE_H

#include "trplay.h"

#pragma pack(4)

#define TR_IX signed char
#define TR_BYTE unsigned char
#define TR_BOOL unsigned int
#define TR_NR signed int

#define TR_ROLE_DATA \
    char *name, *sprite; \
    TR_NR dmg_type: 12, c_hp: 12, c_mp: 10, c_sp: 10, m_hp: 12, m_mp: 10, m_sp: 10; \
    TR_BYTE revive: 1, range: 1;

/*typedef struct tr_role {
    //struct tr_play;
    char *name, *sprite;
    TR_BOOL ranged: 1, revives: 1;
    TR_NR dmg_type: 16, hp: 24, mp: 16, sp: 16, m_hp: 24, m_mp: 16, m_sp: 16;
} TrRole;*/

#endif // CROLE_H
