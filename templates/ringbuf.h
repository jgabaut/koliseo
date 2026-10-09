#ifdef RINGBUF_T //This ensures the library never causes any trouble if this macro was not defined.
// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
/*
    Copyright (C) 2026  jgabaut

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
/*********************************************************************************\
| ringbuf.h                                                                       |
| This code is based on an idea from https://www.davidpriver.com/ctemplates.html. |
| Include this header multiple times to implement a                               |
| simplistic ring buffer.  Before inclusion define at                             |
| least RINGBUF_T to the type the ring buffer can hold.                           |
| See RINGBUF_NAME, RINGBUF_PREFIX and RINGBUF_LINKAGE for                        |
| other customization points.                                                     |
|                                                                                 |
| If you define RINGBUF_DECLS_ONLY, only the declarations                         |
| of the type and its function will be declared.                                  |
\*********************************************************************************/

#ifndef RINGBUF_HEADER_H
#define RINGBUF_HEADER_H
// Inline functions, #defines and includes that will be
// needed for all instantiations can go up here.
#include <stdlib.h> // realloc, size_t, qsort
#include <stdio.h> // fprintf, stderr
#include <inttypes.h>

#define RINGBUF_IMPL(word) RINGBUF_COMB1(RINGBUF_PREFIX,word)
#define RINGBUF_COMB1(pre, word) RINGBUF_COMB2(pre, word)
#define RINGBUF_COMB2(pre, word) pre##word

#endif // RINGBUF_HEADER_H

// NOTE: this section is *not* guarded as it is intended
// to be included multiple times.

#ifndef RINGBUF_T
#error "RINGBUF_T must be defined"
#endif

// The name of the data type to be generated.
// If not given, will expand to something like
// `ringbuf_int` for an `int`.
#ifndef RINGBUF_NAME
#define RINGBUF_NAME RINGBUF_COMB1(RINGBUF_COMB1(ringbuf,_), RINGBUF_T)
#endif

// Prefix for generated functions.
#ifndef RINGBUF_PREFIX
#define RINGBUF_PREFIX RINGBUF_COMB1(RINGBUF_NAME, _)
#endif

// Customize the linkage of the function.
#ifndef RINGBUF_LINKAGE
#define RINGBUF_LINKAGE static inline
#endif

typedef struct RINGBUF_NAME RINGBUF_NAME;
struct RINGBUF_NAME {
    bool use_temp;
    union {
        Koliseo* kls;
        Koliseo_Temp* t_kls;
    } allocator;
    RINGBUF_T* items;
    size_t head;
    size_t tail;
    size_t capacity;
    bool is_full;
};

#define RINGBUF_init RINGBUF_IMPL(init)
#define RINGBUF_init_t RINGBUF_IMPL(init_t)
#define RINGBUF_get_head RINGBUF_IMPL(get_head)
#define RINGBUF_get_tail RINGBUF_IMPL(get_tail)
#define RINGBUF_get_capacity RINGBUF_IMPL(get_capacity)
#define RINGBUF_isfull RINGBUF_IMPL(isfull)
#define RINGBUF_push RINGBUF_IMPL(push)
#define RINGBUF_get_newest_idx RINGBUF_IMPL(get_newest_idx)
#define RINGBUF_get_oldest_idx RINGBUF_IMPL(get_oldest_idx)
#define RINGBUF_getelem_by_index RINGBUF_IMPL(getelem_by_index)
#define RINGBUF_getelem_newest RINGBUF_IMPL(getelem_newest)
#define RINGBUF_getelem_oldest RINGBUF_IMPL(getelem_oldest)
#define RINGBUF_pop RINGBUF_IMPL(pop)

#ifdef RINGBUF_DECLS_ONLY

RINGBUF_LINKAGE
RINGBUF_NAME*
RINGBUF_init(Koliseo* kls, size_t capacity);

RINGBUF_LINKAGE
RINGBUF_NAME*
RINGBUF_init_t(Koliseo_Temp* t_kls, size_t capacity);

RINGBUF_LINKAGE
size_t
RINGBUF_get_head(RINGBUF_NAME* rb);

RINGBUF_LINKAGE
size_t
RINGBUF_get_tail(RINGBUF_NAME* rb);

RINGBUF_LINKAGE
size_t
RINGBUF_get_capacity(RINGBUF_NAME* rb);

RINGBUF_LINKAGE
bool
RINGBUF_isfull(RINGBUF_NAME* rb);

RINGBUF_LINKAGE
bool
RINGBUF_push(RINGBUF_NAME* rb, RINGBUF_T item);

RINGBUF_LINKAGE
size_t
RINGBUF_get_newest_idx(RINGBUF_NAME* rb, bool* result);

RINGBUF_LINKAGE
size_t
RINGBUF_get_oldest_idx(RINGBUF_NAME* rb, bool* result);

RINGBUF_LINKAGE
RINGBUF_T*
RINGBUF_getelem_by_index(RINGBUF_NAME* rb, size_t index, bool* result);

RINGBUF_LINKAGE
RINGBUF_T*
RINGBUF_getelem_newest(RINGBUF_NAME* rb, bool* result);

RINGBUF_LINKAGE
RINGBUF_T*
RINGBUF_getelem_oldest(RINGBUF_NAME* rb, bool* result);

RINGBUF_LINKAGE
RINGBUF_T*
RINGBUF_pop(RINGBUF_NAME* rb, bool* result);
#else

RINGBUF_LINKAGE
RINGBUF_NAME*
RINGBUF_init(Koliseo* kls, size_t capacity)
{
    // This functions sets the passed Koliseo as the backing memory for the array, and returns a pointer to it.
    if (kls == NULL) {
        fprintf(stderr,"In %s, at %i: %s(): kls was NULL.\n", __FILE__, __LINE__, __func__);
        exit(EXIT_FAILURE);
    }
    RINGBUF_NAME* res = KLS_PUSH_EX(kls, RINGBUF_NAME, "from RINGBUF_new()");
    if (res == NULL) {
        fprintf(stderr,"In %s, at %i: %s(): res was NULL.\n", __FILE__, __LINE__, __func__);
        kls_free(res->allocator.kls);
        exit(EXIT_FAILURE);
    }
    res->allocator.kls = kls;
    res->capacity = capacity;
    res->head = 0;
    res->tail = 0;
    res->is_full = false;
    res->items = KLS_PUSH_ARR(kls, RINGBUF_T, capacity);
    return res;
}

RINGBUF_LINKAGE
RINGBUF_NAME*
RINGBUF_init_t(Koliseo_Temp* t_kls, size_t capacity)
{
    // This functions sets the passed Koliseo as the backing memory for the array, and returns a pointer to it.
    if(t_kls == NULL) {
        fprintf(stderr,"In %s, at %i: %s(): kls was NULL.\n", __FILE__, __LINE__, __func__);
        exit(EXIT_FAILURE);
    }
    RINGBUF_NAME* res = KLS_PUSH_T_EX(t_kls, RINGBUF_NAME, "from RINGBUF_new()");
    if (res == NULL) {
        fprintf(stderr,"In %s, at %i: %s(): res was NULL.\n", __FILE__, __LINE__, __func__);
        kls_temp_end(res->allocator.t_kls);
        exit(EXIT_FAILURE);
    }
    res->use_temp = true;
    res->allocator.t_kls = t_kls;
    res->capacity = capacity;
    res->head = 0;
    res->tail = 0;
    res->is_full = false;
    res->items = KLS_PUSH_ARR_T(t_kls, RINGBUF_T, capacity);
    return res;
}

RINGBUF_LINKAGE
size_t
RINGBUF_get_head(RINGBUF_NAME* rb)
{
    return rb->head;
}

RINGBUF_LINKAGE
size_t
RINGBUF_get_tail(RINGBUF_NAME* rb)
{
    return rb->tail;
}

RINGBUF_LINKAGE
size_t
RINGBUF_get_capacity(RINGBUF_NAME* rb)
{
    return rb->capacity;
}

RINGBUF_LINKAGE
bool
RINGBUF_isfull(RINGBUF_NAME* rb)
{
    return rb->is_full;
}

RINGBUF_LINKAGE
bool
RINGBUF_push(RINGBUF_NAME* rb, RINGBUF_T item)
{
    if (!rb) {
        fprintf(stderr, "%s():    rb was NULL.\n", __func__);
        return false;
    }
    if (rb->capacity == 0) {
        fprintf(stderr, "%s():    rb->capacity was 0.\n", __func__);
        return false;
    }
    memcpy(&(rb->items[rb->head]), &item, sizeof(RINGBUF_T));
    rb->head = (rb->head +1) % rb->capacity;

    if (rb->is_full) {
        rb->tail = (rb->tail +1) % rb->capacity;
    }
    rb->is_full = (rb->head == rb->tail);

    return true;
}

RINGBUF_LINKAGE
size_t
RINGBUF_get_newest_idx(RINGBUF_NAME* rb, bool* result)
{
    if (!rb) {
        fprintf(stderr, "%s():    rb was NULL.\n", __func__);
        *result = false;
        return -1;
    }
    if (rb->capacity == 0) {
        fprintf(stderr, "%s():    rb->capacity was 0.\n", __func__);
        *result = false;
        return -1;
    }
    size_t head = rb->head;
    size_t max_idx = rb->capacity -1;
    size_t newest_idx = -1;

    if (rb->is_full) {
        newest_idx = (head == 0 ? max_idx : head-1);
    } else {
        if (rb->head == 0) {
            fprintf(stderr, "%s():    ring is empty.\n", __func__);
            *result = false;
            return -1;
        }
        newest_idx = head-1;
    }

    *result = true;
    return newest_idx;
}

RINGBUF_LINKAGE
size_t
RINGBUF_get_oldest_idx(RINGBUF_NAME* rb, bool* result)
{
    if (!rb) {
        fprintf(stderr, "%s():    rb was NULL.\n", __func__);
        *result = false;
        return -1;
    }

    size_t head = rb->head;
    size_t oldest_idx = -1;

    if (rb->is_full) {
        oldest_idx = (head == 0 ? 0 : head);
    } else {
        if (head == 0) {
            fprintf(stderr, "%s():    ring is empty.\n", __func__);
            *result = false;
            return -1;
        }
        oldest_idx = 0;
    }
    *result = true;
    return oldest_idx;
}

RINGBUF_LINKAGE
RINGBUF_T*
RINGBUF_getelem_by_index(RINGBUF_NAME* rb, size_t index, bool* result)
{
    if (!rb) {
        fprintf(stderr, "%s():    rb was NULL.\n", __func__);
        *result = false;
        return NULL;
    }
    if (index < 0) {
        fprintf(stderr, "%s():    Passed index is negative.\n", __func__);
        *result = false;
        return NULL;
    }
    if (index > rb->capacity) {
#ifndef _WIN32
        fprintf(stderr, "%s():    Passed index { %li } is greater than capacity { %li }\n", __func__, index, rb->capacity);
#else
        fprintf(stderr, "%s():    Passed index { %lli } is greater than capacity { %lli }\n", __func__, index, rb->capacity);
#endif // _WIN32
        *result = false;
        return NULL;
    }
    if (!RINGBUF_isfull(rb)) {
        size_t head = rb->head;
        size_t max_idx = head -1;
        if (index > max_idx) {
#ifndef _WIN32
            fprintf(stderr, "%s():    Passed index { %li } is greater than max_idx { %li }\n", __func__, index, max_idx);
#else
            fprintf(stderr, "%s():    Passed index { %lli } is greater than max_idx { %lli }\n", __func__, index, max_idx);
#endif // _WIN32
            *result = false;
            return NULL;
        }
    }

    *result = true;
    return &(rb->items[index]);
}

RINGBUF_LINKAGE
RINGBUF_T*
RINGBUF_getelem_newest(RINGBUF_NAME* rb, bool* result)
{
    if (!rb) {
        fprintf(stderr, "%s():    rb was NULL.\n", __func__);
        *result = false;
        return NULL;
    }
    bool idx_res = true;
    size_t newest_idx = RINGBUF_get_newest_idx(rb, &idx_res);
    if (newest_idx >= 0 && idx_res) {
        return RINGBUF_getelem_by_index(rb, newest_idx, result);
    } else {
        if (!idx_res) {
            fprintf(stderr, "%s():    Failed rb_get_newest_idx().\n", __func__);
            *result = false;
            return NULL;
        } else {
#ifndef _WIN32
            fprintf(stderr, "%s():    rb_get_newest_idx() did not fail, but index was negative. {%li}\n", __func__, newest_idx);
#else
            fprintf(stderr, "%s():    rb_get_newest_idx() did not fail, but index was negative. {%lli}\n", __func__, newest_idx);
#endif
            *result = false;
            return NULL;
        }
    }
}

RINGBUF_LINKAGE
RINGBUF_T*
RINGBUF_getelem_oldest(RINGBUF_NAME* rb, bool* result)
{
    if (!rb) {
        fprintf(stderr, "%s():    rb was NULL.\n", __func__);
        *result = false;
        return NULL;
    }
    bool idx_res = true;
    size_t oldest_idx = RINGBUF_get_oldest_idx(rb, &idx_res);
    if (oldest_idx >= 0 && idx_res) {
        return RINGBUF_getelem_by_index(rb, oldest_idx, result);
    } else {
        if (!idx_res) {
            fprintf(stderr, "%s():    Failed rb_get_oldest_idx().\n", __func__);
            *result = false;
            return NULL;
        } else {
#ifndef _WIN32
            fprintf(stderr, "%s():    rb_get_oldest_idx() did not fail, but index was negative. {%li}\n", __func__, oldest_idx);
#else
            fprintf(stderr, "%s():    rb_get_oldest_idx() did not fail, but index was negative. {%lli}\n", __func__, oldest_idx);
#endif
            *result = false;
            return NULL;
        }
    }
}

RINGBUF_LINKAGE
RINGBUF_T*
RINGBUF_pop(RINGBUF_NAME* rb, bool* result)
{
    if (!rb) {
        fprintf(stderr, "%s():    rb was NULL.\n", __func__);
        *result = false;
        return NULL;
    }
    if (rb->capacity == 0) {
        fprintf(stderr, "%s():    rb->capacity was 0.\n", __func__);
        *result = false;
        return NULL;
    }
    if (rb->head == rb->tail && !rb->is_full) {
        fprintf(stderr, "%s():    Buffer is empty.\n", __func__);
        *result = false;
        return NULL;
    }

    RINGBUF_T* res = &(rb->items[rb->tail]);
    rb->tail = (rb->tail +1) % rb->capacity;
    rb->is_full = false;
    *result = true;
    return res;
}
#endif // RINGBUF_DECLS_ONLY

// Cleanup
// These need to be undef'ed so they can be redefined the
// next time you need to instantiate this template.
#undef RINGBUF_T
#undef RINGBUF_PREFIX
#undef RINGBUF_NAME
#undef RINGBUF_LINKAGE
#undef RINGBUF_init
#undef RINGBUF_init_t
#undef RINGBUF_get_head
#undef RINGBUF_get_tail
#undef RINGBUF_get_capacity
#undef RINGBUF_isfull
#undef RINGBUF_push
#undef RINGBUF_get_newest_idx
#undef RINGBUF_get_oldest_idx
#undef RINGBUF_getelem_by_index
#undef RINGBUF_getelem_newest
#undef RINGBUF_getelem_oldest
#undef RINGBUF_pop
#ifdef RINGBUF_DECLS_ONLY
#undef RINGBUF_DECLS_ONLY
#endif // RINGBUF_DECLS_ONLY
#endif // RINGBUF_T
