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

#ifndef KOLISEO_H_

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) //We need C11
#define KLS_ALIGNOF _Alignof
#elif defined(__cplusplus)
#define KLS_ALIGNOF alignof
#else
#error "This code requires C11 or later.\n    _Alignof() is not available"
#endif // __STDC_VERSION__ && __STDC_VERSION__ >= 201112L //We need C11

#define KOLISEO_H_

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

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
#define KLS_MINOR 5 /**< Represents current minor release.*/
#define KLS_PATCH 10 /**< Represents current patch release.*/

typedef void*(kls_alloc_func)(size_t); /**< Used to select an allocation function for the arena's backing memory.*/
typedef void(kls_free_func)(void*); /**< Used to select a free function for the arena's backing memory.*/

#define STRINGIFY_2(x) #x

#define STRINGIFY(x) STRINGIFY_2(x)

#define KLS_MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 * Defines current API version number from KLS_MAJOR, KLS_MINOR and KLS_PATCH.
 */
static const int KOLISEO_API_VERSION_INT =
    (KLS_MAJOR * 1000000 + KLS_MINOR * 10000 + KLS_PATCH * 100);
/**< Represents current version with numeric format.*/

/**
 * Defines current API version string.
 */
static const char KOLISEO_API_VERSION_STRING[] = "0.5.10"; /**< Represents current version with MAJOR.MINOR.PATCH format.*/

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

typedef void(KLS_hook_on_new)(struct Koliseo* kls); /**< Used to pass an extension handler for kls_new_alloc().*/

typedef void(KLS_hook_on_free)(struct Koliseo* kls); /**< Used to pass an extension handler for kls_free().*/

typedef void(KLS_hook_on_push)(struct Koliseo* kls, ptrdiff_t padding, const char* caller, void* user); /**< Used to pass an extension handler for kls_push().*/

typedef void(KLS_hook_on_temp_start)(struct Koliseo_Temp* t_kls); /**< Used to pass an extension handler for kls_temp_start().*/

typedef void(KLS_hook_on_temp_free)(struct Koliseo_Temp* t_kls); /**< Used to pass an extension handler for kls_temp_end().*/

typedef void(KLS_hook_on_temp_push)(struct Koliseo_Temp* t_kls, ptrdiff_t padding, const char* caller, void* user); /**< Used to pass an extension handler for kls_temp_push().*/

typedef struct KLS_Hooks {
    KLS_hook_on_new* on_new_handler; /**< Used to pass custom new handler for kls_new_alloc calls.*/
    KLS_hook_on_free* on_free_handler; /**< Used to pass custom free handler for kls_free calls.*/
    KLS_hook_on_push* on_push_handler; /**< Used to pass custom push handler for kls_push calls.*/
    KLS_hook_on_temp_start* on_temp_start_handler; /**< Used to pass custom start handler for kls_temp_start calls.*/
    KLS_hook_on_temp_free* on_temp_free_handler; /**< Used to pass custom free handler for kls_temp_end calls.*/
    KLS_hook_on_temp_push* on_temp_push_handler; /**< Used to pass custom push handler for kls_temp_push calls.*/
} KLS_Hooks;

/**
 * Defines default hooks that are loaded on kls_new() variants lacking explicit set of KLS_Hooks.
 * Useful to be redefined by an extension file, together with KLS_DEFAULT_EXTENSION_DATA.
 * @see KLS_DEFAULT_EXTENSION_DATA
 * @see KLS_DEFAULT_EXTENSIONS_LEN
 * @see KLS_Hooks
 */
#ifndef KLS_DEFAULT_HOOKS
#define KLS_DEFAULT_HOOKS &(KLS_Hooks){0}
#endif // KLS_DEFAULT_HOOKS

/**
 * Defines default hooks that are loaded on kls_new() variants lacking explicit set of KLS_Hooks.
 * Useful to be redefined by an extension file, together with KLS_DEFAULT_HOOKS.
 * @see KLS_DEFAULT_HOOKS
 * @see KLS_DEFAULT_EXTENSIONS_LEN
 */
#ifndef KLS_DEFAULT_EXTENSION_DATA
#define KLS_DEFAULT_EXTENSION_DATA NULL
#endif // KLS_DEFAULT_EXTENSION_DATA

/**
 * Defines flags for Koliseo.
 * @see Koliseo
 */
typedef struct KLS_Conf {
    int kls_collect_stats; /**< If set to 1, make the Koliseo collect performance stats.*/
    int kls_verbose_lvl; /**< If > 0, makes the Koliseo try to acquire kls_log_fp from kls_log_filepath.*/
    FILE *kls_log_fp; /**< FILE pointer used by the Koliseo to print its kls_log() output.*/
    const char *kls_log_filepath; /**< String representing the path to the Koliseo logfile.*/
    int kls_block_while_has_temp; /**< If set to 1, make the Koliseo reject push calls while it has an open Koliseo_Temp.*/
    int kls_allow_zerocount_push; /**< If set to 1, make the Koliseo accept push calls with a count of 0.*/
    int kls_growable; /**< If set to 1, make the Koliseo grow when a out of memory for a push call.*/
    KLS_Err_Handlers err_handlers; /**< Used to pass custom error handlers for push calls.*/
} KLS_Conf;

KLS_Conf kls_conf_init_handled(int collect_stats, int verbose_lvl, int block_while_has_temp, int allow_zerocount_push, int growable, FILE* log_fp, const char* log_filepath, KLS_Err_Handlers err_handlers);

KLS_Conf kls_conf_init(int collect_stats, int verbose_lvl, int block_while_has_temp, int allow_zerocount_push, int growable, FILE* log_fp, const char* log_filepath);

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
#define KLS_Conf_Fmt "KLS_Conf { collect_stats: %i, verbose_lvl: %i, log_filepath: \"%s\", log_fp: %p, block_while_has_temp: %i, allow_zerocount_push: %i }"

/**
 * Defines a format macro for KLS_Conf args.
 * @see KLS_Conf_Fmt
 */
#define KLS_Conf_Arg(conf) (conf.kls_collect_stats),(conf.kls_verbose_lvl),(conf.kls_log_filepath),(void*)(conf.kls_log_fp),(conf.kls_block_while_has_temp),(conf.kls_allow_zerocount_push)

/**
 * Defines a format string for KLS_Stats.
 * @see KLS_Stats_Arg()
 */
#ifdef KLS_DEBUG_CORE
#define KLS_Stats_Fmt "KLS_Stats { tot_pushes: %i, tot_pops: %i, tot_temp_pushes: %i, tot_temp_pops: %i, tot_hiccups: %i, worst_push_time: %.7f }"
#else
#define KLS_Stats_Fmt "KLS_Stats { tot_pushes: %i, tot_pops: %i, tot_temp_pushes: %i, tot_temp_pops: %i, tot_hiccups: %i }"
#endif // KLS_DEBUG_CORE

/**
 * Defines a format macro for KLS_Stats args.
 * @see KLS_Stats_Fmt
 */
#ifdef KLS_DEBUG_CORE
#define KLS_Stats_Arg(stats) (stats.tot_pushes),(stats.tot_pops),(stats.tot_temp_pushes),(stats.tot_temp_pops),(stats.tot_hiccups),(stats.worst_pushcall_time)
#else
#define KLS_Stats_Arg(stats) (stats.tot_pushes),(stats.tot_pops),(stats.tot_temp_pushes),(stats.tot_temp_pops),(stats.tot_hiccups)
#endif // KLS_DEBUG_CORE

/**
 * DEPRECATED: Support for multiple extension will be dropped in the next release.
 * Defines how many extensions can be handled at once.
 * @see KLS_Hooks
 * @see Koliseo
 */
#ifndef KLS_MAX_EXTENSIONS
#define KLS_MAX_EXTENSIONS 1
#endif // KLS_MAX_EXTENSIONS

/**
 * DEPRECATED: Support for multiple extension will be dropped in the next release.
 * Defines how many extensions are loaded on kls_new() variants lacking explicit set of KLS_Hooks.
 * Useful to be redefined by an extension file, together with KLS_DEFAULT_HOOKS.
 * @see KLS_DEFAULT_HOOKS
 * @see KLS_Hooks
 * @see Koliseo
 */
#ifndef KLS_DEFAULT_EXTENSIONS_LEN
#define KLS_DEFAULT_EXTENSIONS_LEN 0
#endif // KLS_MAX_EXTENSIONS

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
    int has_temp;     /**< When == 1, a Koliseo_Temp is currently active on this Koliseo.*/
    KLS_Conf conf; /**< Contains flags to change the Koliseo behaviour.*/
    KLS_Stats stats; /**< Contains stats for Koliseo performance analysis.*/
    struct Koliseo_Temp *t_kls;	    /**< Points to related active Kolieo_Temp, when has_temp == 1.*/
    KLS_Hooks hooks[KLS_MAX_EXTENSIONS];  /**< Contains handlers for extensions.*/
    void* extension_data[KLS_MAX_EXTENSIONS]; /**< Points to data for extensions.*/
    size_t hooks_len; /**< Length for hooks and extension_data.*/
    kls_free_func* free_func; /**< Points to the free function for the arena's backing memory.*/
    struct Koliseo* next; /**< Points to the next Koliseo when conf.kls_growable == 1.*/
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
} Koliseo_Temp;

/**
 * Defines the result for kls__check_available_failable()
 * @see kls__check_available_failable
 * @see KLS_Push_Result
 */
typedef enum KLS_Push_Error {
    KLS_PUSH_OK,
    KLS_PUSH_SIZE_LT1,
    KLS_PUSH_ALIGN_LT1,
    KLS_PUSH_ALIGN_NOT_POW2,
    KLS_PUSH_NEGATIVE_COUNT,
    KLS_PUSH_ZEROCOUNT,
    KLS_PUSH_WITH_TEMP_ACTIVE,
    KLS_PUSH_PTRDIFF_MAX,
    KLS_PUSH_OOM,
} KLS_Push_Error;

/**
 * Defines the result for kls__advance() and kls__temp_advance().
 * @see kls__advance
 * @see kls__temp_advance
 */
typedef struct KLS_Push_Result {
    void* p;
    KLS_Push_Error error;
} KLS_Push_Result;

void kls_log(Koliseo * kls, const char *tag, const char *format, ...);
ptrdiff_t kls_get_pos(const Koliseo * kls);

#ifndef KOLISEO_HAS_LOCATE
Koliseo *kls_new_alloc_ext(ptrdiff_t size, kls_alloc_func alloc_func, kls_free_func free_func, KLS_Hooks* ext_handlers, void** user, size_t ext_len);
#else
Koliseo *kls_new_alloc_ext_dbg(ptrdiff_t size, kls_alloc_func alloc_func, kls_free_func free_func, KLS_Hooks* ext_handlers, void** user, size_t ext_len, Koliseo_Loc loc);
#define kls_new_alloc_ext(size, alloc_func, free_func, ext_handlers, user, ext_len) kls_new_alloc_ext_dbg((size), (alloc_func), (free_func), (ext_handlers), (user), (ext_len), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE

#ifndef KOLISEO_HAS_LOCATE
Koliseo *kls_new_alloc(ptrdiff_t size, kls_alloc_func alloc_func, kls_free_func free_func);
#else
Koliseo *kls_new_alloc_dbg(ptrdiff_t size, kls_alloc_func alloc_func, kls_free_func free_func, Koliseo_Loc loc);
#define kls_new_alloc(size, alloc_func, free_func) kls_new_alloc_dbg((size), (alloc_func), (free_func), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE

#ifndef KLS_DEFAULT_ALLOCF
#define KLS_DEFAULT_ALLOCF malloc /**< Defines the default allocation function.*/
#endif

#ifndef KLS_DEFAULT_FREEF
#define KLS_DEFAULT_FREEF free /**< Defines the default free function.*/
#endif

Koliseo* kls_new(ptrdiff_t size);
//bool kls_set_conf(Koliseo* kls, KLS_Conf conf);
Koliseo *kls_new_conf_alloc_ext(ptrdiff_t size, KLS_Conf conf, kls_alloc_func alloc_func, kls_free_func free_func, KLS_Hooks* ext_handlers, void** user, size_t ext_len);
Koliseo *kls_new_conf_alloc(ptrdiff_t size, KLS_Conf conf, kls_alloc_func alloc_func, kls_free_func kls_free_func);
Koliseo *kls_new_conf_ext(ptrdiff_t size, KLS_Conf conf, KLS_Hooks* ext_handlers, void** user, size_t ext_len);
Koliseo *kls_new_conf(ptrdiff_t size, KLS_Conf conf);

Koliseo *kls_new_traced_alloc_handled_ext(ptrdiff_t size, const char *output_path, kls_alloc_func alloc_func, kls_free_func free_func, KLS_Err_Handlers err_handlers, KLS_Hooks* ext_handlers, void** user, size_t ext_len);
Koliseo *kls_new_traced_alloc_handled(ptrdiff_t size, const char *output_path, kls_alloc_func alloc_func, kls_free_func free_func, KLS_Err_Handlers err_handlers);
Koliseo *kls_new_traced_ext(ptrdiff_t size, const char *output_path, KLS_Hooks* ext_handlers, void** user, size_t ext_len);
Koliseo *kls_new_traced_alloc(ptrdiff_t size, const char *output_path, kls_alloc_func alloc_func, kls_free_func free_func);
Koliseo *kls_new_traced(ptrdiff_t size, const char* output_path);
Koliseo *kls_new_traced_handled(ptrdiff_t size, const char* output_path, KLS_Err_Handlers err_handlers);

Koliseo *kls_new_dbg_alloc_handled_ext(ptrdiff_t size, kls_alloc_func alloc_func, kls_free_func free_func, KLS_Err_Handlers err_handlers, KLS_Hooks* ext_handlers, void** user, size_t ext_len);
Koliseo *kls_new_dbg_alloc_handled(ptrdiff_t size, kls_alloc_func alloc_func, kls_free_func free_func, KLS_Err_Handlers err_handlers);
Koliseo *kls_new_dbg_ext(ptrdiff_t size, KLS_Hooks* ext_handlers, void** user, size_t ext_len);
Koliseo *kls_new_dbg_alloc(ptrdiff_t size, kls_alloc_func alloc_func, kls_free_func free_func);
Koliseo *kls_new_dbg(ptrdiff_t size);
Koliseo *kls_new_dbg_handled(ptrdiff_t size, KLS_Err_Handlers err_handlers);

#ifndef KOLISEO_HAS_LOCATE
void* kls__handle_push_result(Koliseo* kls, KLS_Push_Result r, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, ptrdiff_t padding, const char* caller_name);
#else
void* kls__handle_push_result_dbg(Koliseo* kls, KLS_Push_Result r, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, ptrdiff_t padding, const char* caller_name, Koliseo_Loc loc);
#endif // KOLISEO_HAS_LOCATE

#ifndef KOLISEO_HAS_LOCATE
KLS_Push_Result kls__advance(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, ptrdiff_t* padding, const char* caller_name);
#else
KLS_Push_Result kls__advance_dbg(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, ptrdiff_t* padding, const char* caller_name, Koliseo_Loc loc);
#endif // KOLISEO_HAS_LOCATE

#ifndef KOLISEO_HAS_LOCATE
KLS_Push_Result kls__temp_advance(Koliseo_Temp* t_kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, ptrdiff_t* padding, const char* caller_name);
#else
KLS_Push_Result kls__temp_advance_dbg(Koliseo_Temp* t_kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, ptrdiff_t* padding, const char* caller_name, Koliseo_Loc loc);
#endif // KOLISEO_HAS_LOCATE

#ifndef KOLISEO_HAS_LOCATE
KLS_Push_Error kls__check_available_failable(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, const char* caller_name);
#else
KLS_Push_Error kls__check_available_failable_dbg(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, const char* caller_name, Koliseo_Loc loc);
#endif // KOLISEO_HAS_LOCATE

/**
 * DEPRECATED: Support for this macro will be dropped in the next release.
 * Macro to return NULL on errors from kls__check_available_failable.
 * @see kls__check_available_failable
 */
#ifndef KOLISEO_HAS_LOCATE
#define kls__check_available(kls, size, align, count) do { \
    int res = kls__check_available_failable((kls), (size), (align), (count), __func__); \
    if (res != 0) return NULL; \
} while(0)
#else
#define kls__check_available_dbg(kls, size, align, count, loc) do { \
    int res = kls__check_available_failable_dbg((kls), (size), (align), (count), __func__, (loc)); \
    if (res != 0) return NULL; \
} while(0)
#endif // KOLISEO_HAS_LOCATE

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
void *kls_push_zero_ext(Koliseo * kls, ptrdiff_t size, ptrdiff_t align,
                        ptrdiff_t count);
#else
void *kls_push_zero_ext_dbg(Koliseo * kls, ptrdiff_t size, ptrdiff_t align,
                            ptrdiff_t count, Koliseo_Loc loc);
#define kls_push_zero_ext(kls, size, align, count) kls_push_zero_ext_dbg((kls), (size), (align), (count), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE

#ifndef KOLISEO_HAS_LOCATE
char* kls_vsprintf(Koliseo* kls, const char* fmt, va_list args);
#else
char* kls_vsprintf_dbg(Koliseo* kls, Koliseo_Loc loc, const char* fmt, va_list args);
#define kls_vsprintf(kls, fmt, args) kls_vsprintf_dbg((kls), KLS_HERE, (fmt), (args))
#endif // KOLISEO_HAS_LOCATE

#ifndef KOLISEO_HAS_LOCATE
char* kls_sprintf(Koliseo* kls, const char* fmt, ...);
#else
char* kls_sprintf_dbg(Koliseo* kls, Koliseo_Loc loc, const char* fmt, ...);
#define kls_sprintf(kls, fmt, ...) kls_sprintf_dbg((kls), KLS_HERE, (fmt), __VA_ARGS__)
#endif // KOLISEO_HAS_LOCATE

#ifndef KOLISEO_HAS_LOCATE
void *kls_repush(Koliseo *kls, void* old, ptrdiff_t size, ptrdiff_t align,
                 ptrdiff_t old_count, ptrdiff_t new_count);
#else
void *kls_repush_dbg(Koliseo *kls, void* old, ptrdiff_t size, ptrdiff_t align,
                     ptrdiff_t old_count, ptrdiff_t new_count, Koliseo_Loc loc);
#define kls_repush(kls, old, size, align, old_count, new_count) kls_repush_dbg((kls), (old), (size), (align), (old_count), (new_count), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE

#ifndef KOLISEO_HAS_LOCATE
void *kls_temp_repush(Koliseo_Temp *t_kls, void* old, ptrdiff_t size, ptrdiff_t align,
                      ptrdiff_t old_count, ptrdiff_t new_count);
#else
void *kls_temp_repush_dbg(Koliseo_Temp *t_kls, void* old, ptrdiff_t size, ptrdiff_t align,
                          ptrdiff_t old_count, ptrdiff_t new_count, Koliseo_Loc loc);
#define kls_temp_repush(t_kls, old, size, align, old_count, new_count) kls_temp_repush_dbg((t_kls), (old), (size), (align), (old_count), (new_count), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE

/**
 * Macro used to request memory for an array of type values from a Koliseo.
 */
#define KLS_PUSH_ARR(kls, type, count) (type*)kls_push_zero_ext((kls), sizeof(type), KLS_ALIGNOF(type), (count))

/**
 * Macro used to format a cstring into a Koliseo.
 */
#define KLS_SPRINTF(kls, fmt, ...) kls_sprintf((kls), (fmt), __VA_ARGS__)

/**
 * Macro to request memory for a C string from a Koliseo.
 * @see KLS_STRDUP()
 */
#define KLS_PUSH_STR(kls, cstr) KLS_PUSH_ARR((kls), char, strlen((cstr))+1)

/**
 * Macro used to repush memory for dinamic arrays from a Koliseo.
 */
#define KLS_REPUSH(kls, old, type, old_count, new_count) (type*)kls_repush((kls), (old), sizeof(type), KLS_ALIGNOF(type), (old_count), (new_count))

/**
 * Macro used to repush memory for dinamic arrays from a Koliseo_Temp.
 */
#define KLS_REPUSH_T(t_kls, old, type, old_count, new_count) (type*)kls_temp_repush((t_kls), (old), sizeof(type), KLS_ALIGNOF(type), (old_count), (new_count))

/**
 * Macro used to request memory for an array of type values from a Koliseo, and assign a name and a description to the region item.
 */
#define KLS_PUSH_ARR_NAMED(kls, type, count, name, desc) KLS_PUSH_ARR((kls),type,(count))

/**
 * Macro to request memory for a C string from a Koliseo, and assign a name and a description to the region item.
 */
#define KLS_PUSH_STR_NAMED(kls, cstr, name, desc) KLS_PUSH_ARR_NAMED((kls), char, strlen((cstr))+1, (name), (desc))

/**
 * Macro used to request memory for an array of type values from a Koliseo, and assign a type, a name and a description to the region item.
 */
#define KLS_PUSH_ARR_TYPED(kls, type, count, region_type, name, desc) KLS_PUSH_ARR((kls),type,(count))

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
void *kls_temp_push_zero_ext(Koliseo_Temp * t_kls, ptrdiff_t size,
                             ptrdiff_t align, ptrdiff_t count);
#else
void *kls_temp_push_zero_ext_dbg(Koliseo_Temp * t_kls, ptrdiff_t size,
                                 ptrdiff_t align, ptrdiff_t count, Koliseo_Loc loc);
#define kls_temp_push_zero_ext(t_kls, size, align, count) kls_temp_push_zero_ext_dbg((t_kls), (size), (align), (count), KLS_HERE)
#endif // KOLISEO_HAS_LOCATE

#ifndef KOLISEO_HAS_LOCATE
char* kls_temp_vsprintf(Koliseo_Temp* kls_t, const char* fmt, va_list args);
#else
char* kls_temp_vsprintf_dbg(Koliseo_Temp* kls_t, Koliseo_Loc loc, const char* fmt, va_list args);
#define kls_temp_vsprintf(t_kls, fmt, args) kls_temp_vsprintf_dbg((t_kls), KLS_HERE, (fmt), (args))
#endif // KOLISEO_HAS_LOCATE

#ifndef KOLISEO_HAS_LOCATE
char* kls_temp_sprintf(Koliseo_Temp* kls_t, const char* fmt, ...);
#else
char* kls_temp_sprintf_dbg(Koliseo_Temp* kls_t, Koliseo_Loc loc, const char* fmt, ...);
#define kls_temp_sprintf(t_kls, fmt, ...) kls_temp_sprintf_dbg((t_kls), KLS_HERE, (fmt), __VA_ARGS__)
#endif // KOLISEO_HAS_LOCATE

void print_temp_kls_2file(FILE * fp, const Koliseo_Temp * t_kls);
void print_dbg_temp_kls(const Koliseo_Temp * t_kls);

/**
 * Macro used to request memory for an array of type values from a Koliseo_Temp.
 */
#define KLS_PUSH_ARR_T(kls_temp, type, count) (type*)kls_temp_push_zero_ext((kls_temp), sizeof(type), KLS_ALIGNOF(type), (count))

/**
 * Macro used to format a cstring into a Koliseo_Temp.
 */
#define KLS_SPRINTF_T(kls_temp, fmt, ...) kls_temp_sprintf((kls_temp), (fmt), __VA_ARGS__)

/**
 * Macro to request memory for a C string from a Koliseo_Temp.
 * @see KLS_STRDUP_T()
 */
#define KLS_PUSH_STR_T(kls_temp, cstr) KLS_PUSH_ARR_T((kls_temp), char, strlen((cstr))+1)

/**
 * Macro used to request memory for an array of type values from a Koliseo_Temp, and assign a name and a description to the region item.
 */
#define KLS_PUSH_ARR_T_NAMED(kls_temp, type, count, name, desc) KLS_PUSH_ARR_T((kls_temp),type,(count))

/**
 * Macro to request memory for a C string from a Koliseo_Temp, and assign a name and a description to the region item.
 */
#define KLS_PUSH_STR_T_NAMED(kls_temp, cstr, name, desc) KLS_PUSH_ARR_T_NAMED((kls_temp), char, strlen((cstr))+1, (name), (desc))

/**
 * Macro used to request memory for an array of type values from a Koliseo_Temp, and assign a type, a name and a description to the region item.
 */
#define KLS_PUSH_ARR_T_TYPED(kls_temp, type, count, region_type, name, desc) KLS_PUSH_ARR_T((kls_temp),type,(count))

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

#ifdef KOLISEO_HAS_EXPER

void *kls_pop(Koliseo * kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void *kls_pop_AR(Koliseo *kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);

/**
 * Macro used to "remove" memory as an array from a Koliseo. Rewinds the pointer by the requested type and returns a pointer to that memory before updating the Koliseo index.
 * It's up to you to copy your item somewhere else before calling any PUSH operation again, as that memory should be overwritten.
 */
#define KLS_POP_ARR(kls, type, count) (type*)kls_pop_AR((kls), sizeof(type), KLS_ALIGNOF(type), (count))

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
#define KLS_POP_ARR_T(kls_temp, type, count) (type*)kls_temp_pop_AR((kls_temp), sizeof(type), KLS_ALIGNOF(type), (count))

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

#if defined(__cplusplus)
}
#endif // __cplusplus
#endif //KOLISEO_H_

