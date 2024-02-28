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
#define KLS_PATCH 1 /**< Represents current patch release.*/

typedef void*(kls_alloc_func)(size_t); /**< Used to select an allocation function for the arena's backing memory.*/

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
    double worst_pushcall_time;	/**< Longest time taken by a PUSH call.*/
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
#define KLS_Stats_Fmt "KLS_Stats { tot_pushes: %i, tot_pops: %i, tot_temp_pushes: %i, tot_temp_pops: %i, avg_region_size: %li, tot_hiccups: %i, worst_push_time: %.9f }"
#else
#define KLS_Stats_Fmt "KLS_Stats { tot_pushes: %i, tot_pops: %i, tot_temp_pushes: %i, tot_temp_pops: %i, avg_region_size: %lli, tot_hiccups: %i, worst_push_time: %.7f }"
#endif
#else

#define KLS_Stats_Fmt "KLS_Stats { tot_pushes: %i, tot_pops: %i, tot_temp_pushes: %i, tot_temp_pops: %i, tot_hiccups: %i, worst_push_time: %.7f }"
#endif // KOLISEO_HAS_REGION

/**
 * Defines a format macro for KLS_Stats args.
 * @see KLS_Stats_Fmt
 */
#ifdef KOLISEO_HAS_REGION
#define KLS_Stats_Arg(stats) (stats.tot_pushes),(stats.tot_pops),(stats.tot_temp_pushes),(stats.tot_temp_pops),(stats.avg_region_size),(stats.tot_hiccups),(stats.worst_pushcall_time)
#else

#define KLS_Stats_Arg(stats) (stats.tot_pushes),(stats.tot_pops),(stats.tot_temp_pushes),(stats.tot_temp_pops),(stats.tot_hiccups),(stats.worst_pushcall_time)
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
static const char KOLISEO_API_VERSION_STRING[] = "0.4.1-dev"; /**< Represents current version with MAJOR.MINOR.PATCH format.*/

/**
 * Returns current koliseo version as a string.
 */
const char *string_koliseo_version(void);

/**
 * Returns current koliseo version as an integer.
 */
const int int_koliseo_version(void);


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
 * Defines the node for a KLS_Region_List.
 * @see KLS_list_element
 */
typedef struct KLS_list_region {
    KLS_list_element value;	/**< The KLS_Region value.*/
    struct KLS_list_region *next;     /**< Pointer to the next node int the list.*/
} KLS_region_list_item;

typedef KLS_region_list_item *KLS_Region_List;
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
    KLS_Region_List regs;     /**< List of allocated Regions*/
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
    KLS_Region_List t_regs;	/**< List of temporarily allocated Regions*/
    Koliseo *reglist_kls; /**< Reference to the supporting Koliseo when conf.tkls_reglist_alloc_backend is KLS_BASIC.*/
    int max_regions_kls_alloc_basic; /**< Contains maximum number of allocatable KLS_Region when using KLS_REGLIST_ALLOC_KLS_BASIC.*/
    KLS_Temp_Conf conf;	/**< Contains flags to change the Koliseo_Temp behaviour.*/
#endif
} Koliseo_Temp;

void kls_log(Koliseo * kls, const char *tag, const char *format, ...);
ptrdiff_t kls_get_pos(Koliseo * kls);

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
void *kls_pop(Koliseo * kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void *kls_pop_AR(Koliseo *kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void kls_dbg_features(void);
char* kls_strdup(Koliseo* kls, char* source);
char** kls_strdup_arr(Koliseo* kls, size_t count, char** source);
char* kls_t_strdup(Koliseo_Temp* t_kls, char* source);
char** kls_t_strdup_arr(Koliseo_Temp* t_kls, size_t count, char** source);

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
 * Macro used to request memory from a Koliseo.
 */
#define KLS_PUSH(kls, type) KLS_PUSH_ARR((kls), type, 1)

/**
 * Macro used to request memory from a Koliseo, and assign a name and a description to the region item.
 */
#define KLS_PUSH_NAMED(kls, type, name, desc) KLS_PUSH_ARR_NAMED((kls), type, 1, (name), (desc))

/**
 * Macro used to request memory from a Koliseo, and assign a a type, a name and a description to the region item.
 */
#define KLS_PUSH_TYPED(kls, type, region_type, name, desc) KLS_PUSH_ARR_TYPED((kls), type, 1, (region_type), (name), (desc))

/**
 * Macro used to "remove" memory from a Koliseo. Rewinds the pointer by the requested type and returns a pointer to that memory before updating the Koliseo index.
 * It's up to you to copy your item somewhere else before calling any PUSH operation again, as that memory should be overwritten.
 */
#define KLS_POP(kls, type) KLS_POP_ARR((kls), type, 1)

void kls_clear(Koliseo * kls);
void kls_free(Koliseo * kls);
void print_kls_2file(FILE * fp, Koliseo * kls);
void print_dbg_kls(Koliseo * kls);
void kls_formatSize(ptrdiff_t size, char *outputBuffer, size_t bufferSize);

#ifdef KOLISEO_HAS_CURSES /**< This definition controls the inclusion of ncurses functions.*/

#ifndef KOLISEO_CURSES_H_
#define KOLISEO_CURSES_H_

#ifndef WINDOWS_BUILD
#include "ncurses.h"
#else
#include <ncursesw/ncurses.h>
#endif				//WINDOWS_BUILD

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
void *kls_temp_pop(Koliseo_Temp * t_kls, ptrdiff_t size, ptrdiff_t align,
                   ptrdiff_t count);
void *kls_temp_pop_AR(Koliseo_Temp *t_kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void print_temp_kls_2file(FILE * fp, Koliseo_Temp * t_kls);
void print_dbg_temp_kls(Koliseo_Temp * t_kls);

/**
 * Macro used to request memory for an array of type values from a Koliseo_Temp.
 */
#define KLS_PUSH_ARR_T(kls_temp, type, count) (type*)kls_temp_push_zero_AR((kls_temp), sizeof(type), _Alignof(type), (count))

/**
 * Macro to request memory for a C string from a Koliseo_Temp.
 * @see KLS_STRDUP_T()
 */
#define KLS_PUSH_STR_T(kls_temp, cstr) KLS_PUSH_ARR_T((kls_temp), char, strlen((cstr))+1)

/*
 * Macro to dupe a C string to a passed Koliseo_Temp, returns a pointer to the allocated string.
 * Unsafe, do not use.
 * @see kls_t_strdup()
 */
#define KLS_STRDUP_T(t_kls, source) kls_t_strdup((t_kls), (source))

/**
 * Macro used to request memory for an array of type values from a Koliseo_Temp, and assign a name and a description to the region item.
 */
#ifdef KOLISEO_HAS_REGION
#define KLS_PUSH_ARR_T_NAMED(kls_temp, type, count, name, desc) (type*)kls_temp_push_zero_named((kls_temp), sizeof(type), _Alignof(type), (count), (name), (desc))
#else
#define KLS_PUSH_ARR_T_NAMED(kls_temp, type, count, name, desc) KLS_PUSH_ARR_T((kls_temp),(type),(count))
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
 * Macro used to request memory from a Koliseo_Temp.
 */
#define KLS_PUSH_T(kls_temp, type) KLS_PUSH_ARR_T((kls_temp), type, 1)

/**
 * Macro used to request memory from a Koliseo_Temp, and assign a name and a description to the region item.
 */
#define KLS_PUSH_T_NAMED(kls_temp, type, name, desc) KLS_PUSH_ARR_T_NAMED((kls_temp), type, 1, (name), (desc))
/**
 * Macro used to request memory from a Koliseo_Temp, and assign a type, a name and a description to the region item.
 */
#define KLS_PUSH_T_TYPED(kls_temp, type, region_type, name, desc) KLS_PUSH_ARR_T_TYPED((kls_temp), type, 1, (region_type), (name), (desc))

/**
 * Macro used to "remove" memory from a Koliseo_Temp. Rewinds the pointer by the requested type and returns a pointer to that memory before updating the Koliseo_Temp index.
 * It's up to you to copy your item somewhere else before calling any PUSH operation again, as that memory should be overwritten.
 */
#define KLS_POP_T(kls_temp, type) KLS_POP_ARR_T((kls_temp), type, 1)

#ifdef KOLISEO_HAS_REGION

KLS_Region_List kls_emptyList(void);
#define KLS_GETLIST() kls_emptyList()
bool kls_empty(KLS_Region_List);
KLS_list_element kls_head(KLS_Region_List);
KLS_Region_List kls_tail(KLS_Region_List);
KLS_Region_List kls_cons(Koliseo *, KLS_list_element, KLS_Region_List);
KLS_region_list_item* kls_list_pop(Koliseo *kls);
KLS_Region_List kls_t_cons(Koliseo_Temp *, KLS_list_element, KLS_Region_List);
KLS_region_list_item* kls_t_list_pop(Koliseo_Temp *t_kls);

void kls_freeList(KLS_Region_List);
#define KLS_FREELIST(kls_list) kls_freeList(kls_list)
void kls_showList(KLS_Region_List);
void kls_showList_toFile(KLS_Region_List, FILE * fp);
#define KLS_ECHOLIST(kls_list) kls_showList(kls_list)
#define KLS_PRINTLIST(kls_list,file) kls_showList_toFile(kls_list,file)
bool kls_member(KLS_list_element, KLS_Region_List);
int kls_length(KLS_Region_List);
KLS_Region_List kls_append(Koliseo *, KLS_Region_List, KLS_Region_List);
KLS_Region_List kls_reverse(Koliseo *, KLS_Region_List);
KLS_Region_List kls_copy(Koliseo *, KLS_Region_List);
KLS_Region_List kls_delete(Koliseo *, KLS_list_element, KLS_Region_List);

KLS_Region_List kls_insord(Koliseo *, KLS_list_element, KLS_Region_List);
#define KLS_PUSHLIST(kls,reg,kls_list) kls_insord(kls,reg,kls_list)
KLS_Region_List kls_insord_p(Koliseo *, KLS_list_element, KLS_Region_List);
#define KLS_PUSHLIST_P(kls,reg,kls_list) kls_insord_p(kls,reg,kls_list)
KLS_Region_List kls_mergeList(Koliseo *, KLS_Region_List, KLS_Region_List);
KLS_Region_List kls_intersect(Koliseo *, KLS_Region_List, KLS_Region_List);
KLS_Region_List kls_diff(Koliseo *, KLS_Region_List, KLS_Region_List);

#define KLS_DIFF(kls,kls_list1,kls_list2) kls_diff(kls,kls_list1,kls_list2)
bool kls_isLess(KLS_list_element, KLS_list_element);
bool kls_isEqual(KLS_list_element, KLS_list_element);
double kls_usageShare(KLS_list_element, Koliseo *);
ptrdiff_t kls_regionSize(KLS_list_element);
ptrdiff_t kls_avg_regionSize(Koliseo *);
void kls_usageReport_toFile(Koliseo *, FILE *);
void kls_usageReport(Koliseo *);
ptrdiff_t kls_type_usage(int, Koliseo *);

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

#else
#error "This code requires C11 or later.\n    _Alignof() is not available"
#endif // __STDC_VERSION__ && __STDC_VERSION__ >= 201112L //We need C11

#endif //KOLISEO_H_
