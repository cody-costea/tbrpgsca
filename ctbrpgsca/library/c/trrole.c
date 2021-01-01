/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#include "trrole.h"
#include "trcostume.h"
#include "tractor.h"

void tr_role_damage(char *ret, TrActor* user, TrActor* target, const TR_NR dmg, const TR_BOOL percent) {

}

/*TrIndexVector tr_index_vec_init(const TR_BOOL size) {
    return (TrIndexVector) {
        size, (TR_IX *)malloc(sizeof(TR_IX) * size)
    };
}

TrIndexVector tr_index_vec_new(const TR_BOOL size, const TR_IX empty_value) {
    TrIndexVector tr_index_vector = tr_index_vec_init(size);
    TR_IX *data = tr_index_vector.data;
    unsigned int i = 0;
    for (i = 0; i < size; i += 1) {
        data[i] = empty_value;
    }
}

TrIndexVector tr_index_vec_array(const TR_IX array[], const TR_BOOL size) {
    TrIndexVector tr_index_vector = tr_index_vec_init(size);
    TR_IX *data = tr_index_vector.data;
    unsigned int i = 0;
    for (i = 0; i < size; i += 1) {
        data[i] = array[i];
    }
}

TrIndexVector tr_index_vec_copy(const TrIndexVector *tr_index_vec) {
    return tr_index_vec_array(tr_index_vec->data, tr_index_vec->count);
}

TR_BOOL tr_index_vec_resize(TrIndexVector *tr_index_vec, const TR_BOOL size) {
    TR_IX *data = (TR_IX *)realloc(tr_index_vec->data, sizeof(TR_IX) * size);
    if (data) {
        tr_index_vec->data = data;
        tr_index_vec->count = size;
        return 1;
    } else {
        return 0;
    }
}

TR_BOOL tr_index_vec_add(TrIndexVector *tr_index_vec, const TR_IX new_value, const TR_IX empty_value) {
    TR_BOOL size = tr_index_vec->count;
    if (size < 1) {
        if (tr_index_vec_resize(tr_index_vec, 1)) {
            tr_index_vec->data[0] = new_value;
        } else {
            return 0;
        }
    } else {
        TR_BOOL j = size - 1;
        TR_IX *data = tr_index_vec->data;
        if (data[j] == empty_value) {
            do {
                size = j;
            } while (--j > 0 && data[j] == empty_value);
        } else if (!tr_index_vec_resize(tr_index_vec, size * 2)) {
            return 0;
        }
        data[size] = new_value;
    }
    return 1;
}
*/
