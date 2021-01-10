/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#pragma once

#ifndef CROLE_H
#define CROLE_H

#include <stdlib.h>
#if ((defined(__GNUC__) || (defined(__clang__)) && !defined(_MSC_VER)))
    #include "mm_malloc.h"
#else
    #include "malloc.h"
#endif

#define MULTI_RND 1
#define DMG_RND 3

//#pragma pack(1)

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
#define DEFAULT_RES 3

#define TR_VEC_TYPE(Struct, DataType, CountType, EmptyValue, IncSize, Alias) \
    typedef struct Struct { \
        CountType count; \
        DataType *data; \
    } Alias; \
    static Alias Struct##_init(const TR_BOOL size) { \
        Alias obj; \
        obj.count = (CountType)size; \
        obj.data = size > 0 ? (DataType *)malloc(sizeof(DataType) * size) : 0; \
        return obj; \
    } \
    static Alias Struct##_new(const TR_BOOL size) { \
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
    static Alias *Struct##_alloc() { \
        return (Alias *)malloc(sizeof(Alias)); \
    } \
    static Alias *Struct##_new_alloc(const TR_BOOL size) { \
        Alias *Struct##_obj = Struct##_alloc(); \
        *Struct##_obj = Struct##_new(size); \
        return Struct##_obj; \
    } \
    static Alias Struct##_array(const DataType array[], const TR_BOOL size) { \
        Alias Struct##_obj = Struct##_init(size); \
        DataType *data = Struct##_obj.data; \
        TR_BOOL i; \
        for (i = 0; i < size; i += 1) { \
            data[i] = array[i]; \
        } \
        return Struct##_obj; \
    } \
    static Alias *Struct##_array_alloc(const DataType array[], const TR_BOOL size) { \
        Alias *Struct##_obj = Struct##_alloc(); \
        *Struct##_obj = Struct##_array(array, size); \
        return Struct##_obj; \
    } \
    static Alias Struct##_copy(Alias *Struct##_obj) { \
        return Struct##_array(Struct##_obj->data, Struct##_obj->count); \
    } \
    static Alias *Struct##_copy_alloc(Alias *Struct##_obj) { \
        Alias *Struct##_ptr = Struct##_alloc(); \
        *Struct##_ptr = Struct##_copy(Struct##_obj); \
        return Struct##_ptr; \
    } \
    static void Struct##_free(Alias *Struct##_obj) { \
        DataType* data = Struct##_obj->data; \
        if (data) { \
            Struct##_obj->count = 0; \
            Struct##_obj->data = 0; \
            free(data); \
        } \
    } \
    static void Struct##_free_alloc(Alias **Struct##_obj) { \
        Alias *ptr = *Struct##_obj; \
        if (ptr) { \
            *Struct##_obj = 0; \
            Struct##_free(ptr); \
            free(ptr); \
        } \
    } \
    static TR_BOOL Struct##_resize(Alias *Struct##_obj, const TR_BOOL size) { \
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
    static TR_BOOL Struct##_add(Alias *Struct##_obj, const DataType new_value) { \
        TR_BOOL size = (TR_BOOL)Struct##_obj->count; \
        if (size < 1) { \
            if (Struct##_resize(Struct##_obj, IncSize > 0 ? IncSize : 1)) { \
                Struct##_obj->data[0] = new_value; \
            } else { \
                return 0; \
            } \
        } else { \
            TR_BOOL j = size - 1; \
            DataType *data = Struct##_obj->data; \
            if (data[j] == EmptyValue) { \
                TR_BOOL k = j; \
                while (--k > 0 && data[k] == EmptyValue) { \
                    j = k; \
                } \
            } else if (!Struct##_resize(Struct##_obj, IncSize > 0 ? size + IncSize : size * 2)) { \
                return 0; \
            } \
            data[j] = new_value; \
        } \
        return 1; \
    } \
    static TR_BOOL Struct##_add_alloc(Alias **Struct##_obj, const DataType new_value) { \
        Alias* ptr = *Struct##_obj; \
        if  (!ptr) { \
            ptr = Struct##_new_alloc(IncSize > 0 ? IncSize : 1); \
        } \
        return Struct##_add(ptr, new_value); \
    } \
    static DataType Struct##_value_at(const Alias *Struct##_obj, const TR_BOOL index) { \
        DataType *values = Struct##_obj->data; \
        if (values && index < Struct##_obj->count) { \
            return values[index]; \
        } \
        return EmptyValue; \
    } \
    static void Struct##_clear(const Alias *Struct##_obj, TR_BOOL index) { \
        DataType *values = Struct##_obj->data; \
        const TR_BOOL size = Struct##_obj->count; \
        if (values) { \
            for (; index < size; index += 1) { \
                values[index] = EmptyValue; \
            } \
        } \
    }

#define TR_MAP_TYPE(Struct, KeyType, ValueType, CountType, EmptyKey, EmptyValue, IncSize, Alias) \
    typedef struct Struct { \
        CountType count; \
        ValueType *values; \
        KeyType *keys; \
    } Alias; \
    static Alias Struct##_init(const TR_BOOL size) { \
        Alias obj; \
        obj.count = (CountType)size; \
        obj.values = size > 0 ? (ValueType *)malloc(sizeof(ValueType) * size) : 0; \
        obj.keys = size > 0 ? (KeyType *)malloc(sizeof(KeyType) * size) : 0; \
        return obj; \
    } \
    static Alias Struct##_new(const TR_BOOL size) { \
        if (size < 1) { \
            Alias obj; \
            obj.count = 0; \
            obj.values = 0; \
            obj.keys = 0; \
            return obj; \
        } \
        Alias Struct##_obj = Struct##_init(size); \
        KeyType *values = Struct##_obj.values; \
        KeyType *keys = Struct##_obj.keys; \
        TR_BOOL i = 0; \
        for (i = 0; i < size; i += 1) { \
            values[i] = EmptyValue; \
            keys[i] = EmptyKey; \
        } \
        return Struct##_obj; \
    } \
    static Alias *Struct##_alloc() { \
        return (Alias *)malloc(sizeof(Alias)); \
    } \
    static Alias *Struct##_new_alloc(const TR_BOOL size) { \
        Alias *Struct##_obj = Struct##_alloc(); \
        *Struct##_obj = Struct##_new(size); \
        return Struct##_obj; \
    } \
    static Alias Struct##_array(const KeyType key_array[], const ValueType value_array[], const TR_BOOL size) { \
        Alias Struct##_obj = Struct##_init(size); \
        KeyType *values = Struct##_obj.values; \
        KeyType *keys = Struct##_obj.keys; \
        TR_BOOL i; \
        for (i = 0; i < size; i += 1) { \
            values[i] = value_array[i]; \
            keys[i] = key_array[i]; \
        } \
        return Struct##_obj; \
    } \
    static Alias *Struct##_array_alloc(const KeyType key_array[], const ValueType value_array[], const TR_BOOL size) { \
        Alias *Struct##_obj = Struct##_alloc(); \
        *Struct##_obj = Struct##_array(key_array, value_array, size); \
        return Struct##_obj; \
    } \
    static Alias Struct##_copy(Alias *Struct##_obj) { \
        return Struct##_array(Struct##_obj->keys, Struct##_obj->values, Struct##_obj->count); \
    } \
    static Alias *Struct##_copy_alloc(Alias *Struct##_obj) { \
        Alias *Struct##_ptr = Struct##_alloc(); \
        *Struct##_ptr = Struct##_copy(Struct##_obj); \
        return Struct##_ptr; \
    } \
    static void Struct##_free(Alias *Struct##_obj) { \
        KeyType* keys = Struct##_obj->keys; \
        if (keys) { \
            ValueType* values = Struct##_obj->values; \
            Struct##_obj->count = 0; \
            Struct##_obj->values = 0; \
            Struct##_obj->keys = 0; \
            free(values); \
            free(keys); \
        } \
    } \
    static void Struct##_free_alloc(Alias **Struct##_obj) { \
        Alias *ptr = *Struct##_obj; \
        if (ptr) { \
            *Struct##_obj = 0; \
            Struct##_free(ptr); \
            free(ptr); \
        } \
    } \
    static TR_BOOL Struct##_resize(Alias *Struct##_obj, const TR_BOOL size) { \
        const TR_BOOL count = Struct##_obj->count; \
        ValueType* values; \
        KeyType* keys; \
        if (count < 1) { \
            if (size > 0) { \
                values = (ValueType *)malloc(sizeof(ValueType) * size); \
                keys = (KeyType *)malloc(sizeof(KeyType) * size); \
            } else { \
                return 1; \
            } \
        } else { \
            if (size > 0) { \
                values = (ValueType *)realloc(Struct##_obj->values, sizeof(ValueType) * size); \
                keys = (KeyType *)realloc(Struct##_obj->keys, sizeof(KeyType) * size); \
                if (keys && values) { \
                    TR_BOOL i; \
                    for (i = count; i < size; i += 1) { \
                        values[i] = EmptyValue; \
                        keys[i] = EmptyKey; \
                    } \
                    Struct##_obj->keys = keys; \
                    Struct##_obj->values = values; \
                    Struct##_obj->count = (CountType)size; \
                } else { \
                    if (values) { \
                        free(values);\
                    } \
                    if (keys) { \
                        free(keys);\
                    } \
                    return 0; \
                } \
            } else { \
                Struct##_free(Struct##_obj); \
                return 1; \
            } \
        } \
        Struct##_obj->count = (CountType)size; \
        Struct##_obj->values = values; \
        Struct##_obj->keys = keys; \
        return 1; \
    } \
    static TR_BOOL Struct##_put(Alias *Struct##_obj, const KeyType new_key, const ValueType new_value) { \
        TR_BOOL size = (TR_BOOL)Struct##_obj->count; \
        if (size < 1) { \
            if (Struct##_resize(Struct##_obj, IncSize > 0 ? IncSize : 1)) { \
                Struct##_obj->values[0] = new_value; \
                Struct##_obj->keys[0] = new_key; \
            } else { \
                return 0; \
            } \
        } else { \
            TR_BOOL i; \
            KeyType *keys = Struct##_obj->keys; \
            ValueType *values = Struct##_obj->values; { \
            for (i = 0; i < size; i += 1) { \
                const KeyType key = keys[i]; \
                if (key == new_key || key == EmptyKey) { \
                    values[i] = new_value; \
                    return 1; \
                } \
            } } \
            if (Struct##_resize(Struct##_obj, IncSize > 0 ? size + IncSize : size * 2)) { \
                values[size] = new_value; \
                keys[size] = new_key; \
            } else { \
                return 0; \
            } \
        } \
        return 1; \
    } \
    static TR_BOOL Struct##_put_alloc(Alias **Struct##_obj, const KeyType new_key, const ValueType new_value) { \
        Alias* ptr = *Struct##_obj; \
        if  (!ptr) { \
            ptr = Struct##_new_alloc(IncSize > 0 ? IncSize : 1); \
        } \
        return Struct##_put(ptr, new_key, new_value); \
    } \
    static ValueType Struct##_value_at(const Alias *Struct##_obj, const TR_BOOL index) { \
        ValueType *values = Struct##_obj->values; \
        if (values && index < Struct##_obj->count) { \
            return values[index]; \
        } \
        return EmptyValue; \
    } \
    static ValueType Struct##_key_at(const Alias *Struct##_obj, const TR_BOOL index) { \
        KeyType *keys = Struct##_obj->keys; \
        if (keys && index < Struct##_obj->count) { \
            return keys[index]; \
        } \
        return EmptyKey; \
    } \
    static ValueType Struct##_get(const Alias *Struct##_obj, const KeyType key) { \
        KeyType *keys = Struct##_obj->keys; \
        if (keys) { \
            TR_BOOL size = (TR_BOOL)Struct##_obj->count; \
            if (size > 0) { \
                TR_BOOL i; \
                for (i = 0; i < size; i += 1) { \
                    const KeyType key = keys[i]; \
                    if (key == key) { \
                        return Struct##_obj->values[i]; \
                    } \
                    if (key == EmptyKey) { \
                        return EmptyValue; \
                    } \
                } \
            } \
        } \
        return EmptyValue; \
    } \
    static void Struct##_clear(const Alias *Struct##_obj, TR_BOOL index) { \
        const TR_BOOL size = Struct##_obj->count; \
        ValueType *values = Struct##_obj->values; \
        KeyType *keys = Struct##_obj->keys; \
        if (values) { \
            for (; index < size; index += 1) { \
                values[index] = EmptyValue; \
                keys[index] = EmptyKey; \
            } \
        } \
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
typedef struct tr_scene TrScene;

TR_VEC_TYPE(tr_index_vec, TR_IX, TR_BYTE, -1, 4, TrIndexVector)
TR_MAP_TYPE(tr_index_map, TR_IX, TR_IX, TR_BYTE, -1, -1, 4, TrIndexMap)

//TR_BYTE revive: 1, range: 1;

#define TR_HAS_ALL_FLAGS(Flags, Flag) \
    ((Flags & Flag) == Flag)

#define TR_HAS_ANY_FLAG(Flags, Flag) \
    ((Flags & Flag) != Flag)

#define TR_IF_HAS_ALL_FLAGS(Flags, Flag, Conditions, Action) \
    if (Conditions TR_HAS_ALL_FLAGS(Flags, Flag)) Action;

#define TR_IF_HAS_ANY_FLAG(Flags, Flag, Conditions, Action) \
    if (Conditions TR_HAS_ANY_FLAG(Flags, Flag)) Action;

#define TR_FLAGS_DATA \
    tr_nr flags: 12;

#define TR_ROLE_DATA \
    char *name, *sprite; \
    TR_FLAGS_DATA \
    tr_nr dmg_type: 12, c_hp: 12, c_mp: 10, c_sp: 10, m_hp: 12, m_mp: 10, m_sp: 10; \
    TrIndexVector *states; \
    TrIndexMap *res;

void tr_role_damage(char *ret, TrScene *const Scene, TrActor *user, TrActor *target, const TR_NR dmg_type, TR_NR hp_dmg, TR_NR mp_dmg, TR_NR sp_dmg, const TR_NR rnd_dmg, const TR_BOOL percent);

#endif // CROLE_H
