// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
/*
    Copyright (C) 2023-2024  jgabaut

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef KLS_PIT_H_
#define KLS_PIT_H_
#include "koliseo.h"
#define LIST_T Koliseo
#define LIST_NAME KlsList
#include "list.h"


#ifndef KOLISEO_HAS_LOCATE
void KLS_PIT_OOM_default_handler__(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, ptrdiff_t count);
#else
void KLS_PIT_OOM_default_handler_dbg__(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, ptrdiff_t count, Koliseo_Loc loc);
#endif

#ifndef KOLISEO_HAS_LOCATE
#define KLS_PIT_ERR_HANDLERS (KLS_Err_Handlers) { \
    .OOM_handler = &KLS_PIT_OOM_default_handler__, \
    .PTRDIFF_MAX_handler = &KLS_PTRDIFF_MAX_default_handler__, \
}
#else
#define KLS_PIT_ERR_HANDLERS (KLS_Err_Handlers) { \
    .OOM_handler = &KLS_PIT_OOM_default_handler_dbg__, \
    .PTRDIFF_MAX_handler = &KLS_PTRDIFF_MAX_default_handler_dbg__, \
}
#endif // KOLISEO_HAS_LOCATE

#define KLS_PIT_CONF (KLS_Conf) { \
    .err_handlers =  KLS_PIT_ERR_HANDLERS, \
}

typedef struct KLS_Pit {
    KlsList list;
    ptrdiff_t kls_size;
} KLS_Pit;

#define KLS_PIT_NULL (KLS_Pit){0}

KLS_Pit kls_new_pit(ptrdiff_t size);
char* kls_pit_push_zero(KLS_Pit* pit, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void kls_pit_free(KLS_Pit* p);

#define KLS_PIT_PUSH_ARR(pit, type, count) (type*)kls_pit_push_zero((pit), sizeof(type), _Alignof(type), (count))

#define KLS_PIT_PUSH(pit, type) KLS_PIT_PUSH_ARR((pit), type, 1)

#ifdef KLS_PIT_IMPLEMENTATION

#ifndef KOLISEO_HAS_LOCATE
void KLS_PIT_OOM_default_handler__(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, ptrdiff_t count) {
#else
void KLS_PIT_OOM_default_handler__(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, ptrdiff_t count, Koliseo_Loc loc) {
#endif
    //fprintf(stderr, "%s():    OOM!\n", __func__);
    (void)0;
}

KLS_Pit kls_new_pit(ptrdiff_t size) {
    if (size < sizeof(Koliseo)) {
        size = KLS_DEFAULT_SIZE;
    }
    KLS_Pit res = (KLS_Pit) {
        .list = KlsList_nullList(),
        .kls_size = size,
    };
    Koliseo* head = kls_new_conf(res.kls_size, KLS_PIT_CONF);
    if (!head) {
        fprintf(stderr, "%s():    Failed creation of head Koliseo\n", __func__);
        return KLS_PIT_NULL;
    }
    res.list = KlsList_cons_gl(head, res.list);
    return res;
}

static inline bool kls_pit_grow(KLS_Pit* pit) {
    if (!pit) return false;
    Koliseo* new_kls = kls_new_conf(pit->kls_size, KLS_PIT_CONF);
    if (!new_kls) {
        fprintf(stderr, "%s():    Failed creation of new_kls\n", __func__);
        return false;
    }
    pit->list = KlsList_cons_gl(new_kls, pit->list);
    return true;
}

char* kls_pit_push_zero(KLS_Pit* pit, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count) {
    Koliseo* head = KlsList_head(pit->list);
    if (!head) {
        fprintf(stderr, "%s():    Failed push to empty KLS_Pit\n", __func__);
        return NULL;
    }
    char* res = NULL;
    res = kls_push_zero(head, size, align, count); // We have a custom OOM handler
    if (!res) {
        // fprintf(stderr, "%s():    Failed push to head Koliseo, creation of new_kls\n", __func__);
        if (!kls_pit_grow(pit)) {
            fprintf(stderr, "%s():    Failed growing KLS_Pit\n", __func__);
            return NULL;
        }
        return kls_pit_push_zero(pit, size, align, count);
    }
}

void kls_pit_free(KLS_Pit* p) {
    if (!p) return;
    KlsList_free_gl(p->list);
}
#endif // KLS_PIT_IMPLEMENTATION
#endif // KLS_PIT_H_
