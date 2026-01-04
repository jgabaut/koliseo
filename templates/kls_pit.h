// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
/*
    Copyright (C) 2023-2026  jgabaut

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
#ifndef KOLISEO_H_
#include "koliseo.h"
#endif // KOLISEO_H_
#define LIST_T Koliseo
#define LIST_NAME KlsList
#include "list.h"


/**
 * Out custom handler for when an underlying Koliseo fails a push operation with "OOM".
 * The only important thing it does is actually return instead of calling exit() inside (as the default handlers would do).
 */
#ifndef KOLISEO_HAS_LOCATE
void KLS_PIT_OOM_default_handler__(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, ptrdiff_t count);
#else
void KLS_PIT_OOM_default_handler_dbg__(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, ptrdiff_t count, Koliseo_Loc loc);
#endif

/**
 * Convenience macro used internally to express the default handlers for Koliseo expected by KLS_Pit.
 */
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

/**
 * Used to pass some configuration to all the Koliseos.
 */
typedef struct KLS_Pit_Conf {
#ifdef KOLISEO_HAS_REGION
    int kls_autoset_regions; /**< If set to 1, make the Koliseos handle the KLS_Regions for their usage.*/
    int kls_autoset_temp_regions; /**< If set to 1, make the Koliseos handle the KLS_Regions for their usage when operating on Koliseo_Temp instances.*/
#endif // KOLISEO_HAS_REGION
    int kls_collect_stats; /**< If set to 1, make the Koliseos collect performance stats.*/
    int kls_block_while_has_temp; /**< If set to 1, make thes Koliseo reject push calls while they have has an open Koliseo_Temp.*/
#ifndef KOLISEO_HAS_LOCATE
    void (*PTRDIFF_MAX_handler)(struct Koliseo* kls, ptrdiff_t size, ptrdiff_t count);
#else
    void (*PTRDIFF_MAX_handler)(struct Koliseo* kls, ptrdiff_t size, ptrdiff_t count, Koliseo_Loc loc);
#endif // KOLISEO_HAS_LOCATE
} KLS_Pit_Conf;

/**
 * Offers a list-of-arenas service, without implementing the list into Koliseo itself.
 */
typedef struct KLS_Pit {
    KlsList list; /**< The Koliseo list.*/
    ptrdiff_t kls_size; /**< Used to set size for all underlying Koliseos.*/
    const KLS_Pit_Conf conf; /**< Used to pass configuration to the initialised Koliseos.*/
} KLS_Pit;

#define KLS_PIT_NULL (KLS_Pit){0}

/**
 * Create a customised KLS_Pit.
 * @param size The size to use for the underlying Koliseos.
 * @param pit_conf The configuration to use for the underlying Koliseos.
 * @return The initialised KLS_Pit, or KLS_PIT_NULL on error.
 */
KLS_Pit kls_new_pit_conf(ptrdiff_t size, KLS_Pit_Conf pit_conf);

/**
 * Create a default KLS_Pit.
 * @param size The size to use for the underlying Koliseos.
 * @return The initialised KLS_Pit, or KLS_PIT_NULL on error.
 */
KLS_Pit kls_new_pit(ptrdiff_t size);

/**
 * Takes a Koliseo pointer, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the Koliseo data field, or goes to exit() if the operation fails.
 * Notably, it zeroes the memory region.
 * @param pit The KLS_Pit at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @return A void pointer to the start of memory just pushed to the KLS_Pit, or NULL for errors.
 */
char* kls_pit_push_zero(KLS_Pit* pit, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);

/**
 * Frees all memory used by the KLS_Pit.
 * @param p The KLS_Pit to free.
 */
void kls_pit_free(KLS_Pit* p);

#define KLS_PIT_PUSH_ARR(pit, type, count) (type*)kls_pit_push_zero((pit), sizeof(type), _Alignof(type), (count))

#define KLS_PIT_PUSH(pit, type) KLS_PIT_PUSH_ARR((pit), type, 1)

#ifdef KLS_PIT_IMPLEMENTATION

#ifndef KOLISEO_HAS_LOCATE
void KLS_PIT_OOM_default_handler__(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, ptrdiff_t count)
{
#else
void KLS_PIT_OOM_default_handler__(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, ptrdiff_t count, Koliseo_Loc loc)
{
#endif
    //fprintf(stderr, "%s():    OOM!\n", __func__);
    (void)0;
}

static inline KLS_Conf kls_conf_from_pitconf(KLS_Pit_Conf pit_conf)
{
    KLS_Conf kls_conf = KLS_DEFAULT_CONF;
#ifdef KOLISEO_HAS_REGION
    kls_conf.kls_autoset_regions = pit_conf.kls_autoset_regions;
    kls_conf.kls_autoset_temp_regions = pit_conf.kls_autoset_temp_regions;
#endif // KOLISEO_HAS_REGION
    kls_conf.kls_collect_stats = pit_conf.kls_collect_stats;
    kls_conf.kls_block_while_has_temp = pit_conf.kls_block_while_has_temp;
    kls_conf.err_handlers = KLS_PIT_ERR_HANDLERS;
    if (pit_conf.PTRDIFF_MAX_handler != NULL) {
        kls_conf.err_handlers.PTRDIFF_MAX_handler = pit_conf.PTRDIFF_MAX_handler;
    }
    return kls_conf;
}

KLS_Pit kls_new_pit_conf(ptrdiff_t size, KLS_Pit_Conf pit_conf)
{
    if (size < sizeof(Koliseo)) {
        size = KLS_DEFAULT_SIZE;
    }
    KLS_Pit res = (KLS_Pit) {
        .list = KlsList_nullList(),
        .kls_size = size,
    };
    KLS_Conf kls_conf = kls_conf_from_pitconf(pit_conf);
    Koliseo* head = kls_new_conf(res.kls_size, kls_conf);
    if (!head) {
        fprintf(stderr, "%s():    Failed creation of head Koliseo\n", __func__);
        return KLS_PIT_NULL;
    }
    res.list = KlsList_cons_gl(head, res.list);
    return res;
}

KLS_Pit kls_new_pit(ptrdiff_t size)
{
    return kls_new_pit_conf(size, (KLS_Pit_Conf) {
        0
    });
}

static inline bool kls_pit_grow(KLS_Pit* pit)
{
    if (!pit) return false;
    KLS_Conf kls_conf = kls_conf_from_pitconf(pit->conf);
    Koliseo* new_kls = kls_new_conf(pit->kls_size, kls_conf);
    if (!new_kls) {
        fprintf(stderr, "%s():    Failed creation of new_kls\n", __func__);
        return false;
    }
    pit->list = KlsList_cons_gl(new_kls, pit->list);
    return true;
}

char* kls_pit_push_zero(KLS_Pit* pit, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count)
{
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

void kls_pit_free(KLS_Pit* p)
{
    if (!p) return;
    KlsList_free_gl(p->list);
}
#endif // KLS_PIT_IMPLEMENTATION
#endif // KLS_PIT_H_
