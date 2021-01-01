/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#pragma once

#ifndef CROLE_H
#define CROLE_H

#include <malloc.h>

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
        Alias obj; \
        obj.count = (CountType)size; \
        obj.data = size > 0 ? (DataType *)malloc(sizeof(DataType) * size) : 0; \
        return obj; \
    } \
    Alias Struct##_new(const TR_BOOL size) { \
        if (size < 1) { \
            Alias obj; \
            obj.count = 0; \
            obj.data = 0; \
            return obj; \
        } \
        Alias Struct##_obj = Struct##_init(size); \
        DataType *data = Struct##_obj.data; \
        TR_BOOL i = 0; \
        for (i = 0; i < size; i += 1) { \
            data[i] = EmptyValue; \
        } \
        return Struct##_obj; \
    } \
    Alias *Struct##_alloc() { \
        return (Alias *)malloc(sizeof(Alias)); \
    } \
    Alias *Struct##_new_alloc(const TR_BOOL size) { \
        Alias *Struct##_obj = Struct##_alloc(); \
        *Struct##_obj = Struct##_new(size); \
        return Struct##_obj; \
    } \
    Alias Struct##_array(const DataType array[], const TR_BOOL size) { \
        Alias Struct##_obj = Struct##_init(size); \
        DataType *data = Struct##_obj.data; \
        unsigned int i = 0; \
        for (i = 0; i < size; i += 1) { \
            data[i] = array[i]; \
        } \
        return Struct##_obj; \
    } \
    Alias *Struct##_array_alloc(const DataType array[], const TR_BOOL size) { \
        Alias *Struct##_obj = Struct##_alloc(); \
        *Struct##_obj = Struct##_array(array, size); \
        return Struct##_obj; \
    } \
    Alias Struct##_copy(Alias *Struct##_obj) { \
        return Struct##_array(Struct##_obj->data, Struct##_obj->count); \
    } \
    Alias *Struct##_copy_alloc(Alias *Struct##_obj) { \
        Alias *Struct##_ptr = Struct##_alloc(); \
        *Struct##_ptr = Struct##_copy(Struct##_obj); \
        return Struct##_ptr; \
    } \
    void Struct##_free(Alias *Struct##_obj) { \
        DataType* data = Struct##_obj->data; \
        if (data) { \
            Struct##_obj->count = 0; \
            Struct##_obj->data = 0; \
            free(data); \
        } \
    } \
    void Struct##_free_alloc(Alias **Struct##_obj) { \
        Alias *ptr = *Struct##_obj; \
        if (ptr) { \
            *Struct##_obj = 0; \
            Struct##_free(ptr); \
            free(ptr); \
        } \
    } \
    TR_BOOL Struct##_resize(Alias *Struct##_obj, const TR_BOOL size) { \
        const TR_BOOL count = Struct##_obj->count; \
        DataType* data; \
        if (count < 1) { \
            if (size > 0) { \
                data = (DataType *)malloc(sizeof(DataType) * size); \
            } else { \
                return 1; \
            } \
        } else { \
            if (size > 0) { \
                data = (DataType *)realloc(Struct##_obj->data, sizeof(DataType) * size); \
                if (data) { \
                    TR_BOOL i; \
                    for (i = count; i < size; i += 1) { \
                        data[i] = EmptyValue; \
                    } \
                    Struct##_obj->data = data; \
                    Struct##_obj->count = (CountType)size; \
                } else { \
                    return 0; \
                } \
            } else { \
                Struct##_free(Struct##_obj); \
                return 1; \
            } \
        } \
        Struct##_obj->count = (CountType)size; \
        Struct##_obj->data = data; \
        return 1; \
    } \
    TR_BOOL Struct##_add(Alias *Struct##_obj, const DataType new_value) { \
        TR_BOOL size = (TR_BOOL)Struct##_obj->count; \
        if (size < 1) { \
            if (Struct##_resize(Struct##_obj, 1)) { \
                Struct##_obj->data[0] = new_value; \
            } else { \
                return 0; \
            } \
        } else { \
            TR_BOOL j = size - 1; \
            DataType *data = Struct##_obj->data; \
            if (data[j] == EmptyValue) { \
                do { \
                    size = j; \
                } while (--j > 0 && data[j] == EmptyValue); \
            } else if (!Struct##_resize(Struct##_obj, size + 4)) { \
                return 0; \
            } \
            data[size] = new_value; \
        } \
        return 1; \
    }

typedef TR_NR tr_nr;
typedef TR_BOOL tr_bool;
typedef TR_BYTE tr_byte;
typedef TR_IX tr_ix;

typedef TR_NR TrNr;
typedef TR_BOOL TrBool;
typedef TR_BYTE TrByte;
typedef TR_IX TrIx;

typedef struct tr_actor TrActor;

//TR_VEC_TYPE(tr_index_vec, TR_IX, TR_BYTE, -1, TrIndexVector)

typedef struct tr_index_map {
    TR_IX *keys, *values;
    TR_BYTE count;
} TrIndexMap;
    //TR_BYTE revive: 1, range: 1;

#define TR_HAS_ALL_FLAGS(Flags, Flag) \
    ((Flags & Flag) == Flag)

#define TR_HAS_ANY_FLAG(Flags, Flag) \
    ((Flags & Flag) != Flag)

#define TR_IF_HAS_ALL_FLAGS(Flags, Flag, Conditions, Action) \
    if (Conditions TR_HAS_ALL_FLAGS(Flags, Flag)) Action;

#define TR_IF_HAS_ANY_FLAG(Flags, Flag, Conditions, Action) \
    if (Conditions TR_HAS_ANY_FLAG(Flags, Flag)) Action;

#define TR_ROLE_DATA \
    char *name, *sprite; \
    tr_nr dmg_type: 12, c_hp: 12, c_mp: 10, c_sp: 10, m_hp: 12, m_mp: 10, m_sp: 10, flags: 12;
    //TrIndexVector states;

void tr_role_damage(char *ret, TrActor* user, TrActor* target, const TR_NR dmg, const TR_BOOL percent);

/*TR_BOOL tr_index_map_resize(TrIndexMap *tr_index_map, const TR_BOOL size);

TR_BOOL tr_index_map_add(TrIndexMap *tr_index_map, const TR_IX key, const TR_IX value, const TR_IX empty_key);

TrIndexMap tr_index_map_new(const TR_BOOL size, const TR_IX empty_key, const TR_IX empty_value);
TrIndexMap tr_index_map_array(const TR_IX keys[], const TR_IX values[], const TR_BOOL size);
TrIndexMap tr_index_map_copy(const TrIndexMap *tr_index_map);
TrIndexMap tr_index_map_init(const TR_BOOL size);*/

#endif // CROLE_H
