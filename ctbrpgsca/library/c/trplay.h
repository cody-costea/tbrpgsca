/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef CPLAY_H
#define CPLAY_H

#define TR_IX char
#define TR_STR char *
#define TR_STC_BOOL(Name) int Name: 1
#define TR_BOOL unsigned int
#define TR_NR signed int

typedef struct tr_play {
    signed int flags: 16;
} TrPlay;

#endif // CPLAY_H
