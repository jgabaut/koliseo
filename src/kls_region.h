// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
/*
    Copyright (C) 2023-2025  jgabaut

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
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include <time.h>
#include <stddef.h>
#include <assert.h>

#ifndef KLS_AUTOREGION_AUTOSET_DEFAULT
#define KLS_AUTOREGION_AUTOSET_DEFAULT 1
#endif // KLS_AUTOREGION_AUTOSET_DEFAULT

#ifndef KLS_AUTOREGION_ALLOC_DEFAULT
#define KLS_AUTOREGION_ALLOC_DEFAULT KLS_REGLIST_ALLOC_KLS_BASIC
#endif // KLS_AUTOREGION_ALLOC_DEFAULT

#ifndef KLS_AUTOREGION_REGLIST_SIZE_DEFAULT
#define KLS_AUTOREGION_REGLIST_SIZE_DEFAULT KLS_DEFAULT_SIZE
#endif // KLS_AUTOREGION_REGLIST_SIZE_DEFAULT

#ifndef KLS_AUTOREGION_AUTOSET_TEMP_DEFAULT
#define KLS_AUTOREGION_AUTOSET_TEMP_DEFAULT 1
#endif // KLS_AUTOREGION_AUTOSET_TEMP_DEFAULT

#ifndef KLS_AUTOREGION_ALLOC_TEMP_DEFAULT
#define KLS_AUTOREGION_ALLOC_TEMP_DEFAULT KLS_REGLIST_ALLOC_KLS_BASIC
#endif // KLS_AUTOREGION_ALLOC_TEMP_DEFAULT

#ifndef KLS_AUTOREGION_REGLIST_TEMP_SIZE_DEFAULT
#define KLS_AUTOREGION_REGLIST_TEMP_SIZE_DEFAULT KLS_DEFAULT_SIZE
#endif // KLS_AUTOREGION_REGLIST_TEMP_SIZE_DEFAULT

/**
 * Defines allocation backend for KLS_Region_List items.
 * @see Koliseo
 * @see kls_new_traced_AR_KLS()
 */
typedef enum KLS_RegList_Alloc_Backend {
    KLS_REGLIST_ALLOC_LIBC = 0,
    KLS_REGLIST_ALLOC_KLS_BASIC,
    KLS_REGLIST_ALLOC_KLS,
    KLS_REGLIST_TOTAL_BACKENDS
} KLS_RegList_Alloc_Backend;

/**
 * Defines strings for KLS_RegList_Alloc_Backend values.
 * @see KLS_RegList_Alloc_Backend
 * @see kls_reglist_backend_string()
 */
extern const char* kls_reglist_backend_strings[KLS_REGLIST_TOTAL_BACKENDS];

/**
 * Returns the string corresponding to passed kls_be.
 * @see KLS_RegList_Alloc_Backend
 * @see kls_reglist_backend_strings
 */
const char* kls_reglist_backend_string(KLS_RegList_Alloc_Backend kls_be);

/**
 * Represents a type index for Regions.
 * @see KLS_PUSH_TYPED()
 */
typedef enum KLS_Region_Type {
    KLS_None = 0,
    Temp_KLS_Header = 1,
    KLS_Header = 2,
} KLS_Region_Type;

/**
 * Defines max index for Koliseo's own Region_Type values.
 * @see Region_Type
 */
#define KLS_REGIONTYPE_MAX KLS_Header

/**
 * Defines max size for KLS_Region's name field.
 * @see KLS_Region
 */
#define KLS_REGION_MAX_NAME_SIZE 15
/**
 * Defines max size for KLS_Region's desc field.
 * @see KLS_Region
 */
#define KLS_REGION_MAX_DESC_SIZE 20

/**
 * Represents an allocated memory region in a Koliseo.
 * @see KLS_PUSH()
 * @see KLS_PUSH_NAMED()
 */
typedef struct KLS_Region {
    ptrdiff_t begin_offset;	/**< Starting offset of memory region.*/
    ptrdiff_t end_offset;     /**< Ending offset of memory region.*/
    ptrdiff_t size;	/**< Size of memory for the KLS_Region.*/
    ptrdiff_t padding;	   /**< Size of padding for the KLS_Region.*/
    char name[KLS_REGION_MAX_NAME_SIZE + 1];   /**< Name field for the KLS_Region.*/
    char desc[KLS_REGION_MAX_DESC_SIZE + 1];   /**< Description field for the KLS_Region.*/
    int type;	  /**< Used to identify which type the KLS_Region holds.*/
} KLS_Region;

static const char KOLISEO_DEFAULT_REGION_NAME[] = "No Name"; /**< Represents default Region name, used for kls_push_zero().*/
static const char KOLISEO_DEFAULT_REGION_DESC[] = "No Desc"; /**< Represents default Region desc, used for kls_push_zero().*/

typedef KLS_Region *KLS_list_element; /**< Redundant typedef to better denote the actual value field of a KLS_region_list_item.*/

/**
 * Defines the node for a KLS_Region_List.
 * @see KLS_list_element
 */
typedef struct KLS_list_region {
    KLS_list_element value;	/**< The KLS_Region value.*/
    struct KLS_list_region *next;     /**< Pointer to the next node int the list.*/
} KLS_region_list_item;

typedef KLS_region_list_item *KLS_Region_List;

struct Koliseo; // Forward declaration
struct Koliseo_Temp; // Forward declaration

typedef struct KLS_Autoregion_Extension_Conf {
    int kls_autoset_regions; /**< If set to 1, make the Koliseo handle the KLS_Regions for its usage.*/
    KLS_RegList_Alloc_Backend kls_reglist_alloc_backend; /**< Sets the backend for the KLS_Regions allocation.*/
    ptrdiff_t kls_reglist_kls_size; /**< Sets the size for reglist_kls when on KLS_REGLIST_ALLOC_KLS_BASIC.*/
    int kls_autoset_temp_regions; /**< If set to 1, make the Koliseo handle the KLS_Regions for its usage when operating on a Koliseo_Temp instance.*/
    KLS_RegList_Alloc_Backend tkls_reglist_alloc_backend; /**< Sets the backend for the KLS_Regions allocation.*/
    ptrdiff_t tkls_reglist_kls_size; /**< Sets the size for reglist_kls when on KLS_REGLIST_ALLOC_KLS_BASIC.*/
} KLS_Autoregion_Extension_Conf;

typedef struct KLS_Autoregion_Extension_Data {
    KLS_Region_List regs;     /**< List of allocated Regions*/
    struct Koliseo *reglist_kls; /**< When conf.kls_reglist_alloc_backend is KLS_REGLIST_ALLOC_KLS_BASIC, points to the backing kls for regs list.*/
    int max_regions_kls_alloc_basic; /**< Contains maximum number of allocatable KLS_Region when using KLS_REGLIST_ALLOC_KLS_BASIC.*/
    KLS_Region_List t_regs;     /**< List of allocated Regions for a Koliseo_Temp instance.*/
    struct Koliseo *t_reglist_kls; /**< When conf.kls_reglist_alloc_backend is KLS_REGLIST_ALLOC_KLS_BASIC, points to the backing kls for t_regs list.*/
    KLS_Autoregion_Extension_Conf conf; /**< Contains config for autoregion extension.*/
} KLS_Autoregion_Extension_Data;

void KLS_autoregion_on_new(struct Koliseo* kls);
void KLS_autoregion_on_free(struct Koliseo* kls);
void KLS_autoregion_on_push(struct Koliseo* kls, ptrdiff_t padding, const char* caller, void* user);
void KLS_autoregion_on_temp_start(struct Koliseo_Temp* t_kls);
void KLS_autoregion_on_temp_free(struct Koliseo_Temp* t_kls);
void KLS_autoregion_on_temp_push(struct Koliseo_Temp* t_kls, ptrdiff_t padding, const char* caller, void* user);

#undef KLS_DEFAULT_HOOKS
#define KLS_DEFAULT_HOOKS &(KLS_Hooks) { \
        .on_new_handler = &KLS_autoregion_on_new, \
        .on_free_handler = &KLS_autoregion_on_free, \
        .on_push_handler =  &KLS_autoregion_on_push, \
        .on_temp_start_handler = &KLS_autoregion_on_temp_start, \
        .on_temp_free_handler = &KLS_autoregion_on_temp_free, \
        .on_temp_push_handler = &KLS_autoregion_on_temp_push, \
    }

#ifndef KLS_DEFAULT_EXTENSIONS_LEN
#define KLS_DEFAULT_EXTENSIONS_LEN 1
#endif // KLS_DEFAULT_EXTENSIONS_LEN

#include "koliseo.h"

#ifndef KOLISEO_HAS_LOCATE
void *kls_push_zero_named(Koliseo * kls, ptrdiff_t size, ptrdiff_t align,
                          ptrdiff_t count, char *name, char *desc);
#else
void *kls_push_zero_named_dbg(Koliseo * kls, ptrdiff_t size, ptrdiff_t align,
                              ptrdiff_t count, char *name, char *desc, Koliseo_Loc loc);
#define kls_push_zero_named(kls, size, align, count, name, desc) kls_push_zero_named_dbg((kls), (size), (align), (count), (name), (desc), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE

#ifndef KOLISEO_HAS_LOCATE
void *kls_push_zero_typed(Koliseo * kls, ptrdiff_t size, ptrdiff_t align,
                          ptrdiff_t count, int type, char *name, char *desc);
#else
void *kls_push_zero_typed_dbg(Koliseo * kls, ptrdiff_t size, ptrdiff_t align,
                              ptrdiff_t count, int type, char *name, char *desc, Koliseo_Loc loc);
#define kls_push_zero_typed(kls, size, align, count, type, name, desc) kls_push_zero_typed_dbg((kls), (size), (align), (count), (type), (name), (desc), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE

#ifndef KOLISEO_HAS_LOCATE
void *kls_temp_push_zero_named(Koliseo_Temp * t_kls, ptrdiff_t size,
                               ptrdiff_t align, ptrdiff_t count, char *name,
                               char *desc);
#else
void *kls_temp_push_zero_named_dbg(Koliseo_Temp * t_kls, ptrdiff_t size,
                                   ptrdiff_t align, ptrdiff_t count, char *name,
                                   char *desc, Koliseo_Loc loc);
#define kls_temp_push_zero_named(t_kls, size, align, count, name, desc) kls_temp_push_zero_named_dbg((t_kls), (size), (align), (count), (name), (desc), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE

#ifndef KOLISEO_HAS_LOCATE
void *kls_temp_push_zero_typed(Koliseo_Temp * t_kls, ptrdiff_t size,
                               ptrdiff_t align, ptrdiff_t count, int type,
                               char *name, char *desc);
#else
void *kls_temp_push_zero_typed_dbg(Koliseo_Temp * t_kls, ptrdiff_t size,
                                   ptrdiff_t align, ptrdiff_t count, int type,
                                   char *name, char *desc, Koliseo_Loc loc);
#define kls_temp_push_zero_typed(t_kls, size, align, count, type, name, desc) kls_temp_push_zero_typed_dbg((t_kls), (size), (align), (count), (type), (name), (desc), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE

#undef KLS_PUSH_ARR_NAMED
#define KLS_PUSH_ARR_NAMED(kls, type, count, name, desc) (type*)kls_push_zero_named((kls), sizeof(type), _Alignof(type), (count), (name), (desc))

#undef KLS_PUSH_ARR_TYPED
#define KLS_PUSH_ARR_TYPED(kls, type, count, region_type, name, desc) (type*)kls_push_zero_typed((kls), sizeof(type), _Alignof(type), (count), (region_type), (name), (desc))

#undef KLS_PUSH_ARR_T_NAMED
#define KLS_PUSH_ARR_T_NAMED(kls_temp, type, count, name, desc) (type*)kls_temp_push_zero_named((kls_temp), sizeof(type), _Alignof(type), (count), (name), (desc))

#undef KLS_PUSH_ARR_T_TYPED
#define KLS_PUSH_ARR_T_TYPED(kls_temp, type, count, region_type, name, desc) (type*)kls_temp_push_zero_typed((kls_temp), sizeof(type), _Alignof(type), (count), (region_type), (name), (desc))

KLS_Region_List kls_rl_emptyList(void);
#define KLS_RL_GETLIST() kls_rl_emptyList()
bool kls_rl_empty(KLS_Region_List);
KLS_list_element kls_rl_head(KLS_Region_List);
KLS_Region_List kls_rl_tail(KLS_Region_List);
KLS_Region_List kls_rl_cons(Koliseo *, KLS_list_element, KLS_Region_List);
KLS_Region_List kls_rl_t_cons(Koliseo_Temp *, KLS_list_element, KLS_Region_List);

void kls_rl_freeList(KLS_Region_List);
#define KLS_RL_FREELIST(kls_list) kls_rl_freeList(kls_list)
void kls_rl_showList(KLS_Region_List);
#define kls_showList(list) kls_rl_showList((list))
void kls_rl_showList_toFile(KLS_Region_List, FILE * fp);
#define kls_showList_toFile(list, fp) kls_rl_showList_toFile((list), (fp))
#define KLS_RL_ECHOLIST(kls_list) kls_rl_showList(kls_list)
#define KLS_RL_PRINTLIST(kls_list,file) kls_rl_showList_toFile(kls_list,file)
bool kls_rl_member(KLS_list_element, KLS_Region_List);
int kls_rl_length(KLS_Region_List);
KLS_Region_List kls_rl_append(Koliseo *, KLS_Region_List, KLS_Region_List);
KLS_Region_List kls_rl_reverse(Koliseo *, KLS_Region_List);
KLS_Region_List kls_rl_copy(Koliseo *, KLS_Region_List);
KLS_Region_List kls_rl_delete(Koliseo *, KLS_list_element, KLS_Region_List);

KLS_Region_List kls_rl_insord(Koliseo *, KLS_list_element, KLS_Region_List);
#define KLS_RL_PUSHLIST(kls,reg,kls_list) kls_rl_insord(kls,reg,kls_list)
KLS_Region_List kls_rl_insord_p(Koliseo *, KLS_list_element, KLS_Region_List);
bool kls_rl_isLess(KLS_list_element, KLS_list_element);
bool kls_rl_isEqual(KLS_list_element, KLS_list_element);
#define KLS_RL_PUSHLIST_P(kls,reg,kls_list) kls_rl_insord_p(kls,reg,kls_list)
KLS_Region_List kls_rl_mergeList(Koliseo *, KLS_Region_List, KLS_Region_List);
KLS_Region_List kls_rl_intersect(Koliseo *, KLS_Region_List, KLS_Region_List);
KLS_Region_List kls_rl_diff(Koliseo *, KLS_Region_List, KLS_Region_List);

#define KLS_RL_DIFF(kls,kls_list1,kls_list2) kls_rl_diff(kls,kls_list1,kls_list2)
double kls_usageShare(KLS_list_element, Koliseo *);
ptrdiff_t kls_regionSize(KLS_list_element);
ptrdiff_t kls_avg_regionSize(Koliseo *);
void kls_usageReport_toFile(Koliseo *, FILE *);
void kls_usageReport(Koliseo *);
ptrdiff_t kls_type_usage(int, Koliseo *);
ptrdiff_t kls_total_padding(Koliseo *);
int kls_get_maxRegions_KLS_BASIC(Koliseo *kls);
