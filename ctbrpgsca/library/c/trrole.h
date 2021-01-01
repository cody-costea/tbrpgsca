/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef CROLE_H
#define CROLE_H

#include <malloc.h>

#include "tractor.h"

#pragma pack(1)

#define TR_IX signed char
#define TR_BYTE unsigned char
#define TR_BOOL unsigned int
#define TR_NR signed int

#define FLAG_REVIVE 1
#define FLAG_RANGE 2
#define DMG_TYPE_ATK 1
#define DMG_TYPE_DEF 2
#define DMG_TYPE_SPI 4
#define DMG_TYPE_WIS 8
#define DMG_TYPE_AGI 16
#define ELEMENT_FIRE 32
#define ELEMENT_WATER 64
#define ELEMENT_THUNDER 128
#define ELEMENT_EARTH 256
#define ELEMENT_PSYCHIC 512
#define ELEMENT_LIGHT 1024

#define TR_VEC_TYPE(Struct, DataType, CountType, EmptyValue, Alias) \
    typedef struct Struct { \
        CountType count; \
        DataType *data; \
    } Alias; \
    Alias Struct##_init(const TR_BOOL size) { \
        return (Alias) { \
            size, (DataType *)malloc(sizeof(DataType) * size) \
        }; \
    } \
    Alias Struct##_new(const TR_BOOL size) { \
        Alias Struct = Struct##_init(size); \
        DataType *data = Struct.data; \
        unsigned int i = 0; \
        for (i = 0; i < size; i += 1) { \
            data[i] = EmptyValue; \
        } \
        return Struct; \
    } \
    Alias *Struct##_alloc() { \
        return (Alias *)malloc(sizeof(Alias)); \
    } \
    Alias *Struct##_new_alloc(const TR_BOOL size) { \
        Alias *Struct = Struct##_alloc(); \
        *Struct = Struct##_new(size); \
        return Struct; \
    } \
    Alias Struct##_array(const DataType array[], const TR_BOOL size) { \
        Alias Struct = Struct##_init(size); \
        DataType *data = Struct.data; \
        unsigned int i = 0; \
        for (i = 0; i < size; i += 1) { \
            data[i] = array[i]; \
        } \
        return Struct; \
    } \
    Alias *Struct##_array_alloc(const DataType array[], const TR_BOOL size) { \
        Alias *Struct = Struct##_alloc(); \
        *Struct = Struct##_array(array, size); \
        return Struct; \
    } \
    Alias Struct##_copy(Alias *Struct) { \
        return Struct##_array(Struct->data, Struct->count); \
    } \
    Alias *Struct##_copy_alloc(Alias *Struct) { \
        Alias *Struct##_ptr = Struct##_alloc(); \
        *Struct##_ptr = Struct##_copy(Struct); \
        return Struct##_ptr; \
    } \
    TR_BOOL Struct##_resize(Alias *Struct, const TR_BOOL size) { \
        DataType *data = (DataType *)realloc(Struct->data, sizeof(DataType) * size); \
        if (data) { \
            Struct->data = data; \
            Struct->count = size; \
            return 1; \
        } else { \
            return 0; \
        } \
    } \
    TR_BOOL Struct##_add(Alias *Struct, const DataType new_value) { \
        TR_BOOL size = Struct->count; \
        if (size < 1) { \
            if (Struct##_resize(Struct, 1)) { \
                Struct->data[0] = new_value; \
            } else { \
                return 0; \
            } \
        } else { \
            TR_BOOL j = size - 1; \
            DataType *data = Struct->data; \
            if (data[j] == EmptyValue) { \
                do { \
                    size = j; \
                } while (--j > 0 && data[j] == EmptyValue); \
            } else if (!Struct##_resize(Struct, size * 2)) { \
                return 0; \
            } \
            data[size] = new_value; \
        } \
        return 1; \
    } \
    void Struct##_free(Alias *Struct) { \
        DataType* data = Struct->data; \
        if (data) { \
            Struct->count = 0; \
            Struct->data = NULL; \
            free(data); \
        } \
    } \
    void Struct##_free_alloc(Alias **Struct) { \
        Alias *ptr = *Struct; \
        if (ptr) { \
            *Struct = NULL; \
            Struct##_free(ptr); \
            free(ptr); \
        } \
    } \

TR_VEC_TYPE(tr_index_vec, TR_IX, TR_BYTE, -1, TrIndexVector)

typedef struct tr_index_map {
    TR_IX *keys, *values;
    TR_BYTE count;
} TrIndexMap;

#define TR_ROLE_DATA \
    char *name, *sprite; \
    TR_NR dmg_type: 12, c_hp: 12, c_mp: 10, c_sp: 10, m_hp: 12, m_mp: 10, m_sp: 10, flags: 12; \
    TrIndexVector states;
    //TR_BYTE revive: 1, range: 1;

#define TR_HAS_ALL_FLAGS(Flags, Flag) \
    ((Flags & Flag) == Flag)

#define TR_HAS_ANY_FLAG(Flags, Flag) \
    ((Flags & Flag) != Flag)

#define TR_IF_HAS_ALL_FLAGS(Flags, Flag, Conditions, Action) \
    if (Conditions TR_HAS_ALL_FLAGS(Flags, Flag)) Action;

#define TR_IF_HAS_ANY_FLAG(Flags, Flag, Conditions, Action) \
    if (Conditions TR_HAS_ANY_FLAG(Flags, Flag)) Action;

void tr_role_damage(char *ret, TrActor* user, TrActor* target, const TR_NR dmg, const TR_BOOL percent);

TR_BOOL tr_index_map_resize(TrIndexMap *tr_index_map, const TR_BOOL size);

TR_BOOL tr_index_map_add(TrIndexMap *tr_index_map, const TR_IX key, const TR_IX value, const TR_IX empty_key);

TrIndexMap tr_index_map_new(const TR_BOOL size, const TR_IX empty_key, const TR_IX empty_value);
TrIndexMap tr_index_map_array(const TR_IX keys[], const TR_IX values[], const TR_BOOL size);
TrIndexMap tr_index_map_copy(const TrIndexMap *tr_index_map);
TrIndexMap tr_index_map_init(const TR_BOOL size);

#endif // CROLE_H
