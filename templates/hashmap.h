#ifdef HASHMAP_T //This ensures the library never causes any trouble if this macro was not defined.
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
| hashmap.h                                                                        |
| This code is based on an idea from https://www.davidpriver.com/ctemplates.html. |
| Include this header multiple times to implement a                               |
| simplistic hashmap.  Before inclusion define at                           |
| least HASHMAP_T to the type of values the hashmap can hold.                          |
| See HASHMAP_NAME, HASHMAP_PREFIX and HASHMAP_LINKAGE for                           |
| other customization points.                                                     |
|                                                                                 |
| If you define HASHMAP_DECLS_ONLY, only the declarations                          |
| of the type and its function will be declared.                                  |
\*********************************************************************************/

#ifndef HASHMAP_HEADER_H
#define HASHMAP_HEADER_H
// Inline functions, #defines and includes that will be
// needed for all instantiations can go up here.
#include <stdlib.h> // realloc, size_t
#include <stdio.h> // fprintf, stderr

#define HASHMAP_IMPL(word) HASHMAP_COMB1(HASHMAP_PREFIX,word)
#define HASHMAP_COMB1(pre, word) HASHMAP_COMB2(pre, word)
#define HASHMAP_COMB2(pre, word) pre##word

#endif // HASHMAP_HEADER_H

// NOTE: this section is *not* guarded as it is intended
// to be included multiple times.

#ifndef HASHMAP_T
#error "HASHMAP_T must be defined"
#endif

// The name of the data type to be generated.
// If not given, will expand to something like
// `darray_int` for an `int`.
#ifndef HASHMAP_NAME
#define HASHMAP_NAME HASHMAP_COMB1(HASHMAP_COMB1(map,_), HASHMAP_T)
#endif

// Prefix for generated functions.
#ifndef HASHMAP_PREFIX
#define HASHMAP_PREFIX HASHMAP_COMB1(HASHMAP_NAME, _)
#endif

// Customize the linkage of the function.
#ifndef HASHMAP_LINKAGE
#define HASHMAP_LINKAGE static inline
#endif
#include <stdlib.h>
#include "koliseo.h"

#ifndef HASHMAP_NODE_NAME
#define HASHMAP_NODE_NAME HASHMAP_COMB1(HASHMAP_NAME, _node)
#endif

typedef struct HASHMAP_NODE_NAME {
    char* key;
    HASHMAP_T* value;
} HASHMAP_NODE_NAME;

#define DARRAY_T HASHMAP_NODE_NAME
#define DARRAY_NAME HASHMAP_COMB1(da_, HASHMAP_NODE_NAME)
#define DARRAY_LINKAGE static inline
#define DARRAY_STARTING_CAPACITY 2
#include "darray.h"
#define DARRAY_T HASHMAP_NODE_NAME
#define DARRAY_NAME HASHMAP_COMB1(da_, HASHMAP_NODE_NAME)
#define HASHMAP_DA_IMPL(word) HASHMAP_COMB1(HASHMAP_COMB1(DARRAY_NAME, _), word)

typedef struct HASHMAP_NAME {
    size_t bucket_count;
    DARRAY_NAME** buckets;
    Koliseo* kls;
} HASHMAP_NAME;

#ifndef HASHMAP_HASH
#define HASHMAP_hash_str HASHMAP_IMPL(hash_str)
#else
#define HASHMAP_hash_str HASHMAP_HASH
#endif // HASHMAP_HASH
#define HASHMAP_new HASHMAP_IMPL(new)
#define HASHMAP_push HASHMAP_IMPL(push)
#define HASHMAP_get HASHMAP_IMPL(get)
#define HASHMAP_remove HASHMAP_IMPL(remove)

#ifdef HASHMAP_DECLS_ONLY

#ifndef HASHMAP_HASH
static
uint64_t
HASHMAP_hash_str(const char* s);
#endif // HASHMAP_HASH

HASHMAP_LINKAGE
HASHMAP_NAME*
HASHMAP_new(Koliseo* kls, size_t bucket_count);

HASHMAP_LINKAGE
bool
HASHMAP_push(HASHMAP_NAME* map, const char* key, HASHMAP_T* value);

HASHMAP_LINKAGE
HASHMAP_T*
HASHMAP_get(HASHMAP_NAME* map, const char* key);

HASHMAP_LINKAGE
bool
HASHMAP_remove(HASHMAP_NAME *map, const char *key);
#else

#ifndef HASHMAP_HASH
/* FNV-1a hash */
static uint64_t HASHMAP_hash_str(const char *s)
{
    uint64_t h = 1469598103934665603ULL;
    while (*s) {
        h ^= (unsigned char)*s++;
        h *= 1099511628211ULL;
    }
    return h;
}
#endif // HASHMAP_HASH

HASHMAP_NAME *HASHMAP_new(Koliseo* kls, size_t bucket_count)
{
    HASHMAP_NAME *map = KLS_PUSH(kls, HASHMAP_NAME);
    if (!map) return NULL;
    map->kls = kls;

    map->bucket_count = bucket_count;
    map->buckets = KLS_PUSH_ARR(kls, DARRAY_NAME*, bucket_count);
    if (!map->buckets) {
        return NULL;
    }
    for (size_t i = 0; i < bucket_count; i++) {
        map->buckets[i] = HASHMAP_DA_IMPL(init)(kls);
    }

    return map;
}

bool HASHMAP_push(HASHMAP_NAME *map, const char *key, HASHMAP_T *value)
{
    uint64_t h = HASHMAP_hash_str(key);
    size_t index = h % map->bucket_count;

    DARRAY_NAME *node = map->buckets[index];
    for (int i = 0; i < node->count; i++) {
        // Collision
        if (strcmp(node->items[i].key, key) == 0) {
            node->items[i].value = value; // Replace
            return false;
        }
    }
    Koliseo* kls = node->allocator.kls;
    char* key_dup = KLS_PUSH_STR(kls, key);
    memcpy(key_dup, key, strlen(key));
    HASHMAP_NODE_NAME new = {
        .key = key_dup,
        .value = value,
    };
    HASHMAP_DA_IMPL(push)(node, new);
    map->buckets[index] = node;
    return true;
}

HASHMAP_T *HASHMAP_get(HASHMAP_NAME *map, const char *key)
{
    uint64_t h = HASHMAP_hash_str(key);
    size_t index = h % map->bucket_count;

    DARRAY_NAME *node = map->buckets[index];
    for (int i = 0; i < node->count; i++) {
        if (strcmp(node->items[i].key, key) == 0) {
            return node->items[i].value;
        }
    }
    return NULL;
}

bool HASHMAP_remove(HASHMAP_NAME *map, const char *key)
{
    uint64_t h = HASHMAP_hash_str(key);
    size_t index = h % map->bucket_count;

    DARRAY_NAME **prev = &map->buckets[index];
    DARRAY_NAME *node = *prev;
    for (int i = 0; i < node->count; i++) {
        if (strcmp(node->items[i].key, key) == 0) {
            for (int j = i; j < node->count -1; j++) {
                (*prev)->items[j] = (*prev)->items[j+1];
            }
            (*prev)->count -= 1;
            return true;
        }
    }
    return false;
}
#endif // HASHMAP_DECLS_ONLY

// Cleanup
// These need to be undef'ed so they can be redefined the
// next time you need to instantiate this template.
#undef HASHMAP_T
#undef HASHMAP_PREFIX
#undef HASHMAP_NAME
#undef HASHMAP_NODE_NAME
#undef HASHMAP_LINKAGE
#undef DARRAY_T
#undef DARRAY_NAME
#undef HASHMAP_DA_IMPL
#undef HASHMAP_hash_str
#ifdef HASHMAP_HASH
#undef HASHMAP_HASH
#endif // HASHMAP_HASH
#undef HASHMAP_new
#undef HASHMAP_push
#undef HASHMAP_get
#undef HASHMAP_remove
#ifdef HASHMAP_DECLS_ONLY
#undef HASHMAP_DECLS_ONLY
#endif // HASHMAP_DECLS_ONLY
#endif // HASHMAP_T
