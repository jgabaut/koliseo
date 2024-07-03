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

#ifndef KOLISEO_H_

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) //We need C11
#define KOLISEO_H_

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#ifdef KLS_DEBUG_CORE
#include <time.h>

#ifdef _WIN32
#include <windows.h>		//Used for QueryPerformanceFrequency(), QueryPerformanceCounter()
#endif
#endif				//KLS_DEBUG_CORE

#define KLS_MAJOR 0 /**< Represents current major release.*/
#define KLS_MINOR 4 /**< Represents current minor release.*/
#define KLS_PATCH 4 /**< Represents current patch release.*/

typedef void*(kls_alloc_func)(size_t); /**< Used to select an allocation function for the arena's backing memory.*/

#define STRINGIFY_2(x) #x

#define STRINGIFY(x) STRINGIFY_2(x)

#ifdef KOLISEO_HAS_REGION
/**
 * Defines allocation backend for KLS_Region_List items.
 * @see Koliseo
 * @see kls_new_traced_AR_KLS()
 */
typedef enum KLS_RegList_Alloc_Backend {
    KLS_REGLIST_ALLOC_LIBC = 0,
    KLS_REGLIST_ALLOC_KLS_BASIC,
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
#endif // KOLISEO_HAS_REGION

/**
 * Defines flags for Koliseo.
 * @see Koliseo
 */
typedef struct KLS_Conf {
#ifdef KOLISEO_HAS_REGION
    int kls_autoset_regions; /**< If set to 1, make the Koliseo handle the KLS_Regions for its usage.*/
    KLS_RegList_Alloc_Backend kls_reglist_alloc_backend; /**< Sets the backend for the KLS_Regions allocation.*/
    ptrdiff_t kls_reglist_kls_size; /**< Sets the size for reglist_kls when on KLS_REGLIST_ALLOC_KLS_BASIC.*/
    int kls_autoset_temp_regions; /**< If set to 1, make the Koliseo handle the KLS_Regions for its usage when operating on a Koliseo_Temp instance.*/
#endif // KOLISEO_HAS_REGION
    int kls_collect_stats; /**< If set to 1, make the Koliseo collect performance stats.*/
    int kls_verbose_lvl; /**< If > 0, makes the Koliseo try to acquire kls_log_fp from kls_log_filepath.*/
    FILE *kls_log_fp; /**< FILE pointer used by the Koliseo to print its kls_log() output.*/
    const char *kls_log_filepath; /**< String representing the path to the Koliseo logfile.*/
} KLS_Conf;

KLS_Conf kls_conf_init(int autoset_regions, int alloc_backend, ptrdiff_t reglist_kls_size, int autoset_temp_regions, int collect_stats, int verbose_lvl, FILE* log_fp, const char* log_filepath);

void kls_dbg_features(void);

/**
 * Defines a stat struct for Koliseo.
 * @see Koliseo
 */
typedef struct KLS_Stats {
    int tot_pushes; /**< Total PUSH calls done.*/
    int tot_temp_pushes; /**< Total PUSH_T calls done.*/
    int tot_pops; /**< Total POP calls done.*/
    int tot_temp_pops; /**< Total POP_T calls done.*/
    int tot_logcalls; /**< Total kls_log() calls done.*/
    int tot_hiccups; /**< Total hiccups encountered.*/
#ifdef KOLISEO_HAS_REGION
    ptrdiff_t avg_region_size; /**< Average size for allocated KLS_Region.*/
#endif
#ifdef KLS_DEBUG_CORE
    double worst_pushcall_time;	/**< Longest time taken by a PUSH call.*/
#endif
} KLS_Stats;

/**
 * Default KLS_Conf used by kls_new().
 * @see kls_new()
 * @see KLS_Conf
 */
extern KLS_Conf KLS_DEFAULT_CONF;

/**
 * Default KLS_Stats values, used by kls_new().
 * @see kls_new()
 * @see KLS_Stats
 */
extern KLS_Stats KLS_STATS_DEFAULT;

/**
 * Defines a format string for KLS_Conf.
 * @see KLS_Conf_Arg()
 */
#ifdef KOLISEO_HAS_REGION
#ifndef _WIN32
#define KLS_Conf_Fmt "KLS_Conf { autoset_regions: %i, reglist_backend: %s, reglist_kls_size: %li, autoset_temp_regions: %i, collect_stats: %i, verbose_lvl: %i, log_filepath: \"%s\", log_fp: %p }"
#else
#define KLS_Conf_Fmt "KLS_Conf { autoset_regions: %i, reglist_backend: %s, reglist_kls_size: %lli, autoset_temp_regions: %i, collect_stats: %i, verbose_lvl: %i, log_filepath: \"%s\", log_fp: %p }"
#endif
#else

#define KLS_Conf_Fmt "KLS_Conf { collect_stats: %i, verbose_lvl: %i, log_filepath: \"%s\", log_fp: %p }"
#endif // KOLISEO_HAS_REGION

/**
 * Defines a format macro for KLS_Conf args.
 * @see KLS_Conf_Fmt
 */
#ifdef KOLISEO_HAS_REGION
#define KLS_Conf_Arg(conf) (conf.kls_autoset_regions),kls_reglist_backend_string((conf.kls_reglist_alloc_backend)),(conf.kls_reglist_kls_size),(conf.kls_autoset_temp_regions),(conf.kls_collect_stats),(conf.kls_verbose_lvl),(conf.kls_log_filepath),(void*)(conf.kls_log_fp)
#else
#define KLS_Conf_Arg(conf) (conf.kls_collect_stats),(conf.kls_verbose_lvl),(conf.kls_log_filepath),(void*)(conf.kls_log_fp)
#endif // KOLISEO_HAS_REGION

/**
 * Defines a format string for KLS_Stats.
 * @see KLS_Stats_Arg()
 */
#ifdef KOLISEO_HAS_REGION

#ifndef _WIN32

#ifdef KLS_DEBUG_CORE
#define KLS_Stats_Fmt "KLS_Stats { tot_pushes: %i, tot_pops: %i, tot_temp_pushes: %i, tot_temp_pops: %i, avg_region_size: %li, tot_hiccups: %i, worst_push_time: %.9f }"
#else
#define KLS_Stats_Fmt "KLS_Stats { tot_pushes: %i, tot_pops: %i, tot_temp_pushes: %i, tot_temp_pops: %i, avg_region_size: %li, tot_hiccups: %i }"
#endif // KLS_DEBUG_CORE

#else

#ifdef KLS_DEBUG_CORE
#define KLS_Stats_Fmt "KLS_Stats { tot_pushes: %i, tot_pops: %i, tot_temp_pushes: %i, tot_temp_pops: %i, avg_region_size: %lli, tot_hiccups: %i, worst_push_time: %.7f }"
#else
#define KLS_Stats_Fmt "KLS_Stats { tot_pushes: %i, tot_pops: %i, tot_temp_pushes: %i, tot_temp_pops: %i, avg_region_size: %lli, tot_hiccups: %i }"
#endif // KLS_DEBUG_CORE
#endif // _WIN32

#else

#ifdef KLS_DEBUG_CORE
#define KLS_Stats_Fmt "KLS_Stats { tot_pushes: %i, tot_pops: %i, tot_temp_pushes: %i, tot_temp_pops: %i, tot_hiccups: %i, worst_push_time: %.7f }"
#else
#define KLS_Stats_Fmt "KLS_Stats { tot_pushes: %i, tot_pops: %i, tot_temp_pushes: %i, tot_temp_pops: %i, tot_hiccups: %i }"
#endif // KLS_DEBUG_CORE

#endif // KOLISEO_HAS_REGION

/**
 * Defines a format macro for KLS_Stats args.
 * @see KLS_Stats_Fmt
 */
#ifdef KOLISEO_HAS_REGION
#ifdef KLS_DEBUG_CORE
#define KLS_Stats_Arg(stats) (stats.tot_pushes),(stats.tot_pops),(stats.tot_temp_pushes),(stats.tot_temp_pops),(stats.avg_region_size),(stats.tot_hiccups),(stats.worst_pushcall_time)
#else
#define KLS_Stats_Arg(stats) (stats.tot_pushes),(stats.tot_pops),(stats.tot_temp_pushes),(stats.tot_temp_pops),(stats.avg_region_size),(stats.tot_hiccups)
#endif // KLS_DEBUG_CORE
#else
#ifdef KLS_DEBUG_CORE
#define KLS_Stats_Arg(stats) (stats.tot_pushes),(stats.tot_pops),(stats.tot_temp_pushes),(stats.tot_temp_pops),(stats.tot_hiccups),(stats.worst_pushcall_time)
#else
#define KLS_Stats_Arg(stats) (stats.tot_pushes),(stats.tot_pops),(stats.tot_temp_pushes),(stats.tot_temp_pops),(stats.tot_hiccups)
#endif // KLS_DEBUG_CORE
#endif // KOLISEO_HAS_REGION

#ifdef KOLISEO_HAS_REGION
/**
 * Defines flags for Koliseo_Temp.
 * @see Koliseo_Temp
 * @see kls_temp_start()
 * @see KLS_Conf
 */
typedef struct KLS_Temp_Conf {
    int kls_autoset_regions; /**< Sets if the Koliseo_Temp will use regions.*/
    KLS_RegList_Alloc_Backend tkls_reglist_alloc_backend; /**< Sets the backend for the KLS_Regions allocation.*/
    ptrdiff_t kls_reglist_kls_size; /**< Sets the size for reglist_kls when on KLS_REGLIST_ALLOC_KLS_BASIC.*/
} KLS_Temp_Conf;
#endif // KOLISEO_HAS_REGION

/**
 * Defines a format string for KLS_Temp_Conf.
 * @see KLS_Temp_Conf_Arg()
 */
#ifndef _WIN32
#define KLS_Temp_Conf_Fmt "KLS_Temp_Conf {autoset_regions: %i, tkls_reglist_alloc_backend: %i, kls_reglist_kls_size: %li}"
#else
#define KLS_Temp_Conf_Fmt "KLS_Temp_Conf {autoset_regions: %i, tkls_reglist_alloc_backend: %i, kls_reglist_kls_size: %lli}"
#endif

/**
 * Defines a format macro for KLS_Conf args.
 * @see KLS_Temp_Conf_Fmt
 */
#define KLS_Temp_Conf_Arg(conf) (conf.kls_autoset_regions),(conf.tkls_reglist_alloc_backend),(conf.kls_reglist_kls_size)

/**
 * Defines current API version number from KLS_MAJOR, KLS_MINOR and KLS_PATCH.
 */
static const int KOLISEO_API_VERSION_INT =
    (KLS_MAJOR * 1000000 + KLS_MINOR * 10000 + KLS_PATCH * 100);
/**< Represents current version with numeric format.*/

/**
 * Defines current API version string.
 */
static const char KOLISEO_API_VERSION_STRING[] = "0.4.4-dev"; /**< Represents current version with MAJOR.MINOR.PATCH format.*/

/**
 * Returns current koliseo version as a string.
 */
const char *string_koliseo_version(void);

/**
 * Returns current koliseo version as an integer.
 */
int int_koliseo_version(void);

#ifdef KOLISEO_HAS_TITLE
#define KLS_TITLEROWS 33 /**< Defines how many rows the title banner has.*/
extern char *kls_title[KLS_TITLEROWS + 1];
/**< Contains title banner.*/

void kls_print_title_2file(FILE * fp);/**< Prints the title banner to the passed FILE.*/
void kls_print_title(void);
#endif // KOLISEO_HAS_TITLE

#define KLS_DEFAULT_SIZE (16*1024) /**< Represents a simple default size for demo purposes.*/

#ifndef KLS_DEFAULT_ALIGNMENT
#define KLS_DEFAULT_ALIGNMENT (2*sizeof(void *)) /**< Represents a default alignment value. Not used.*/
#endif

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

#ifdef KOLISEO_HAS_REGION

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
 * Defines the node for a KLS_Region_SlList.
 * @see KLS_list_element
 */
typedef struct KLS_sllist_region {
    KLS_list_element value;	/**< The KLS_Region value.*/
    struct KLS_sllist_region *next;     /**< Pointer to the next node in the list.*/
} KLS_region_sllist_item;

typedef KLS_region_sllist_item *KLS_Region_SlList;

/**
 * Defines the node for a KLS_Region_DlList.
 * @see KLS_list_element
 */
typedef struct KLS_dllist_region {
    KLS_list_element value; /**< The KLS_Region value.*/
    struct KLS_dllist_region* prev; /**< Pointer to the prev node in the list.*/
    struct KLS_dllist_region* next; /**< Pointer to the next node in the list.*/
} KLS_region_dllist_item;

typedef KLS_region_dllist_item KLS_Region_DlList_Node; /**< Redundant typedef to better denote the head and tail fields of a KLS_region_dllist.*/

typedef struct KLS_region_dllist {
    KLS_Region_DlList_Node *head;
    KLS_Region_DlList_Node *tail;
    size_t len;
} KLS_region_dllist;

typedef KLS_region_dllist *KLS_Region_DlList; /**< Redundant typedef to better denote the doubly linked list field in a Koliseo.*/

typedef union KLS_Region_List {
    KLS_Region_SlList list;     /**< List of allocated Regions*/
    KLS_Region_DlList dllist; /**< Doubly linked list of allocated Regions*/
} KLS_Region_List; /**< Defines the type of list a Koliseo is using.*/

#endif // KOLISEO_HAS_REGION

struct Koliseo_Temp;		//Forward declaration for Koliseo itself

/**
 * Represents the initialised arena allocator struct.
 * @see kls_new()
 * @see kls_clear()
 * @see kls_free()
 * @see KLS_PUSH()
 * @see KLS_POP()
 */
typedef struct Koliseo {
    char *data;	    /**< Points to data field.*/
    ptrdiff_t size;	/**< Size of data field.*/
    ptrdiff_t offset;	  /**< Current position of memory pointer.*/
    ptrdiff_t prev_offset;     /**< Previous position of memory pointer.*/
#ifdef KOLISEO_HAS_REGION
    KLS_Region_List regs; /**< List of allocated regions.*/
    struct Koliseo *reglist_kls; /**< When conf.kls_reglist_alloc_backend is KLS_REGLIST_ALLOC_KLS_BASIC, points to the backing kls for regs list.*/
    int max_regions_kls_alloc_basic; /**< Contains maximum number of allocatable KLS_Region when using KLS_REGLIST_ALLOC_KLS_BASIC.*/
#endif
    int has_temp;     /**< When == 1, a Koliseo_Temp is currently active on this Koliseo.*/
    KLS_Conf conf; /**< Contains flags to change the Koliseo behaviour.*/
    KLS_Stats stats; /**< Contains stats for Koliseo performance analysis.*/
    struct Koliseo_Temp *t_kls;	    /**< Points to related active Kolieo_Temp, when has_temp == 1.*/
} Koliseo;

/**
 * Defines a format string for Koliseo.
 * @see KLSArg()
 */
#ifndef _WIN32
#define KLSFmt "KLS {begin: %p, curr: %p, size: %li, offset: %li, has_temp: %i}"
#else
#define KLSFmt "KLS {begin: %p, curr: %p, size: %lli, offset: %lli, has_temp: %i}"
#endif

/**
 * Defines a format macro for Koliseo args.
 * @see KLSFmt
 */
#define KLS_Arg(kls) (void*)(kls),(void*)((kls)+(kls->offset)),(kls->size),(kls->offset),(kls->has_temp)

/**
 * Represents a savestate for a Koliseo.
 * @see kls_temp_start()
 * @see kls_temp_end()
 * @see KLS_PUSH()
 * @see KLS_POP()
 */
typedef struct Koliseo_Temp {
    Koliseo *kls;     /**< Reference to the actual Koliseo we're saving.*/
    ptrdiff_t offset;	  /**< Current position of memory pointer.*/
    ptrdiff_t prev_offset;     /**< Previous position of memory pointer.*/
#ifdef KOLISEO_HAS_REGION
    KLS_Region_List t_regs; /**< List of temporarily allocated regions.*/
    Koliseo *reglist_kls; /**< Reference to the supporting Koliseo when conf.tkls_reglist_alloc_backend is KLS_BASIC.*/
    int max_regions_kls_alloc_basic; /**< Contains maximum number of allocatable KLS_Region when using KLS_REGLIST_ALLOC_KLS_BASIC.*/
    KLS_Temp_Conf conf;	/**< Contains flags to change the Koliseo_Temp behaviour.*/
#endif
} Koliseo_Temp;

void kls_log(Koliseo * kls, const char *tag, const char *format, ...);
ptrdiff_t kls_get_pos(const Koliseo * kls);

#ifdef KOLISEO_HAS_REGION
int kls_get_maxRegions_KLS_BASIC(Koliseo * kls);
int kls_temp_get_maxRegions_KLS_BASIC(Koliseo_Temp * t_kls);
#endif

Koliseo *kls_new_alloc(ptrdiff_t size, kls_alloc_func alloc_func);

#ifndef KLS_DEFAULT_ALLOCF
#define KLS_DEFAULT_ALLOCF malloc /**< Defines the default allocation function.*/
#endif

#define kls_new(size) kls_new_alloc((size), KLS_DEFAULT_ALLOCF)
//bool kls_set_conf(Koliseo* kls, KLS_Conf conf);
Koliseo *kls_new_conf_alloc(ptrdiff_t size, KLS_Conf conf, kls_alloc_func alloc_func);
#define kls_new_conf(size, conf) kls_new_conf_alloc((size), (conf), KLS_DEFAULT_ALLOCF)
Koliseo *kls_new_traced_alloc(ptrdiff_t size, const char *output_path, kls_alloc_func alloc_func);
#define kls_new_traced(size, output_path) kls_new_traced_alloc((size), (output_path), KLS_DEFAULT_ALLOCF)
Koliseo *kls_new_dbg_alloc(ptrdiff_t size, kls_alloc_func alloc_func);
#define kls_new_dbg(size) kls_new_dbg_alloc((size), KLS_DEFAULT_ALLOCF)
Koliseo *kls_new_traced_AR_KLS_alloc(ptrdiff_t size, const char *output_path,
                                     ptrdiff_t reglist_kls_size, kls_alloc_func alloc_func);
#define kls_new_traced_AR_KLS(size, output_path, reglist_kls_size) kls_new_traced_AR_KLS_alloc((size), (output_path), (reglist_kls_size), KLS_DEFAULT_ALLOCF)

//void* kls_push(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void *kls_push_zero(Koliseo * kls, ptrdiff_t size, ptrdiff_t align,
                    ptrdiff_t count);
void *kls_push_zero_AR(Koliseo * kls, ptrdiff_t size, ptrdiff_t align,
                       ptrdiff_t count);
#ifdef KOLISEO_HAS_REGION
void *kls_push_zero_named(Koliseo * kls, ptrdiff_t size, ptrdiff_t align,
                          ptrdiff_t count, char *name, char *desc);
void *kls_push_zero_typed(Koliseo * kls, ptrdiff_t size, ptrdiff_t align,
                          ptrdiff_t count, int type, char *name, char *desc);
#endif // KOLISEO_HAS_REGION

/**
 * Macro used to request memory for an array of type values from a Koliseo.
 */
#define KLS_PUSH_ARR(kls, type, count) (type*)kls_push_zero_AR((kls), sizeof(type), _Alignof(type), (count))

/**
 * Macro to request memory for a C string from a Koliseo.
 * @see KLS_STRDUP()
 */
#define KLS_PUSH_STR(kls, cstr) KLS_PUSH_ARR((kls), char, strlen((cstr))+1)

/**
 * Macro used to request memory for an array of type values from a Koliseo, and assign a name and a description to the region item.
 */
#ifdef KOLISEO_HAS_REGION
#define KLS_PUSH_ARR_NAMED(kls, type, count, name, desc) (type*)kls_push_zero_named((kls), sizeof(type), _Alignof(type), (count), (name), (desc))
#else
#define KLS_PUSH_ARR_NAMED(kls, type, count, name, desc) KLS_PUSH_ARR((kls),type,(count))
#endif // KOLISEO_HAS_REGION

/**
 * Macro to request memory for a C string from a Koliseo, and assign a name and a description to the region item.
 */
#define KLS_PUSH_STR_NAMED(kls, cstr, name, desc) KLS_PUSH_ARR_NAMED((kls), char, strlen((cstr)), (name), (desc))

/**
 * Macro used to request memory for an array of type values from a Koliseo, and assign a type, a name and a description to the region item.
 */
#ifdef KOLISEO_HAS_REGION
#define KLS_PUSH_ARR_TYPED(kls, type, count, region_type, name, desc) (type*)kls_push_zero_typed((kls), sizeof(type), _Alignof(type), (count), (region_type), (name), (desc))
#else
#define KLS_PUSH_ARR_TYPED(kls, type, count, region_type, name, desc) KLS_PUSH_ARR((kls),type,(count))
#endif // KOLISEO_HAS_REGION

/**
 * Macro to request memory for a C string from a Koliseo, and assign a type, a name and a description to the region item.
 */
#define KLS_PUSH_STR_TYPED(kls, cstr, region_type, name, desc) KLS_PUSH_ARR_TYPED((kls), char, strlen((cstr)), (region_type), (name), (desc))

/**
 * Macro used to request memory from a Koliseo.
 */
#define KLS_PUSH(kls, type) KLS_PUSH_ARR((kls), type, 1)

/**
 * Macro used to request memory from a Koliseo, and assign a name and a description to the region item.
 */
#define KLS_PUSH_NAMED(kls, type, name, desc) KLS_PUSH_ARR_NAMED((kls), type, 1, (name), (desc))

/**
 * Macro used to request memory from a Koliseo, and assign a name to the region item.
 * The description field is automatically filled with the stringized passed type.
 */
#define KLS_PUSH_EX(kls, type, name) KLS_PUSH_NAMED((kls), type, (name), STRINGIFY(type))

/**
 * Macro used to request memory from a Koliseo, and assign a a type, a name and a description to the region item.
 */
#define KLS_PUSH_TYPED(kls, type, region_type, name, desc) KLS_PUSH_ARR_TYPED((kls), type, 1, (region_type), (name), (desc))

/**
 * Macro used to request memory from a Koliseo, and assign a type and a name to the region item.
 * The description field is automatically filled with the stringized passed type.
 */
#define KLS_PUSH_TYPED_EX(kls, type, region_type, name) KLS_PUSH_TYPED((kls), type, (region_type), (name), STRINGIFY(type))

void kls_clear(Koliseo * kls);
void kls_free(Koliseo * kls);
void print_kls_2file(FILE * fp, const Koliseo * kls);
void print_dbg_kls(const Koliseo * kls);
void kls_formatSize(ptrdiff_t size, char *outputBuffer, size_t bufferSize);

#ifdef KOLISEO_HAS_CURSES /**< This definition controls the inclusion of ncurses functions.*/

#ifndef KOLISEO_CURSES_H_
#define KOLISEO_CURSES_H_

#ifndef _WIN32
#include "ncurses.h"
#else
#include <ncursesw/ncurses.h>
#endif	//    _WIN32

void kls_show_toWin(Koliseo * kls, WINDOW * win);
void kls_temp_show_toWin(Koliseo_Temp * t_kls, WINDOW * win);
#ifdef KOLISEO_HAS_REGION
void kls_showList_toWin(Koliseo * kls, WINDOW * win);
void kls_temp_showList_toWin(Koliseo_Temp * t_kls, WINDOW * win);
#endif // KOLISEO_HAS_REGION
#endif				//KOLISEO_CURSES_H_

#endif				//KOLISEO_HAS_CURSES

Koliseo_Temp *kls_temp_start(Koliseo * kls);
//bool kls_temp_set_conf(Koliseo_Temp* t_kls, KLS_Temp_Conf conf);
void kls_temp_end(Koliseo_Temp * tmp_kls);
void *kls_temp_push_zero_AR(Koliseo_Temp * t_kls, ptrdiff_t size,
                            ptrdiff_t align, ptrdiff_t count);
#ifdef KOLISEO_HAS_REGION
void *kls_temp_push_zero_named(Koliseo_Temp * t_kls, ptrdiff_t size,
                               ptrdiff_t align, ptrdiff_t count, char *name,
                               char *desc);
void *kls_temp_push_zero_typed(Koliseo_Temp * t_kls, ptrdiff_t size,
                               ptrdiff_t align, ptrdiff_t count, int type,
                               char *name, char *desc);
#endif // KOLISEO_HAS_REGION
void print_temp_kls_2file(FILE * fp, const Koliseo_Temp * t_kls);
void print_dbg_temp_kls(const Koliseo_Temp * t_kls);

/**
 * Macro used to request memory for an array of type values from a Koliseo_Temp.
 */
#define KLS_PUSH_ARR_T(kls_temp, type, count) (type*)kls_temp_push_zero_AR((kls_temp), sizeof(type), _Alignof(type), (count))

/**
 * Macro to request memory for a C string from a Koliseo_Temp.
 * @see KLS_STRDUP_T()
 */
#define KLS_PUSH_STR_T(kls_temp, cstr) KLS_PUSH_ARR_T((kls_temp), char, strlen((cstr))+1)

/**
 * Macro used to request memory for an array of type values from a Koliseo_Temp, and assign a name and a description to the region item.
 */
#ifdef KOLISEO_HAS_REGION
#define KLS_PUSH_ARR_T_NAMED(kls_temp, type, count, name, desc) (type*)kls_temp_push_zero_named((kls_temp), sizeof(type), _Alignof(type), (count), (name), (desc))
#else
#define KLS_PUSH_ARR_T_NAMED(kls_temp, type, count, name, desc) KLS_PUSH_ARR_T((kls_temp),type,(count))
#endif // KOLISEO_HAS_REGION

/**
 * Macro to request memory for a C string from a Koliseo_Temp, and assign a name and a description to the region item.
 */
#define KLS_PUSH_STR_T_NAMED(kls_temp, cstr, name, desc) KLS_PUSH_ARR_T_NAMED((kls_temp), char, strlen((cstr)), (name), (desc))

/**
 * Macro used to request memory for an array of type values from a Koliseo_Temp, and assign a type, a name and a description to the region item.
 */
#ifdef KOLISEO_HAS_REGION
#define KLS_PUSH_ARR_T_TYPED(kls_temp, type, count, region_type, name, desc) (type*)kls_temp_push_zero_typed((kls_temp), sizeof(type), _Alignof(type), (count), (region_type), (name), (desc))
#else
#define KLS_PUSH_ARR_T_TYPED(kls_temp, type, count, region_type, name, desc) KLS_PUSH_ARR_T((kls_temp),type,(count))
#endif // KOLISEO_HAS_REGION

/**
 * Macro to request memory for a C string from a Koliseo_Temp, and assign a type, a name and a description to the region item.
 */
#define KLS_PUSH_STR_T_TYPED(kls_temp, cstr, region_type, name, desc) KLS_PUSH_ARR_T_TYPED((kls_temp), char, strlen((cstr)), (region_type), (name), (desc))

/**
 * Macro used to request memory from a Koliseo_Temp.
 */
#define KLS_PUSH_T(kls_temp, type) KLS_PUSH_ARR_T((kls_temp), type, 1)

/**
 * Macro used to request memory from a Koliseo_Temp, and assign a name and a description to the region item.
 */
#define KLS_PUSH_T_NAMED(kls_temp, type, name, desc) KLS_PUSH_ARR_T_NAMED((kls_temp), type, 1, (name), (desc))

/**
 * Macro used to request memory from a Koliseo_Temp, and assign a name to the region item.
 * The description field is automatically filled with the stringized passed type.
 */
#define KLS_PUSH_T_EX(kls_temp, type, name) KLS_PUSH_T_NAMED((kls_temp), type, (name), STRINGIFY(type))

/**
 * Macro used to request memory from a Koliseo_Temp, and assign a type, a name and a description to the region item.
 */
#define KLS_PUSH_T_TYPED(kls_temp, type, region_type, name, desc) KLS_PUSH_ARR_T_TYPED((kls_temp), type, 1, (region_type), (name), (desc))

/**
 * Macro used to request memory from a Koliseo_Temp, and assign a type and a name to the region item.
 * The description field is automatically filled with the stringized passed type.
 */
#define KLS_PUSH_T_TYPED_EX(kls_temp, type, region_type, name) KLS_PUSH_T_TYPED((kls_temp), type, (region_type), (name), STRINFIGY(type))

#ifdef KOLISEO_HAS_REGION

KLS_Region_SlList kls_rl_emptyList(void);
#define KLS_RL_GETLIST() kls_rl_emptyList()
bool kls_rl_empty(KLS_Region_SlList);
KLS_list_element kls_rl_head(KLS_Region_SlList);
KLS_Region_SlList kls_rl_tail(KLS_Region_SlList);
KLS_Region_SlList kls_rl_cons(Koliseo *, KLS_list_element, KLS_Region_SlList);
#ifdef KOLISEO_HAS_EXPER
KLS_region_sllist_item* kls_rl_list_pop(Koliseo *kls);
#endif // KOLISEO_HAS_EXPER
KLS_Region_SlList kls_rl_t_cons(Koliseo_Temp *, KLS_list_element, KLS_Region_SlList);
#ifdef KOLISEO_HAS_EXPER
KLS_region_sllist_item* kls_rl_t_list_pop(Koliseo_Temp *t_kls);
#endif // KOLISEO_HAS_EXPER

void kls_rl_freeList(KLS_Region_SlList);
#define KLS_RL_FREELIST(kls_list) kls_rl_freeList((kls_list).list)
void kls_rl_showList(KLS_Region_SlList);
#define kls_showList(kls_list) kls_rl_showList((kls_list).list)
void kls_rl_showList_toFile(KLS_Region_SlList, FILE * fp);
#define kls_showList_toFile(kls_list, fp) kls_rl_showList_toFile((kls_list).list, (fp))
#define KLS_RL_ECHOLIST(kls_list) kls_rl_showList((kls_list).list)
#define KLS_RL_PRINTLIST(kls_list,file) kls_rl_showList_toFile((kls_list).list,file)
bool kls_rl_member(KLS_list_element, KLS_Region_SlList);
int kls_rl_length(KLS_Region_SlList);
KLS_Region_SlList kls_rl_append(Koliseo *, KLS_Region_SlList, KLS_Region_SlList);
KLS_Region_SlList kls_rl_reverse(Koliseo *, KLS_Region_SlList);
KLS_Region_SlList kls_rl_copy(Koliseo *, KLS_Region_SlList);
KLS_Region_SlList kls_rl_delete(Koliseo *, KLS_list_element, KLS_Region_SlList);

KLS_Region_SlList kls_rl_insord(Koliseo *, KLS_list_element, KLS_Region_SlList);
#define KLS_RL_PUSHLIST(kls,reg,kls_list) kls_rl_insord(kls,reg,(kls_list).list)
KLS_Region_SlList kls_rl_insord_p(Koliseo *, KLS_list_element, KLS_Region_SlList);
#define KLS_RL_PUSHLIST_P(kls,reg,kls_list) kls_rl_insord_p(kls,reg,(kls_list).list)
KLS_Region_SlList kls_rl_mergeList(Koliseo *, KLS_Region_SlList, KLS_Region_SlList);
KLS_Region_SlList kls_rl_intersect(Koliseo *, KLS_Region_SlList, KLS_Region_SlList);
KLS_Region_SlList kls_rl_diff(Koliseo *, KLS_Region_SlList, KLS_Region_SlList);

#define KLS_RL_DIFF(kls,kls_list1,kls_list2) kls_rl_diff(kls,(kls_list1).list,(kls_list2).list)
bool kls_rl_isLess(KLS_list_element, KLS_list_element);
bool kls_rl_isEqual(KLS_list_element, KLS_list_element);
double kls_usageShare(KLS_list_element, Koliseo *);
ptrdiff_t kls_regionSize(KLS_list_element);
ptrdiff_t kls_avg_regionSize(Koliseo *);
void kls_usageReport_toFile(Koliseo *, FILE *);
void kls_usageReport(Koliseo *);
ptrdiff_t kls_type_usage(int, Koliseo *);

// Doubly linked list functions
KLS_Region_DlList_Node* kls_rdll_node_new(Koliseo*, KLS_Region*);
KLS_Region_DlList* kls_rdll_list_new(Koliseo*);
bool kls_rdll_rpush(Koliseo*, KLS_Region_DlList*, KLS_Region_DlList_Node*);
bool kls_rdll_lpush(Koliseo*, KLS_Region_DlList*, KLS_Region_DlList_Node*);
KLS_Region_DlList_Node* kls_rdll_rpop(Koliseo*, KLS_Region_DlList*);
KLS_Region_DlList_Node* kls_rdll_lpop(Koliseo*, KLS_Region_DlList*);

#endif // KOLISEO_HAS_REGION

#ifdef KOLISEO_HAS_GULP /**< This definition controls the inclusion of gulp functions.*/

#ifndef KOLISEO_GULP_H_
#define KOLISEO_GULP_H_

#include "ctype.h" // Needed for isspace()...

typedef struct Kstr {
    const char* data;
    size_t len;
} Kstr;

Kstr kstr_new(const char* str, size_t len);
Kstr kstr_from_c_lit(const char* c_lit);
bool kstr_eq(Kstr left, Kstr right);
bool kstr_eq_ignorecase(Kstr left, Kstr right);
Kstr kstr_cut_l(Kstr *k, size_t n);
Kstr kstr_cut_r(Kstr *k, size_t n);
Kstr kstr_trim_left(Kstr kstr);
Kstr kstr_trim_right(Kstr kstr);
Kstr kstr_trim(Kstr kstr);
bool kstr_indexof(Kstr k, char c, int* idx);
Kstr kstr_token(Kstr* k, char delim);
bool kstr_try_token(Kstr* k, char delim, Kstr* part);
Kstr kstr_token_kstr(Kstr* k, Kstr delim);

#define KSTR(c_lit) kstr_new(c_lit, sizeof(c_lit) - 1)
#define KSTR_NULL kstr_new(NULL, 0)

/**
 * Format macro for a Kstr.
 */
#define Kstr_Fmt "%.*s"
/**
 * Format matching macro for a Kstr.
 */
#define Kstr_Arg(kstr) (int) (kstr.len), (kstr.data)

/**
 * Defines a one GB size as decimal integer representation.
 * @see GULP_MAX_FILE_SIZE
 * @see GULP_FILE_TOO_LARGE
 * @see kls_gulp_file_sized()
 * @see KLS_GULP_FILE()
 */
#define ONEGB_DEC_INT 1073741824

/**
 * Defines max size allowed for a file passed to KLS_GULP_FILE().
 * Use try_kls_gulp_file() if you want a different upper limit.
 */
#define GULP_MAX_FILE_SIZE ONEGB_DEC_INT

/**
 * Defines possible results for kls_gulp_file_sized().
 * @see kls_gulp_file_sized().
 */
typedef enum Gulp_Res {
    GULP_FILE_OK=0,
    GULP_FILE_NOT_EXIST,
    GULP_FILE_TOO_LARGE,
    GULP_FILE_READ_ERROR,
    GULP_FILE_CONTAINS_NULLCHAR,
    GULP_FILE_KLS_NULL,
    TOT_GULP_RES
} Gulp_Res;

/**
 * Format macro for a Gulp_Res.
 */
#define Gulp_Res_Fmt "%s"
/**
 * Format matching macro for a Gulp_Res.
 */
#define Gulp_Res_Arg(gr) (string_from_Gulp_Res((gr)))

/**
 * String array for representations of Gulp_Res.
 * @see string_from_Gulp_Res()
 */
extern const char* gulp_res_names[TOT_GULP_RES+1];
const char* string_from_Gulp_Res(Gulp_Res g);

//static char * kls_read_file(Koliseo* kls, const char * f_name, Gulp_Res * err, size_t * f_size, ...);
char * kls_gulp_file_sized(Koliseo* kls, const char * filepath, Gulp_Res * err, size_t max_size);
char * try_kls_gulp_file(Koliseo* kls, const char * filepath, size_t max_size);
#define KLS_GULP_FILE(kls, filepath) try_kls_gulp_file((kls),(filepath), GULP_MAX_FILE_SIZE)
//Kstr * kls_read_file_to_kstr(Koliseo* kls, const char * f_name, Gulp_Res * err, size_t * f_size, ...);
Kstr * kls_gulp_file_sized_to_kstr(Koliseo* kls, const char * filepath, Gulp_Res * err, size_t max_size, bool allow_nullchar);
Kstr * try_kls_gulp_file_to_kstr(Koliseo* kls, const char * filepath, size_t max_size, bool allow_nullchar);
#define KLS_GULP_FILE_KSTR(kls, filepath) try_kls_gulp_file_to_kstr((kls),(filepath), GULP_MAX_FILE_SIZE, false)

#endif				//KOLISEO_GULP_H_

#endif				//KOLISEO_HAS_GULP

#ifdef KOLISEO_HAS_EXPER

void *kls_pop(Koliseo * kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void *kls_pop_AR(Koliseo *kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);

/**
 * Macro used to "remove" memory as an array from a Koliseo. Rewinds the pointer by the requested type and returns a pointer to that memory before updating the Koliseo index.
 * It's up to you to copy your item somewhere else before calling any PUSH operation again, as that memory should be overwritten.
 */
#define KLS_POP_ARR(kls, type, count) (type*)kls_pop_AR((kls), sizeof(type), _Alignof(type), (count))

/**
 * Macro to "remove" the memory for a C string from a Koliseo. Rewinds the pointer by the string's memory and returns a pointer to that memory before updating the Koliseo index.
 * It's up to you to copy your string somewhere else before calling any PUSH operation again, as that memory should be overwritten.
 */
#define KLS_POP_STR(kls, cstr) KLS_POP_ARR((kls), char, strlen((cstr)))

/**
 * Macro used to "remove" memory from a Koliseo. Rewinds the pointer by the requested type and returns a pointer to that memory before updating the Koliseo index.
 * It's up to you to copy your item somewhere else before calling any PUSH operation again, as that memory should be overwritten.
 */
#define KLS_POP(kls, type) KLS_POP_ARR((kls), type, 1)

void *kls_temp_pop(Koliseo_Temp * t_kls, ptrdiff_t size, ptrdiff_t align,
                   ptrdiff_t count);
void *kls_temp_pop_AR(Koliseo_Temp *t_kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);

/**
 * Macro used to "remove" memory as an array from a Koliseo_Temp. Rewinds the pointer by the requested type and returns a pointer to that memory before updating the Koliseo_Temp index.
 * It's up to you to copy your item somewhere else before calling any PUSH operation again, as that memory should be overwritten.
 */
#define KLS_POP_ARR_T(kls_temp, type, count) (type*)kls_temp_pop_AR((kls_temp), sizeof(type), _Alignof(type), (count))

/**
 * Macro to "remove" the memory for a C string from a Koliseo_Temp. Rewinds the pointer by the string's memory and returns a pointer to that memory before updating the Koliseo_Temp index.
 * It's up to you to copy your string somewhere else before calling any PUSH operation again, as that memory should be overwritten.
 */
#define KLS_POP_STR_T(kls_temp, cstr) KLS_POP_ARR_T((kls_temp), char, strlen((cstr)))

/**
 * Macro used to "remove" memory from a Koliseo_Temp. Rewinds the pointer by the requested type and returns a pointer to that memory before updating the Koliseo_Temp index.
 * It's up to you to copy your item somewhere else before calling any PUSH operation again, as that memory should be overwritten.
 */
#define KLS_POP_T(kls_temp, type) KLS_POP_ARR_T((kls_temp), type, 1)

char* kls_strdup(Koliseo* kls, char* source);
char** kls_strdup_arr(Koliseo* kls, size_t count, char** source);

/**
 * Macro to copy a C string from a source buffer to a destination buffer.
 * Unsafe, do not use.
 * Used in KLS_STRDUP() and KLS_STRDUP_T().
 * @see KLS_STRDUP()
 * @see KLS_STRDUP_T()
 */
#define __KLS_STRCPY(dest, source) do {\
    strcpy((dest), (source));\
} while (0)

/*
 * Macro to dupe a C string to a passed Koliseo, returns a pointer to the allocated string.
 * Unsafe, do not use.
 * @see kls_strdup()
 */
#define KLS_STRDUP(kls, source) kls_strdup((kls), (source))

char* kls_t_strdup(Koliseo_Temp* t_kls, char* source);
char** kls_t_strdup_arr(Koliseo_Temp* t_kls, size_t count, char** source);

/*
 * Macro to dupe a C string to a passed Koliseo_Temp, returns a pointer to the allocated string.
 * Unsafe, do not use.
 * @see kls_t_strdup()
 */
#define KLS_STRDUP_T(t_kls, source) kls_t_strdup((t_kls), (source))

#endif // KOLISEO_HAS_EXPER

#else
#error "This code requires C11 or later.\n    _Alignof() is not available"
#endif // __STDC_VERSION__ && __STDC_VERSION__ >= 201112L //We need C11

#endif //KOLISEO_H_

#ifdef LIST_T //This ensures the library never causes any trouble if this macro was not defined.
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
// list.h
// This is a template for a linked list, inspired by the dynamic array example in https://www.davidpriver.com/ctemplates.html#template-headers.
// Include this header multiple times to implement a
// simplistic linked list.  Before inclusion define at
// least DLIST_T to the type the linked list can hold.
// See DLIST_NAME, DLIST_PREFIX and DLIST_LINKAGE for
// other customization points.
//
// If you define DLIST_DECLS_ONLY, only the declarations
// of the type and its function will be declared.
//
// Functions ending with _gl use malloc() for the nodes.
// Functions ending with _kls expect a Koliseo arg to use for allocating nodes.
//

#ifndef LIST_HEADER_H
#define LIST_HEADER_H
// Inline functions, #defines and includes that will be
// needed for all instantiations can go up here.
#include <stdbool.h> // bool
#include <stdlib.h> // malloc, size_t

#define LIST_IMPL(word) LIST_COMB1(LIST_PREFIX,word)
#define LIST_COMB1(pre, word) LIST_COMB2(pre, word)
#define LIST_COMB2(pre, word) pre##word

#define LIST_HEADER_VERSION "0.1.0"

#endif // LIST_HEADER_H

// NOTE: this section is *not* guarded as it is intended
// to be included multiple times.

#ifndef LIST_T
#error "LIST_T must be defined"
#endif

// The name of the data type to be generated.
// If not given, will expand to something like
// `list_int` for an `int`.
#ifndef LIST_NAME
#define LIST_NAME LIST_COMB1(LIST_COMB1(list,_), LIST_T)
#endif

// Prefix for generated functions.
#ifndef LIST_PREFIX
#define LIST_PREFIX LIST_COMB1(LIST_NAME, _)
#endif

// Customize the linkage of the function.
#ifndef LIST_LINKAGE
#define LIST_LINKAGE static inline
#endif

// Suffix for generated list item struct.
#ifndef LIST_I_SUFFIX
#define LIST_I_SUFFIX item
#endif

// The name of the item data type to be generated.
#ifndef LIST_ITEM_NAME
#define LIST_ITEM_NAME LIST_COMB1(LIST_COMB1(LIST_T,_), LIST_I_SUFFIX)
#endif

typedef struct LIST_ITEM_NAME LIST_ITEM_NAME;
struct LIST_ITEM_NAME {
    LIST_T* value;
    struct LIST_ITEM_NAME* next;
};
typedef LIST_ITEM_NAME* LIST_NAME;

#define LIST_nullList LIST_IMPL(nullList)
#define LIST_isEmpty LIST_IMPL(isEmpty)
#define LIST_head LIST_IMPL(head)
#define LIST_tail LIST_IMPL(tail)
#define LIST_cons_gl LIST_IMPL(cons_gl)
#define LIST_cons_kls LIST_IMPL(cons_kls)
#define LIST_free_gl LIST_IMPL(free_gl)
#define LIST_member LIST_IMPL(member)
#define LIST_length LIST_IMPL(length)
#define LIST_append_gl LIST_IMPL(append_gl)
#define LIST_append_kls LIST_IMPL(append_kls)
#define LIST_reverse_gl LIST_IMPL(reverse_gl)
#define LIST_reverse_kls LIST_IMPL(reverse_kls)
#define LIST_copy_gl LIST_IMPL(copy_gl)
#define LIST_copy_kls LIST_IMPL(copy_kls)
#define LIST_remove_gl LIST_IMPL(remove_gl)
#define LIST_remove_kls LIST_IMPL(remove_kls)
#define LIST_intersect_gl LIST_IMPL(intersect_gl)
#define LIST_intersect_kls LIST_IMPL(intersect_kls)
#define LIST_diff_gl LIST_IMPL(diff_gl)
#define LIST_diff_kls LIST_IMPL(diff_kls)

#ifdef LIST_DECLS_ONLY

LIST_LINKAGE
LIST_NAME
LIST_nullList(void);

LIST_LINKAGE
bool
LIST_isEmpty(LIST_NAME list);

LIST_LINKAGE
LIST_T*
LIST_head(LIST_NAME list);

LIST_LINKAGE
LIST_NAME
LIST_tail(LIST_NAME list);

LIST_LINKAGE
LIST_NAME
LIST_cons_gl(LIST_T* element, LIST_NAME list);

LIST_LINKAGE
LIST_NAME
LIST_cons_kls(Koliseo* kls, LIST_T* element, LIST_NAME list);

LIST_LINKAGE
void
LIST_free_gl(LIST_NAME list);

LIST_LINKAGE
bool
LIST_member(LIST_T* element, LIST_NAME list);

LIST_LINKAGE
int
LIST_length(LIST_NAME list);

LIST_LINKAGE
LIST_NAME
LIST_append_gl(LIST_NAME l1, LIST_NAME l2);

LIST_LINKAGE
LIST_NAME
LIST_append_kls(Koliseo* kls, LIST_NAME l1, LIST_NAME l2);

LIST_LINKAGE
LIST_NAME
LIST_reverse_gl(LIST_NAME list);

LIST_LINKAGE
LIST_NAME
LIST_reverse_kls(Koliseo* kls, LIST_NAME list);

LIST_LINKAGE
LIST_NAME
LIST_copy_gl(LIST_NAME list);

LIST_LINKAGE
LIST_NAME
LIST_copy_kls(Koliseo* kls, LIST_NAME list);

LIST_LINKAGE
LIST_NAME
LIST_remove_gl(LIST_T* element, LIST_NAME list);

LIST_LINKAGE
LIST_NAME
LIST_remove_kls(Koliseo* kls, LIST_T* element, LIST_NAME list);

LIST_LINKAGE
LIST_NAME
LIST_intersect_gl(LIST_NAME l1, LIST_NAME l2);

LIST_LINKAGE
LIST_NAME
LIST_intersect_kls(Koliseo* kls, LIST_NAME l1, LIST_NAME l2);

LIST_LINKAGE
LIST_NAME
LIST_diff_gl(LIST_NAME l1, LIST_NAME l2);

LIST_LINKAGE
LIST_NAME
LIST_diff_kls(Koliseo* kls, LIST_NAME l1, LIST_NAME l2);
#else

LIST_LINKAGE
LIST_NAME
LIST_nullList(void)
{
    return NULL;
}

LIST_LINKAGE
bool
LIST_isEmpty(LIST_NAME list)
{
    if (list == NULL) {
        return true;
    };
    return false;
}

LIST_LINKAGE
LIST_T*
LIST_head(LIST_NAME list)
{
    if (LIST_isEmpty(list)) {
        fprintf(stderr, "%s at %i: %s(): List is empty.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    return list->value;
}

LIST_LINKAGE
LIST_NAME
LIST_tail(LIST_NAME list)
{
    if (LIST_isEmpty(list)) {
        fprintf(stderr, "%s at %i: %s(): List is empty.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    return list->next;
}

LIST_LINKAGE
LIST_NAME
LIST_cons_gl(LIST_T* element, LIST_NAME list)
{
    LIST_NAME t;
    t = (LIST_NAME) malloc(sizeof(LIST_ITEM_NAME));
    t->value = element;
    t->next = list;
    return t;
}

LIST_LINKAGE
LIST_NAME
LIST_cons_kls(Koliseo* kls, LIST_T* element, LIST_NAME list)
{
    if (kls == NULL) {
        fprintf(stderr, "%s at %i: %s(): Koliseo is NULL.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    LIST_NAME t;
    t = (LIST_NAME) KLS_PUSH_EX(kls, LIST_ITEM_NAME, "List node");
    if (t == NULL ) {
        fprintf(stderr, "%s at %i: %s(): Failed KLS_PUSH_EX() call.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    t->value = element;
    t->next = list;
    return t;
}

LIST_LINKAGE
void
LIST_free_gl(LIST_NAME list)
{
    if (LIST_isEmpty(list)) {
        return;
    } else {
        LIST_free_gl(LIST_tail(list));
        free(list);
    }
    return;
}

LIST_LINKAGE
bool
LIST_member(LIST_T* element, LIST_NAME list)
{
    if (LIST_isEmpty(list)) {
        return false;
    } else {
        if (element == LIST_head(list)) {
            return true;
        } else {
            return LIST_member(element, LIST_tail(list));
        }
    }
}

LIST_LINKAGE
int
LIST_length(LIST_NAME list)
{
    if (LIST_isEmpty(list)) {
        return 0;
    } else {
        return 1 + LIST_length(LIST_tail(list));
    }
}

LIST_LINKAGE
LIST_NAME
LIST_append_gl(LIST_NAME l1, LIST_NAME l2)
{
    if (LIST_isEmpty(l1)) {
        return l2;
    } else {
        return LIST_cons_gl(LIST_head(l1), LIST_append_gl(LIST_tail(l1), l2));
    }
}

LIST_LINKAGE
LIST_NAME
LIST_append_kls(Koliseo* kls, LIST_NAME l1, LIST_NAME l2)
{
    if (kls == NULL) {
        fprintf(stderr, "%s at %i: %s(): Koliseo is NULL.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    if (LIST_isEmpty(l1)) {
        return l2;
    } else {
        return LIST_cons_kls(kls, LIST_head(l1), LIST_append_kls(kls, LIST_tail(l1), l2));
    }
}

LIST_LINKAGE
LIST_NAME
LIST_reverse_gl(LIST_NAME list)
{
    if (LIST_isEmpty(list)) {
        return LIST_nullList();
    } else {
        return LIST_append_gl(LIST_reverse_gl(LIST_tail(list)), LIST_cons_gl(LIST_head(list), LIST_nullList()));
    }
}

LIST_LINKAGE
LIST_NAME
LIST_reverse_kls(Koliseo* kls, LIST_NAME list)
{
    if (kls == NULL) {
        fprintf(stderr, "%s at %i: %s(): Koliseo is NULL.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    if (LIST_isEmpty(list)) {
        return LIST_nullList();
    } else {
        return LIST_append_kls(kls, LIST_reverse_kls(kls, LIST_tail(list)), LIST_cons_kls(kls, LIST_head(list), LIST_nullList()));
    }
}

LIST_LINKAGE
LIST_NAME
LIST_copy_gl(LIST_NAME list)
{
    if (LIST_isEmpty(list)) {
        return list;
    } else {
        return LIST_cons_gl(LIST_head(list), LIST_copy_gl(LIST_tail(list)));
    }
}

LIST_LINKAGE
LIST_NAME
LIST_copy_kls(Koliseo* kls, LIST_NAME list)
{
    if (kls == NULL) {
        fprintf(stderr, "%s at %i: %s(): Koliseo is NULL.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    if (LIST_isEmpty(list)) {
        return list;
    } else {
        return LIST_cons_kls(kls, LIST_head(list), LIST_copy_kls(kls, LIST_tail(list)));
    }

}

LIST_LINKAGE
LIST_NAME
LIST_remove_gl(LIST_T* element, LIST_NAME list)
{
    if (LIST_isEmpty(list)) {
        return LIST_nullList();
    } else {
        if (element == LIST_head(list)) {
            return LIST_tail(list);
        } else {
            return LIST_cons_gl(LIST_head(list), LIST_remove_gl(element, LIST_tail(list)));
        }
    }
}

LIST_LINKAGE
LIST_NAME
LIST_remove_kls(Koliseo* kls, LIST_T* element, LIST_NAME list)
{
    if (kls == NULL) {
        fprintf(stderr, "%s at %i: %s(): Koliseo is NULL.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    if (LIST_isEmpty(list)) {
        return LIST_nullList();
    } else {
        if (element == LIST_head(list)) {
            return LIST_tail(list);
        } else {
            return LIST_cons_kls(kls, LIST_head(list), LIST_remove_kls(kls, element, LIST_tail(list)));
        }
    }
}

LIST_LINKAGE
LIST_NAME
LIST_intersect_gl(LIST_NAME l1, LIST_NAME l2)
{
    if (LIST_isEmpty(l1) || LIST_isEmpty(l2)) {
        return LIST_nullList();
    }
    if (LIST_member(LIST_head(l1), l2) && !(LIST_member(LIST_head(l1), LIST_tail(l2)))) {
        return LIST_cons_gl(LIST_head(l1), LIST_intersect_gl(LIST_tail(l1), l2));
    } else {
        return LIST_intersect_gl(LIST_tail(l1), l2);
    }
}

LIST_LINKAGE
LIST_NAME
LIST_intersect_kls(Koliseo* kls, LIST_NAME l1, LIST_NAME l2)
{
    if (kls == NULL) {
        fprintf(stderr, "%s at %i: %s(): Koliseo is NULL.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    if (LIST_isEmpty(l1) || LIST_isEmpty(l2)) {
        return LIST_nullList();
    }
    if (LIST_member(LIST_head(l1), l2) && !(LIST_member(LIST_head(l1), LIST_tail(l2)))) {
        return LIST_cons_kls(kls, LIST_head(l1), LIST_intersect_kls(kls, LIST_tail(l1), l2));
    } else {
        return LIST_intersect_kls(kls, LIST_tail(l1), l2);
    }
}

LIST_LINKAGE
LIST_NAME
LIST_diff_gl(LIST_NAME l1, LIST_NAME l2)
{
    if (LIST_isEmpty(l1) || LIST_isEmpty(l2)) {
        return l1;
    } else {
        if (!LIST_member(LIST_head(l1), l2) && !LIST_member(LIST_head(l1), LIST_tail(l1))) {
            return LIST_cons_gl(LIST_head(l1), LIST_diff_gl(LIST_tail(l1), l2));
        } else {
            return LIST_diff_gl(LIST_tail(l1), l2);
        }
    }
}

LIST_LINKAGE
LIST_NAME
LIST_diff_kls(Koliseo* kls, LIST_NAME l1, LIST_NAME l2)
{
    if (kls == NULL) {
        fprintf(stderr, "%s at %i: %s(): Koliseo is NULL.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    if (LIST_isEmpty(l1) || LIST_isEmpty(l2)) {
        return l1;
    } else {
        if (!LIST_member(LIST_head(l1), l2) && !LIST_member(LIST_head(l1), LIST_tail(l1))) {
            return LIST_cons_kls(kls, LIST_head(l1), LIST_diff_kls(kls, LIST_tail(l1), l2));
        } else {
            return LIST_diff_kls(kls, LIST_tail(l1), l2);
        }
    }
}
#endif

// Cleanup
// These need to be undef'ed so they can be redefined the
// next time you need to instantiate this template.
#undef LIST_T
#undef LIST_PREFIX
#undef LIST_NAME
#undef LIST_LINKAGE
#undef LIST_I_SUFFIX
#undef LIST_ITEM_NAME
#undef LIST_nullList
#undef LIST_isEmpty
#undef LIST_head
#undef LIST_tail
#undef LIST_cons_gl
#undef LIST_cons_kls
#undef LIST_free_gl
#undef LIST_member
#undef LIST_length
#undef LIST_append_gl
#undef LIST_append_kls
#undef LIST_reverse_gl
#undef LIST_reverse_kls
#undef LIST_copy_gl
#undef LIST_copy_kls
#undef LIST_remove_gl
#undef LIST_remove_kls
#undef LIST_intersect_gl
#undef LIST_intersect_kls
#undef LIST_diff_gl
#undef LIST_diff_kls
#ifdef LIST_DECLS_ONLY
#undef LIST_DECLS_ONLY
#endif // LIST_DECLS_ONLY
#endif // LIST_T
