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

/*
#ifndef KOLISEO_HAS_LOCATE
#define KOLISEO_HAS_LOCATE // Used for enabling caller location arguments for some APIs.
#endif // KOLISEO_HAS_LOCATE
*/

#ifdef _WIN32
#include <profileapi.h>		//Used for QueryPerformanceFrequency(), QueryPerformanceCounter()
#endif
#endif //KLS_DEBUG_CORE

#ifdef KOLISEO_HAS_LOCATE
typedef struct Koliseo_Loc {
    const char* file;
    const int line;
    const char* func;
} Koliseo_Loc;

#define KLS_HERE (Koliseo_Loc){ \
    .file = __FILE__, \
    .line = __LINE__, \
    .func = __func__, \
}

/**
 * Defines a format string for Koliseo_Loc.
 * @see KLS_Loc_Arg()
 */
#define KLS_Loc_Fmt "[%s:%i at %s():]    "

/**
 * Defines a format macro for Koliseo_Loc args.
 * @see KLS_Loc_Fmt
 */
#define KLS_Loc_Arg(loc) (loc.file), (loc.line), (loc.func)
#endif // KOLISEO_HAS_LOCATE

#define KLS_MAJOR 0 /**< Represents current major release.*/
#define KLS_MINOR 4 /**< Represents current minor release.*/
#define KLS_PATCH 8 /**< Represents current patch release.*/

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
 * Defines current API version number from KLS_MAJOR, KLS_MINOR and KLS_PATCH.
 */
static const int KOLISEO_API_VERSION_INT =
    (KLS_MAJOR * 1000000 + KLS_MINOR * 10000 + KLS_PATCH * 100);
/**< Represents current version with numeric format.*/

/**
 * Defines current API version string.
 */
static const char KOLISEO_API_VERSION_STRING[] = "0.4.8"; /**< Represents current version with MAJOR.MINOR.PATCH format.*/

/**
 * Returns current koliseo version as a string.
 */
const char *string_koliseo_version(void);

/**
 * Returns current koliseo version as an integer.
 */
int int_koliseo_version(void);

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
struct Koliseo;             //Forward declaration for KLS_OOM_Handler

#ifndef KOLISEO_HAS_LOCATE
typedef void(KLS_OOM_Handler)(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, ptrdiff_t count); /**< Used to pass an error handler for Out-Of-Memory error.*/
#else
typedef void(KLS_OOM_Handler)(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, ptrdiff_t count, Koliseo_Loc loc); /**< Used to pass an error handler for Out-Of-Memory error.*/
#endif

#ifndef KOLISEO_HAS_LOCATE
typedef void(KLS_PTRDIFF_MAX_Handler)(struct Koliseo* kls, ptrdiff_t size, ptrdiff_t count); /**< Used to pass an error handler for count > (PTRDIFF_MAX / size) error.*/
#else
typedef void(KLS_PTRDIFF_MAX_Handler)(struct Koliseo* kls, ptrdiff_t size, ptrdiff_t count, Koliseo_Loc loc); /**< Used to pass an error handler for count > (PTRDIFF_MAX / size) error.*/
#endif

#ifndef KOLISEO_HAS_LOCATE
typedef void(KLS_ZEROCOUNT_Handler)(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size); /**< Used to pass an error handler for zero-count push call error.*/
#else
typedef void(KLS_ZEROCOUNT_Handler)(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, Koliseo_Loc loc); /**< Used to pass an error handler for zero-count push call error.*/
#endif

#ifndef KOLISEO_HAS_LOCATE
void KLS_OOM_default_handler__(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, ptrdiff_t count); /**< Used by default when no handler is passed.*/
#else
void KLS_OOM_default_handler_dbg__(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, ptrdiff_t count, Koliseo_Loc loc); /**< Used by default when no handler is passed.*/
#endif

#ifndef KOLISEO_HAS_LOCATE
void KLS_PTRDIFF_MAX_default_handler__(struct Koliseo* kls, ptrdiff_t size, ptrdiff_t count); /**< Used by default when no handler is passed.*/
#else
void KLS_PTRDIFF_MAX_default_handler_dbg__(struct Koliseo* kls, ptrdiff_t size, ptrdiff_t count, Koliseo_Loc loc); /**< Used by default when no handler is passed.*/
#endif

#ifndef KOLISEO_HAS_LOCATE
void KLS_ZEROCOUNT_default_handler__(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size); /**< Used by default when no handler is passed.*/
#else
void KLS_ZEROCOUNT_default_handler_dbg__(struct Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, Koliseo_Loc loc); /**< Used by default when no handler is passed.*/
#endif

/**
 * Defines the handlers used for errors in push calls.
 * @see KLS_Conf
 */
typedef struct KLS_Err_Handlers {
    KLS_OOM_Handler* OOM_handler; /**< Pointer to handler for Out-Of-Memory errors in push calls.*/
    KLS_PTRDIFF_MAX_Handler* PTRDIFF_MAX_handler; /**< Pointer to handler for count > (PTRDIFF_MAX / size) errors in push calls.*/
    KLS_ZEROCOUNT_Handler* ZEROCOUNT_handler; /**< Pointer to handler for zero-count errors in push calls.*/
} KLS_Err_Handlers;

#ifndef KOLISEO_HAS_LOCATE
#define KLS_DEFAULT_ERR_HANDLERS (KLS_Err_Handlers) { .OOM_handler = &KLS_OOM_default_handler__, .PTRDIFF_MAX_handler = &KLS_PTRDIFF_MAX_default_handler__, .ZEROCOUNT_handler = &KLS_ZEROCOUNT_default_handler__, }
#else
#define KLS_DEFAULT_ERR_HANDLERS (KLS_Err_Handlers) { .OOM_handler = &KLS_OOM_default_handler_dbg__, .PTRDIFF_MAX_handler = &KLS_PTRDIFF_MAX_default_handler_dbg__, .ZEROCOUNT_handler = &KLS_ZEROCOUNT_default_handler_dbg__, }
#endif // KOLISEO_HAS_LOCATE

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
    int kls_block_while_has_temp; /**< If set to 1, make the Koliseo reject push calls while it has an open Koliseo_Temp.*/
    int kls_allow_zerocount_push; /**< If set to 1, make the Koliseo accept push calls with a count of 0.*/
    KLS_Err_Handlers err_handlers; /**< Used to pass custom error handlers for push calls.*/
} KLS_Conf;

KLS_Conf kls_conf_init_handled(int autoset_regions, int alloc_backend, ptrdiff_t reglist_kls_size, int autoset_temp_regions, int collect_stats, int verbose_lvl, int block_while_has_temp, int allow_zerocount_push, FILE* log_fp, const char* log_filepath, KLS_Err_Handlers err_handlers);

KLS_Conf kls_conf_init(int autoset_regions, int alloc_backend, ptrdiff_t reglist_kls_size, int autoset_temp_regions, int collect_stats, int verbose_lvl, int block_while_has_temp, int allow_zerocount_push, FILE* log_fp, const char* log_filepath);

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
#define KLS_Conf_Fmt "KLS_Conf { autoset_regions: %i, reglist_backend: %s, reglist_kls_size: %li, autoset_temp_regions: %i, collect_stats: %i, verbose_lvl: %i, log_filepath: \"%s\", log_fp: %p, block_while_has_temp: %i, allow_zerocount_push: %i }"
#else
#define KLS_Conf_Fmt "KLS_Conf { autoset_regions: %i, reglist_backend: %s, reglist_kls_size: %lli, autoset_temp_regions: %i, collect_stats: %i, verbose_lvl: %i, log_filepath: \"%s\", log_fp: %p, block_while_has_temp: %i, allow_zerocount_push: %i }"
#endif
#else

#define KLS_Conf_Fmt "KLS_Conf { collect_stats: %i, verbose_lvl: %i, log_filepath: \"%s\", log_fp: %p, block_while_has_temp: %i, allow_zerocount_push: %i }"
#endif // KOLISEO_HAS_REGION

/**
 * Defines a format macro for KLS_Conf args.
 * @see KLS_Conf_Fmt
 */
#ifdef KOLISEO_HAS_REGION
#define KLS_Conf_Arg(conf) (conf.kls_autoset_regions),kls_reglist_backend_string((conf.kls_reglist_alloc_backend)),(conf.kls_reglist_kls_size),(conf.kls_autoset_temp_regions),(conf.kls_collect_stats),(conf.kls_verbose_lvl),(conf.kls_log_filepath),(void*)(conf.kls_log_fp),(conf.kls_block_while_has_temp),(conf.kls_allow_zerocount_push)
#else
#define KLS_Conf_Arg(conf) (conf.kls_collect_stats),(conf.kls_verbose_lvl),(conf.kls_log_filepath),(void*)(conf.kls_log_fp),(conf.kls_block_while_has_temp),(conf.kls_allow_zerocount_push)
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
ptrdiff_t kls_get_pos(const Koliseo * kls);

#ifdef KOLISEO_HAS_REGION
int kls_get_maxRegions_KLS_BASIC(Koliseo * kls);
int kls_temp_get_maxRegions_KLS_BASIC(Koliseo_Temp * t_kls);
#endif

#ifndef KOLISEO_HAS_LOCATE
Koliseo *kls_new_alloc(ptrdiff_t size, kls_alloc_func alloc_func);
#else
Koliseo *kls_new_alloc_dbg(ptrdiff_t size, kls_alloc_func alloc_func, Koliseo_Loc loc);
#define kls_new_alloc(size, alloc_func) kls_new_alloc_dbg((size), (alloc_func), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE

#ifndef KLS_DEFAULT_ALLOCF
#define KLS_DEFAULT_ALLOCF malloc /**< Defines the default allocation function.*/
#endif

#define kls_new(size) kls_new_alloc((size), KLS_DEFAULT_ALLOCF)
//bool kls_set_conf(Koliseo* kls, KLS_Conf conf);
Koliseo *kls_new_conf_alloc(ptrdiff_t size, KLS_Conf conf, kls_alloc_func alloc_func);
#define kls_new_conf(size, conf) kls_new_conf_alloc((size), (conf), KLS_DEFAULT_ALLOCF)
Koliseo *kls_new_traced_alloc_handled(ptrdiff_t size, const char *output_path, kls_alloc_func alloc_func, KLS_Err_Handlers err_handlers);
Koliseo *kls_new_traced_alloc(ptrdiff_t size, const char *output_path, kls_alloc_func alloc_func);
#define kls_new_traced(size, output_path) kls_new_traced_alloc((size), (output_path), KLS_DEFAULT_ALLOCF)
#define kls_new_traced_handled(size, output_path, err_handlers) kls_new_traced_alloc_handled((size), (output_path), KLS_DEFAULT_ALLOCF, (err_handlers))
Koliseo *kls_new_dbg_alloc_handled(ptrdiff_t size, kls_alloc_func alloc_func, KLS_Err_Handlers err_handlers);
Koliseo *kls_new_dbg_alloc(ptrdiff_t size, kls_alloc_func alloc_func);
#define kls_new_dbg(size) kls_new_dbg_alloc((size), KLS_DEFAULT_ALLOCF)
#define kls_new_dbg_handled(size, err_handlers) kls_new_dbg_alloc_handled((size), KLS_DEFAULT_ALLOCF,(err_handlers))
Koliseo *kls_new_traced_AR_KLS_alloc_handled(ptrdiff_t size, const char *output_path,
        ptrdiff_t reglist_kls_size, kls_alloc_func alloc_func, KLS_Err_Handlers err_handlers);
Koliseo *kls_new_traced_AR_KLS_alloc(ptrdiff_t size, const char *output_path,
                                     ptrdiff_t reglist_kls_size, kls_alloc_func alloc_func);
#define kls_new_traced_AR_KLS(size, output_path, reglist_kls_size) kls_new_traced_AR_KLS_alloc((size), (output_path), (reglist_kls_size), KLS_DEFAULT_ALLOCF)
#define kls_new_traced_AR_KLS_handled(size, output_path, reglist_kls_size, err_handlers) kls_new_traced_AR_KLS_alloc_handled((size), (output_path), (reglist_kls_size), KLS_DEFAULT_ALLOCF, (err_handlers))

//void* kls_push(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);

#ifndef KOLISEO_HAS_LOCATE
void *kls_push_zero(Koliseo * kls, ptrdiff_t size, ptrdiff_t align,
                    ptrdiff_t count);
#else
void *kls_push_zero_dbg(Koliseo * kls, ptrdiff_t size, ptrdiff_t align,
                        ptrdiff_t count, Koliseo_Loc loc);

#define kls_push_zero(kls, size, align, count) kls_push_zero_dbg((kls), (size), (align), (count), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE

#ifndef KOLISEO_HAS_LOCATE
void *kls_push_zero_AR(Koliseo * kls, ptrdiff_t size, ptrdiff_t align,
                       ptrdiff_t count);
#else
void *kls_push_zero_AR_dbg(Koliseo * kls, ptrdiff_t size, ptrdiff_t align,
                           ptrdiff_t count, Koliseo_Loc loc);
#define kls_push_zero_AR(kls, size, align, count) kls_push_zero_AR_dbg((kls), (size), (align), (count), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE

#ifdef KOLISEO_HAS_REGION

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
#define KLS_PUSH_STR_NAMED(kls, cstr, name, desc) KLS_PUSH_ARR_NAMED((kls), char, strlen((cstr))+1, (name), (desc))

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
#define KLS_PUSH_STR_TYPED(kls, cstr, region_type, name, desc) KLS_PUSH_ARR_TYPED((kls), char, strlen((cstr))+1, (region_type), (name), (desc))

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

#ifndef KOLISEO_HAS_LOCATE
Koliseo_Temp *kls_temp_start(Koliseo * kls);
#else
Koliseo_Temp *kls_temp_start_dbg(Koliseo * kls, Koliseo_Loc loc);
#define kls_temp_start(kls) kls_temp_start_dbg((kls), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE
//bool kls_temp_set_conf(Koliseo_Temp* t_kls, KLS_Temp_Conf conf);
void kls_temp_end(Koliseo_Temp * tmp_kls);

#ifndef KOLISEO_HAS_LOCATE
void *kls_temp_push_zero_AR(Koliseo_Temp * t_kls, ptrdiff_t size,
                            ptrdiff_t align, ptrdiff_t count);
#else
void *kls_temp_push_zero_AR_dbg(Koliseo_Temp * t_kls, ptrdiff_t size,
                                ptrdiff_t align, ptrdiff_t count, Koliseo_Loc loc);
#define kls_temp_push_zero_AR(t_kls, size, align, count) kls_temp_push_zero_AR_dbg((t_kls), (size), (align), (count), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE

#ifdef KOLISEO_HAS_REGION
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
#define KLS_PUSH_STR_T_NAMED(kls_temp, cstr, name, desc) KLS_PUSH_ARR_T_NAMED((kls_temp), char, strlen((cstr))+1, (name), (desc))

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
#define KLS_PUSH_STR_T_TYPED(kls_temp, cstr, region_type, name, desc) KLS_PUSH_ARR_T_TYPED((kls_temp), char, strlen((cstr))+1, (region_type), (name), (desc))

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

KLS_Region_List kls_rl_emptyList(void);
#define KLS_RL_GETLIST() kls_rl_emptyList()
bool kls_rl_empty(KLS_Region_List);
KLS_list_element kls_rl_head(KLS_Region_List);
KLS_Region_List kls_rl_tail(KLS_Region_List);
KLS_Region_List kls_rl_cons(Koliseo *, KLS_list_element, KLS_Region_List);
#ifdef KOLISEO_HAS_EXPER
KLS_region_list_item* kls_rl_list_pop(Koliseo *kls);
#endif // KOLISEO_HAS_EXPER
KLS_Region_List kls_rl_t_cons(Koliseo_Temp *, KLS_list_element, KLS_Region_List);
#ifdef KOLISEO_HAS_EXPER
KLS_region_list_item* kls_rl_t_list_pop(Koliseo_Temp *t_kls);
#endif // KOLISEO_HAS_EXPER

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
#define KLS_RL_PUSHLIST_P(kls,reg,kls_list) kls_rl_insord_p(kls,reg,kls_list)
KLS_Region_List kls_rl_mergeList(Koliseo *, KLS_Region_List, KLS_Region_List);
KLS_Region_List kls_rl_intersect(Koliseo *, KLS_Region_List, KLS_Region_List);
KLS_Region_List kls_rl_diff(Koliseo *, KLS_Region_List, KLS_Region_List);

#define KLS_RL_DIFF(kls,kls_list1,kls_list2) kls_rl_diff(kls,kls_list1,kls_list2)
bool kls_rl_isLess(KLS_list_element, KLS_list_element);
bool kls_rl_isEqual(KLS_list_element, KLS_list_element);
double kls_usageShare(KLS_list_element, Koliseo *);
ptrdiff_t kls_regionSize(KLS_list_element);
ptrdiff_t kls_avg_regionSize(Koliseo *);
void kls_usageReport_toFile(Koliseo *, FILE *);
void kls_usageReport(Koliseo *);
ptrdiff_t kls_type_usage(int, Koliseo *);
ptrdiff_t kls_total_padding(Koliseo *);

#endif // KOLISEO_HAS_REGION

#ifdef KOLISEO_HAS_GULP /**< This definition controls the inclusion of gulp functions.*/

/**
 * KOLISEO_HAS_GULP is deprecated. The feature will be moved to a separate header in an upcoming release.
 */
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
#define KLS__STRCPY(dest, source) do {\
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
