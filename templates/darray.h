#ifdef DARRAY_T //This ensures the library never causes any trouble if this macro was not defined.
// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
/*
    Copyright (C) 2025-2026  jgabaut

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
| darray.h                                                                        |
| This code is based on an idea from https://www.davidpriver.com/ctemplates.html. |
| Include this header multiple times to implement a                               |
| simplistic dynamic array.  Before inclusion define at                           |
| least DARRAY_T to the type the dynamic array can hold.                          |
| See DARRAY_NAME, DARRAY_PREFIX and DARRAY_LINKAGE for                           |
| other customization points.                                                     |
|                                                                                 |
| If you define DARRAY_DECLS_ONLY, only the declarations                          |
| of the type and its function will be declared.                                  |
\*********************************************************************************/

#ifndef DARRAY_HEADER_H
#define DARRAY_HEADER_H
// Inline functions, #defines and includes that will be
// needed for all instantiations can go up here.
#include <stdlib.h> // realloc, size_t
#include <stdio.h> // fprintf, stderr

#define DARRAY_IMPL(word) DARRAY_COMB1(DARRAY_PREFIX,word)
#define DARRAY_COMB1(pre, word) DARRAY_COMB2(pre, word)
#define DARRAY_COMB2(pre, word) pre##word

#endif // DARRAY_HEADER_H

// NOTE: this section is *not* guarded as it is intended
// to be included multiple times.

#ifndef DARRAY_T
#error "DARRAY_T must be defined"
#endif

// The name of the data type to be generated.
// If not given, will expand to something like
// `darray_int` for an `int`.
#ifndef DARRAY_NAME
#define DARRAY_NAME DARRAY_COMB1(DARRAY_COMB1(darray,_), DARRAY_T)
#endif

// Prefix for generated functions.
#ifndef DARRAY_PREFIX
#define DARRAY_PREFIX DARRAY_COMB1(DARRAY_NAME, _)
#endif

// Customize the linkage of the function.
#ifndef DARRAY_LINKAGE
#define DARRAY_LINKAGE static inline
#endif

#ifndef DARRAY_STARTING_CAPACITY
#define DARRAY_STARTING_CAPACITY 4
#endif // DARRAY_STARTING_CAPACITY

typedef struct DARRAY_NAME DARRAY_NAME;
struct DARRAY_NAME {
    bool use_temp;
    union {
        Koliseo* kls;
        Koliseo_Temp* t_kls;
    } allocator;
    DARRAY_T* items;
    size_t count;
    size_t capacity;
};

#define DARRAY_push DARRAY_IMPL(push)
#define DARRAY_init DARRAY_IMPL(init)
#define DARRAY_push_t DARRAY_IMPL(push_t)
#define DARRAY_init_t DARRAY_IMPL(init_t)

#ifdef DARRAY_DECLS_ONLY

DARRAY_LINKAGE
void
DARRAY_push(DARRAY_NAME* array, DARRAY_T item);

DARRAY_LINKAGE
DARRAY_NAME
DARRAY_init(Koliseo* kls);

DARRAY_LINKAGE
void
DARRAY_push_t(DARRAY_NAME* array, DARRAY_T item);

DARRAY_LINKAGE
DARRAY_NAME
DARRAY_init_t(Koliseo_Temp* t_kls);

#else

DARRAY_LINKAGE
void
DARRAY_push(DARRAY_NAME* array, DARRAY_T item)
{
    if (array->use_temp) {
        fprintf(stderr, "In %s, at %i: %s(): array uses Koliseo_Temp\n", __FILE__, __LINE__, __func__);
        exit(EXIT_FAILURE);
    }
    if(array->count >= array->capacity) {
        size_t old_cap = array->capacity;
        size_t new_cap = old_cap?old_cap*2:4;
        // size_t new_size = new_cap * sizeof(DARRAY_T);
        array->items = KLS_REPUSH(array->allocator.kls, array->items, DARRAY_T, old_cap, new_cap);
        if (!array->items) {
            fprintf(stderr, "In %s, at %i: %s(): failed KLS_REPUSH()\n", __FILE__, __LINE__, __func__);
            exit(EXIT_FAILURE);
        }
        array->capacity = new_cap;
    }
    array->items[array->count++] = item;
}

DARRAY_LINKAGE
DARRAY_NAME*
DARRAY_init(Koliseo* kls)
{
    // This functions sets the passed Koliseo as the backing memory for the array, and returns a pointer to it.
    if(kls == NULL) {
        fprintf(stderr,"In %s, at %i: %s(): kls was NULL.\n", __FILE__, __LINE__, __func__);
        exit(EXIT_FAILURE);
    }
    DARRAY_NAME* res = KLS_PUSH_EX(kls, DARRAY_NAME, "from DARRAY_new()");
    if (res == NULL) {
        fprintf(stderr,"In %s, at %i: %s(): res was NULL.\n", __FILE__, __LINE__, __func__);
        kls_free(res->allocator.kls);
        exit(EXIT_FAILURE);
    }
    res->allocator.kls = kls;
    res->capacity = 4;
    res->items = KLS_PUSH_ARR(kls, DARRAY_T, 4);
    return res;
}

DARRAY_LINKAGE
void
DARRAY_push_t(DARRAY_NAME* array, DARRAY_T item)
{
    if (!array->use_temp) {
        fprintf(stderr, "In %s, at %i: %s(): array uses Koliseo\n", __FILE__, __LINE__, __func__);
        exit(EXIT_FAILURE);
    }
    if(array->count >= array->capacity) {
        size_t old_cap = array->capacity;
        size_t new_cap = old_cap?old_cap*2:4;
        // size_t new_size = new_cap * sizeof(DARRAY_T);
        array->items = KLS_REPUSH_T(array->allocator.t_kls, array->items, DARRAY_T, old_cap, new_cap);
        if (!array->items) {
            fprintf(stderr, "In %s, at %i: %s(): failed KLS_REPUSH()\n", __FILE__, __LINE__, __func__);
            exit(EXIT_FAILURE);
        }
        array->capacity = new_cap;
    }
    array->items[array->count++] = item;
}

DARRAY_LINKAGE
DARRAY_NAME*
DARRAY_init_t(Koliseo_Temp* t_kls)
{
    // This functions sets the passed Koliseo as the backing memory for the array, and returns a pointer to it.
    if(t_kls == NULL) {
        fprintf(stderr,"In %s, at %i: %s(): t_kls was NULL.\n", __FILE__, __LINE__, __func__);
        exit(EXIT_FAILURE);
    }
    DARRAY_NAME* res = KLS_PUSH_T_EX(t_kls, DARRAY_NAME, "from DARRAY_new()");
    if (res == NULL) {
        fprintf(stderr,"In %s, at %i: %s(): res was NULL.\n", __FILE__, __LINE__, __func__);
        kls_temp_end(res->allocator.t_kls);
        exit(EXIT_FAILURE);
    }
    res->use_temp = true;
    res->allocator.t_kls = t_kls;
    res->capacity = DARRAY_STARTING_CAPACITY;
    res->items = KLS_PUSH_ARR_T(t_kls, DARRAY_T, DARRAY_STARTING_CAPACITY);
    return res;
}

#endif // DARRAY_DECLS_ONLY

// Cleanup
// These need to be undef'ed so they can be redefined the
// next time you need to instantiate this template.
#undef DARRAY_T
#undef DARRAY_PREFIX
#undef DARRAY_NAME
#undef DARRAY_LINKAGE
#undef DARRAY_STARTING_CAPACITY
#undef DARRAY_push
#undef DARRAY_init
#undef DARRAY_push_t
#undef DARRAY_init_t
#ifdef DARRAY_DECLS_ONLY
#undef DARRAY_DECLS_ONLY
#endif // DARRAY_DECLS_ONLY
#endif // DARRAY_T
