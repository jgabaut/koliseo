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
#include "koliseo.h"

KLS_Conf KLS_DEFAULT_CONF = {
    .kls_collect_stats = 0,
    .kls_verbose_lvl = 0,
    .kls_block_while_has_temp = 1,
    .kls_allow_zerocount_push = 0,
    .kls_log_fp = NULL,
    .kls_growable = 0,
    .kls_log_filepath = "",
    .err_handlers = {
#ifndef KOLISEO_HAS_LOCATE
        .OOM_handler = &KLS_OOM_default_handler__,
        .PTRDIFF_MAX_handler = &KLS_PTRDIFF_MAX_default_handler__,
        .ZEROCOUNT_handler = &KLS_ZEROCOUNT_default_handler__,
#else
        .OOM_handler = &KLS_OOM_default_handler_dbg__,
        .PTRDIFF_MAX_handler = &KLS_PTRDIFF_MAX_default_handler_dbg__,
        .ZEROCOUNT_handler = &KLS_ZEROCOUNT_default_handler_dbg__,
#endif // KOLISEO_HAS_LOCATE
    },
}; /**< Config used by any new Koliseo by default.*/

KLS_Stats KLS_STATS_DEFAULT = {
    .tot_pushes = 0,
    .tot_temp_pushes = 0,
    .tot_pops = 0,
    .tot_temp_pops = 0,
    .tot_logcalls = 0,
    .tot_hiccups = 0,
#ifdef KLS_DEBUG_CORE
    .worst_pushcall_time = -1,
#endif
};

static bool kls_set_conf(Koliseo * kls, KLS_Conf conf);	//Declare function used internally by kls_new() and kls_new_conf()

/**
 * Returns the constant string representing current version for Koliseo.
 * @return A constant string in MAJOR-MINOR-PATCH format for current Koliseo version.
 */
const char *string_koliseo_version(void)
{
    return KOLISEO_API_VERSION_STRING;
}

/**
 * Returns the constant int representing current version for Koliseo.
 * @return A constant int in numeric format for current Koliseo version.
 */
int int_koliseo_version(void)
{
    return KOLISEO_API_VERSION_INT;
}

/**
 * Used internally for handling Out-Of-Memory in push calls when no user handler is provided.
 * @param kls The Koliseo used in the push call
 * @param available The Koliseo's available memory
 * @param padding The current push call's padding
 * @param size The current push call's size
 * @param count The current push call's count
 * @see KLS_Conf
 */
#ifndef KOLISEO_HAS_LOCATE
void KLS_OOM_default_handler__(Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, ptrdiff_t count)
#else
void KLS_OOM_default_handler_dbg__(Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, ptrdiff_t count, Koliseo_Loc loc)
#endif // KOLISEO_HAS_LOCATE
{
#ifndef KOLISEO_HAS_LOCATE
    fprintf(stderr,
            "[KLS]  Out of memory. size*count [%td] was bigger than available-padding [%td].\n",
            size * count, available - padding);
#else
    fprintf(stderr,
            "[KLS] " KLS_Loc_Fmt "Out of memory. size*count [%td] was bigger than available-padding [%td].\n",
            KLS_Loc_Arg(loc),
            size * count, available - padding);
#endif // KOLISEO_HAS_LOCATE
    kls_free(kls); // Is it even worth it to try?
    exit(EXIT_FAILURE); // Better than nothing. May change to return NULL instead? Requiring refactor of handler signature
}

#ifndef KOLISEO_HAS_LOCATE
void KLS_PTRDIFF_MAX_default_handler__(struct Koliseo* kls, ptrdiff_t size, ptrdiff_t count)
#else
void KLS_PTRDIFF_MAX_default_handler_dbg__(struct Koliseo* kls, ptrdiff_t size, ptrdiff_t count, Koliseo_Loc loc)
#endif
{
#ifndef _WIN32
#ifndef KOLISEO_HAS_LOCATE
    fprintf(stderr,
            "[KLS]  count [%td] was bigger than PTRDIFF_MAX/size [%li].\n",
            count, PTRDIFF_MAX / size);
#else
    fprintf(stderr,
            "[KLS] " KLS_Loc_Fmt "count [%td] was bigger than PTRDIFF_MAX/size [%li].\n",
            KLS_Loc_Arg(loc),
            count, PTRDIFF_MAX / size);
#endif // KOLISEO_HAS_LOCATE
#else
#ifndef KOLISEO_HAS_LOCATE
    fprintf(stderr,
            "[KLS]  count [%td] was bigger than PTRDIFF_MAX/size [%lli].\n",
            count, PTRDIFF_MAX / size);
#else
    fprintf(stderr,
            "[KLS] " KLS_Loc_Fmt "count [%td] was bigger than PTRDIFF_MAX/size [%lli].\n",
            KLS_Loc_Arg(loc),
            count, PTRDIFF_MAX / size);
#endif // KOLISEO_HAS_LOCATE
#endif // _WIN32
    kls_free(kls);
    exit(EXIT_FAILURE);
}

/**
 * Used internally for handling zero-count in push calls when no user handler is provided.
 * By default, it goes to exit().
 * @param kls The Koliseo used in the push call
 * @param available The Koliseo's available memory
 * @param padding The current push call's padding
 * @param size The current push call's size
 * @see KLS_Conf
 */
#ifndef KOLISEO_HAS_LOCATE
void KLS_ZEROCOUNT_default_handler__(Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size)
#else
void KLS_ZEROCOUNT_default_handler_dbg__(Koliseo* kls, ptrdiff_t available, ptrdiff_t padding, ptrdiff_t size, Koliseo_Loc loc)
#endif // KOLISEO_HAS_LOCATE
{
#ifndef KOLISEO_HAS_LOCATE
    fprintf(stderr,
            "[KLS]  Doing a zero-count push. size [%td] padding [%td] available [%td].\n",
            size, padding, available);
#else
    fprintf(stderr,
            "[KLS] " KLS_Loc_Fmt "Doing a zero-count push. size [%td] padding [%td] available [%td].\n",
            KLS_Loc_Arg(loc),
            size, padding, available);
#endif // KOLISEO_HAS_LOCATE
    kls_free(kls);
    exit(EXIT_FAILURE);
}

/**
 * Used to prepare a KLS_Conf without caring about KOLISEO_HAS_REGIONS.
 * Passes custom error handlers for errors in push calls.
 * @see KLS_Conf
 */
KLS_Conf kls_conf_init_handled(int collect_stats, int verbose_lvl, int block_while_has_temp, int allow_zerocount_push, int growable, FILE* log_fp, const char* log_filepath, KLS_Err_Handlers err_handlers)
{
    KLS_Conf res = {0};
    res.kls_collect_stats = collect_stats;
    res.kls_verbose_lvl = verbose_lvl;
    res.kls_block_while_has_temp = block_while_has_temp;
    res.kls_allow_zerocount_push = allow_zerocount_push;
    res.kls_growable = growable;
    res.kls_log_fp = log_fp;
    res.kls_log_filepath = log_filepath;

    if (err_handlers.OOM_handler != NULL) {
        res.err_handlers.OOM_handler = err_handlers.OOM_handler;
    } else {
#ifndef KOLISEO_HAS_LOCATE
        res.err_handlers.OOM_handler = &KLS_OOM_default_handler__;
#else
        res.err_handlers.OOM_handler = &KLS_OOM_default_handler_dbg__;
#endif // KOLISEO_HAS_LOCATE
    }

    if (err_handlers.PTRDIFF_MAX_handler != NULL) {
        res.err_handlers.PTRDIFF_MAX_handler = err_handlers.PTRDIFF_MAX_handler;
    } else {
#ifndef KOLISEO_HAS_LOCATE
        res.err_handlers.PTRDIFF_MAX_handler = &KLS_PTRDIFF_MAX_default_handler__;
#else
        res.err_handlers.PTRDIFF_MAX_handler = &KLS_PTRDIFF_MAX_default_handler_dbg__;
#endif // KOLISEO_HAS_LOCATE
    }

    if (err_handlers.ZEROCOUNT_handler != NULL) {
        res.err_handlers.ZEROCOUNT_handler = err_handlers.ZEROCOUNT_handler;
    } else {
#ifndef KOLISEO_HAS_LOCATE
        res.err_handlers.ZEROCOUNT_handler = &KLS_ZEROCOUNT_default_handler__;
#else
        res.err_handlers.ZEROCOUNT_handler = &KLS_ZEROCOUNT_default_handler_dbg__;
#endif // KOLISEO_HAS_LOCATE
    }

    return res;
}

/**
 * Used to prepare a KLS_Conf without caring about KOLISEO_HAS_REGIONS.
 * @see KLS_Conf
 */
KLS_Conf kls_conf_init(int collect_stats, int verbose_lvl, int block_while_has_temp, int allow_zerocount_push, int growable, FILE* log_fp, const char* log_filepath)
{
    KLS_Err_Handlers err_handlers = KLS_DEFAULT_ERR_HANDLERS;
    return kls_conf_init_handled(collect_stats, verbose_lvl, block_while_has_temp, allow_zerocount_push, growable, log_fp, log_filepath, err_handlers);
}

/**
 * Prints enabled Koliseo features to stderr.
 */
void kls_dbg_features(void)
{
#ifdef KOLISEO_HAS_LOCATE
    bool kls_locate = true;
#else
    bool kls_locate = false;
#endif
#ifdef KLS_DEBUG_CORE
    bool kls_debug = true;
#else
    bool kls_debug = false;
#endif
#ifdef KOLISEO_HAS_EXPER
    bool kls_exper = true;
#else
    bool kls_exper = false;
#endif
    bool features[3] = {
        [0] = kls_debug,
        [1] = kls_locate,
        [2] = kls_exper,
    };
    int total_enabled = 0;
    for (int i=0; i<3; i++) {
        if (features[i]) {
            total_enabled += 1;
        }
    }
    fprintf(stderr, "[KLS]    Enabled features: {");
    if (total_enabled == 0) {
        fprintf(stderr, "none}\n");
        return;
    } else {
        if (kls_debug) {
            fprintf(stderr, "debug%s", (total_enabled > 1 ? ", " : ""));
            total_enabled -= 1;
        }
        if (kls_locate) {
            fprintf(stderr, "locate%s", (total_enabled > 1 ? ", " : ""));
            total_enabled -= 1;
        }
        if (kls_exper) {
            fprintf(stderr, "exper");
        }
        fprintf(stderr, "}\n");
    }
}

/**
 * Returns the current offset (position of pointer bumper) for the passed Koliseo.
 * @param kls The Koliseo at hand.
 * @return A ptrdiff_t value for current position.
 */
ptrdiff_t kls_get_pos(const Koliseo *kls)
{
    return kls->offset;
}

/**
 * Logs a message to the log_fp FILE field of the passed Koliseo pointer, if its conf.kls_verbose_lvl is >0.
 * @param kls The Koliseo pointer hosting the log_fp FILE pointer.
 * @param tag Tag for a message.
 * @param format The message format string.
 */
void kls_log(Koliseo *kls, const char *tag, const char *format, ...)
{
    if (kls == NULL) {
        fprintf(stderr, "[KLS]    %s(): Passed kls was NULL.\n", __func__);
        return;
    }
    if (kls->conf.kls_verbose_lvl > 0) {
        va_list args;
        FILE *fp = kls->conf.kls_log_fp;
        va_start(args, format);
        if (fp == NULL) {
            fprintf(stderr,
                    "[KLS]    %s(): Failed opening file to print logs.\n",
                    __func__);
        } else {
            time_t now = time(0);
            const struct tm *mytime = localtime(&now);
            char timeheader[500];
            if (strftime(timeheader, sizeof timeheader, "%X", mytime)) {
                fprintf(fp, "[%-10.10s] [%s] [", tag, timeheader);
                vfprintf(fp, format, args);
                fprintf(fp, "]\n");
            }
        }
        va_end(args);
    }
}

/**
 * Takes a ptrdiff_t size and a function pointer to the allocation function.
 * Additional arguments are for extensions.
 * Allocates the backing memory for a Koliseo.
 * Sets the KLS_Conf field to KLS_DEFAULT_CONF.
 * Sets the fields with appropriate values if memory allocation was successful, goes to exit() otherwise.
 * @param size The size for Koliseo data field.
 * @param alloc_func The allocation function to use to init the Koliseo.
 * @return A pointer to the initialised Koliseo struct.
 * @see Koliseo
 * @see Koliseo_Temp
 * @see KLS_DEFAULT_CONF
 * @see KLS_Hooks
 * @see kls_new()
 * @see kls_temp_start()
 * @see kls_temp_end()
 */
#ifndef KOLISEO_HAS_LOCATE
Koliseo *kls_new_alloc_ext(ptrdiff_t size, kls_alloc_func alloc_func, kls_free_func free_func, KLS_Hooks* ext_handlers, void** user, size_t ext_len)
#else
Koliseo *kls_new_alloc_ext_dbg(ptrdiff_t size, kls_alloc_func alloc_func, kls_free_func free_func, KLS_Hooks* ext_handlers, void** user, size_t ext_len, Koliseo_Loc loc)
#endif // KOLISEO_HAS_LOCATE
{
    if (size < (ptrdiff_t)sizeof(Koliseo)) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[ERROR]    at %s():  invalid requested kls size (%td). Min accepted is: (%td).\n",
                __func__, size, (ptrdiff_t)sizeof(Koliseo));
#else
        fprintf(stderr,
                "[ERROR] " KLS_Loc_Fmt "%s():  invalid requested kls size (%td). Min accepted is: (%td).\n",
                KLS_Loc_Arg(loc), __func__, size, (ptrdiff_t)sizeof(Koliseo));
#endif // KOLISEO_HAS_LOCATE
        //TODO Is it better to abort the program?
        return NULL;
    }
    void *p = alloc_func(size);
    if (p) {
        //sprintf(msg,"Allocated (%li) for new KLS.",size);
        //kls_log("KLS",msg);
        char h_size[200];
        kls_formatSize(size, h_size, sizeof(h_size));
        Koliseo *kls = p;
        kls->data = p;
        kls->size = size;
        kls->offset = sizeof(*kls);
        kls->prev_offset = kls->offset;
        kls->has_temp = 0;
        kls->t_kls = NULL;
        kls_set_conf(kls, KLS_DEFAULT_CONF);
        kls->stats = KLS_STATS_DEFAULT;
        kls->conf.kls_log_fp = stderr;
        kls->hooks_len = ext_len;

        for (size_t i=0; i < kls->hooks_len; i++) {
            kls->hooks[i] = ext_handlers[i];
        }
        for (size_t i=0; i < kls->hooks_len; i++) {
            if (user) {
                kls->extension_data[i] = user[i];
            } else {
                kls->extension_data[i] = NULL;
            }
        }
        kls->free_func = free_func;
        kls->next = NULL;
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "KLS", "API Level { %i } ->  Allocated (%s) for new KLS.",
                int_koliseo_version(), h_size);
        kls_log(kls, "KLS", "KLS offset: { %p }.", kls);
        kls_log(kls, "KLS", "Allocation begin offset: { %p }.",
                kls + kls->offset);
#endif

        for (size_t i=0; i < kls->hooks_len; i++) {
            if (kls->hooks[i].on_new_handler != NULL) {
                // Call on_new extension
                kls->hooks[i].on_new_handler(kls);
            }
        }
    } else {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr, "[KLS] Failed %s() call.\n", __func__);
#else
        fprintf(stderr, "[KLS] " KLS_Loc_Fmt "Failed %s() call.\n", KLS_Loc_Arg(loc), __func__);
#endif // KOLISEO_HAS_LOCATE
        exit(EXIT_FAILURE);
    }
#ifdef KLS_DEBUG_CORE
    Koliseo *kls_ref = p;
    if (kls_ref->conf.kls_verbose_lvl > 0) {
        print_kls_2file(kls_ref->conf.kls_log_fp, p);
    }
#endif
    return p;
}

/**
 * Takes a ptrdiff_t size and a function pointer to the allocation function.
 * Allocates the backing memory for a Koliseo.
 * Sets the KLS_Conf field to KLS_DEFAULT_CONF.
 * Sets the fields with appropriate values if memory allocation was successful, goes to exit() otherwise.
 * @param size The size for Koliseo data field.
 * @param alloc_func The allocation function to use to init the Koliseo.
 * @return A pointer to the initialised Koliseo struct.
 * @see Koliseo
 * @see Koliseo_Temp
 * @see KLS_DEFAULT_CONF
 * @see kls_new()
 * @see kls_temp_start()
 * @see kls_temp_end()
 */
#ifndef KOLISEO_HAS_LOCATE
Koliseo *kls_new_alloc(ptrdiff_t size, kls_alloc_func alloc_func, kls_free_func free_func)
#else
Koliseo *kls_new_alloc_dbg(ptrdiff_t size, kls_alloc_func alloc_func, kls_free_func free_func, Koliseo_Loc loc)
#endif // KOLISEO_HAS_LOCATE
{
#ifndef KOLISEO_HAS_LOCATE
    return kls_new_alloc_ext(size, alloc_func, free_func, KLS_DEFAULT_HOOKS, KLS_DEFAULT_EXTENSION_DATA, KLS_DEFAULT_EXTENSIONS_LEN);
#else
    return kls_new_alloc_ext_dbg(size, alloc_func, free_func, KLS_DEFAULT_HOOKS, KLS_DEFAULT_EXTENSION_DATA, KLS_DEFAULT_EXTENSIONS_LEN, loc);
#endif // KOLISEO_HAS_LOCATE
}

/**
 * Takes a ptrdiff_t size.
 * Allocates the backing memory for a Koliseo.
 * Sets the KLS_Conf field to KLS_DEFAULT_CONF.
 * Sets the fields with appropriate values if memory allocation was successful, goes to exit() otherwise.
 * @param size The size for Koliseo data field.
 * @return A pointer to the initialised Koliseo struct.
 * @see Koliseo
 * @see Koliseo_Temp
 * @see KLS_DEFAULT_CONF
 * @see kls_new_alloc()
 */
Koliseo *kls_new(ptrdiff_t size)
{
    return kls_new_alloc(size, KLS_DEFAULT_ALLOCF, KLS_DEFAULT_FREEF);
}

/**
 * Takes a ptrdiff_t size, a KLS_Conf to configure the new Koliseo, an allocation function pointer and a free function pointer.
 * Additional arguments are for extensions.
 * Calls kls_new_alloc() to initialise the Koliseo, then calls kls_set_conf() to update the config.
 * Returns the new Koliseo.
 * @param size The size for Koliseo data field.
 * @param conf The KLS_Conf for the new Koliseo.
 * @param alloc_func The allocation function to use.
 * @param free_func The free function to use.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see KLS_DEFAULT_CONF
 * @see kls_new_alloc_ext()
 * @see kls_set_conf()
 */
Koliseo *kls_new_conf_alloc_ext(ptrdiff_t size, KLS_Conf conf, kls_alloc_func alloc_func, kls_free_func free_func, KLS_Hooks* ext_handlers, void** user, size_t ext_len)
{
    Koliseo *k = kls_new_alloc_ext(size, alloc_func, free_func, ext_handlers, user, ext_len);
    bool conf_res = kls_set_conf(k, conf);
    if (!conf_res) {
        fprintf(stderr,
                "[ERROR] [%s()]: Failed to set config for new Koliseo.\n",
                __func__);
        exit(EXIT_FAILURE);
    }
    return k;
}

/**
 * Takes a ptrdiff_t size, a KLS_Conf to configure the new Koliseo, an allocation function pointer and a free function pointer.
 * Additional arguments are for extensions.
 * Calls kls_new_alloc() to initialise the Koliseo, then calls kls_set_conf() to update the config.
 * Returns the new Koliseo.
 * @param size The size for Koliseo data field.
 * @param conf The KLS_Conf for the new Koliseo.
 * @param alloc_func The allocation function to use.
 * @param free_func The free function to use.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see KLS_DEFAULT_CONF
 * @see kls_new_alloc()
 * @see kls_set_conf()
 */
Koliseo *kls_new_conf_alloc(ptrdiff_t size, KLS_Conf conf, kls_alloc_func alloc_func, kls_free_func free_func)
{
    return kls_new_conf_alloc_ext(size, conf, alloc_func, free_func, KLS_DEFAULT_HOOKS, KLS_DEFAULT_EXTENSION_DATA, KLS_DEFAULT_EXTENSIONS_LEN);
}

/**
 * Takes a ptrdiff_t size and a KLS_Conf to configure the new Koliseo.
 * Additional arguments are for extensions.
 * Returns the new Koliseo.
 * @param size The size for Koliseo data field.
 * @param conf The KLS_Conf for the new Koliseo.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see KLS_DEFAULT_CONF
 * @see kls_new_alloc_ext()
 */
Koliseo *kls_new_conf_ext(ptrdiff_t size, KLS_Conf conf, KLS_Hooks* ext_handlers, void** user, size_t ext_len)
{
    return kls_new_conf_alloc_ext(size, conf, KLS_DEFAULT_ALLOCF, KLS_DEFAULT_FREEF, ext_handlers, user, ext_len);
}

/**
 * Takes a ptrdiff_t size and a KLS_Conf to configure the new Koliseo.
 * Returns the new Koliseo.
 * @param size The size for Koliseo data field.
 * @param conf The KLS_Conf for the new Koliseo.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see KLS_DEFAULT_CONF
 * @see kls_new_alloc_ext()
 */
Koliseo *kls_new_conf(ptrdiff_t size, KLS_Conf conf)
{
    return kls_new_conf_alloc(size, conf, KLS_DEFAULT_ALLOCF, KLS_DEFAULT_FREEF);
}

/**
 * Takes a ptrdiff_t size, a filepath for the trace output file, an allocation function pointer and a free function pointer.
 * Additional arguments are for extensions.
 * Returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a traced Koliseo, logging to the passed filepath.
 * @param size The size for Koliseo data field.
 * @param output_path The filepath for log output.
 * @param alloc_func The allocation function to use.
 * @param free_func The free function to use.
 * @param err_handlers The error handlers struct for errors in push calls.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf_alloc_ext()
 */
Koliseo *kls_new_traced_alloc_handled_ext(ptrdiff_t size, const char *output_path, kls_alloc_func alloc_func, kls_free_func free_func, KLS_Err_Handlers err_handlers, KLS_Hooks* ext_handlers, void** user, size_t ext_len)
{

#ifndef KLS_DEBUG_CORE
    fprintf(stderr,
            "[WARN]    %s(): KLS_DEBUG_CORE is not defined. No tracing allowed.\n",
            __func__);
#endif
    KLS_Conf k = (KLS_Conf) {
        .kls_collect_stats = 1,.kls_verbose_lvl =
                                 1,.kls_log_filepath = output_path,
#ifndef KOLISEO_HAS_LOCATE
                                   .err_handlers.OOM_handler = (err_handlers.OOM_handler != NULL ? err_handlers.OOM_handler : &KLS_OOM_default_handler__),
                                   .err_handlers.PTRDIFF_MAX_handler = ( err_handlers.PTRDIFF_MAX_handler != NULL ? err_handlers.PTRDIFF_MAX_handler : &KLS_PTRDIFF_MAX_default_handler__),
#else
                                   .err_handlers.OOM_handler = (err_handlers.OOM_handler != NULL ? err_handlers.OOM_handler : &KLS_OOM_default_handler_dbg__),
                                   .err_handlers.PTRDIFF_MAX_handler = ( err_handlers.PTRDIFF_MAX_handler != NULL ? err_handlers.PTRDIFF_MAX_handler : &KLS_PTRDIFF_MAX_default_handler_dbg__),
#endif // KOLISEO_HAS_LOCATE
    };
    return kls_new_conf_alloc_ext(size, k, alloc_func, free_func, ext_handlers, user, ext_len);
}

/**
 * Takes a ptrdiff_t size, a filepath for the trace output file, an allocation function pointer and a free function pointer.
 * Returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a traced Koliseo, logging to the passed filepath.
 * @param size The size for Koliseo data field.
 * @param output_path The filepath for log output.
 * @param alloc_func The allocation function to use.
 * @param free_func The free function to use.
 * @param err_handlers The error handlers struct for errors in push calls.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf_alloc()
 */
Koliseo *kls_new_traced_alloc_handled(ptrdiff_t size, const char *output_path, kls_alloc_func alloc_func, kls_free_func free_func, KLS_Err_Handlers err_handlers)
{
    return kls_new_traced_alloc_handled_ext(size, output_path, alloc_func, free_func, err_handlers, KLS_DEFAULT_HOOKS, KLS_DEFAULT_EXTENSION_DATA, KLS_DEFAULT_EXTENSIONS_LEN);
}

/**
 * Takes a ptrdiff_t size and a filepath for the trace output file.
 * Additional arguments are for extensions.
 * Returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a traced Koliseo, logging to the passed filepath.
 * @param size The size for Koliseo data field.
 * @param output_path The filepath for log output.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf_alloc()
 */
Koliseo *kls_new_traced_ext(ptrdiff_t size, const char *output_path, KLS_Hooks* ext_handlers, void** user, size_t ext_len)
{
    KLS_Err_Handlers err_handlers = KLS_DEFAULT_ERR_HANDLERS;
    return kls_new_traced_alloc_handled_ext(size, output_path, KLS_DEFAULT_ALLOCF, KLS_DEFAULT_FREEF, err_handlers, ext_handlers, user, ext_len);
}

/**
 * Takes a ptrdiff_t size, a filepath for the trace output file, an allocation function pointer and a free function pointer.
 * Returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a traced Koliseo, logging to the passed filepath.
 * @param size The size for Koliseo data field.
 * @param output_path The filepath for log output.
 * @param alloc_func The allocation function to use.
 * @param free_func The free function to use.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf_alloc()
 */
Koliseo *kls_new_traced_alloc(ptrdiff_t size, const char *output_path, kls_alloc_func alloc_func, kls_free_func free_func)
{
    KLS_Err_Handlers err_handlers = KLS_DEFAULT_ERR_HANDLERS;
    return kls_new_traced_alloc_handled(size, output_path, alloc_func, free_func, err_handlers);
}

/**
 * Takes a ptrdiff_t size and a filepath for the trace output file.
 * Returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a traced Koliseo, logging to the passed filepath.
 * @param size The size for Koliseo data field.
 * @param output_path The filepath for log output.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf_alloc()
 */
Koliseo *kls_new_traced(ptrdiff_t size, const char *output_path)
{
    return kls_new_traced_alloc(size, output_path, KLS_DEFAULT_ALLOCF, KLS_DEFAULT_FREEF);
}

/**
 * Takes a ptrdiff_t size and a filepath for the trace output file.
 * Returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a traced Koliseo, logging to the passed filepath.
 * @param size The size for Koliseo data field.
 * @param output_path The filepath for log output.
 * @param err_handlers The error handlers struct for errors in push calls.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf_alloc()
 */
Koliseo *kls_new_traced_handled(ptrdiff_t size, const char *output_path, KLS_Err_Handlers err_handlers)
{
    return kls_new_traced_alloc_handled(size, output_path, KLS_DEFAULT_ALLOCF, KLS_DEFAULT_FREEF, err_handlers);
}

/**
 * Takes a ptrdiff_t size, an allocation function pointer and a free function pointer, and returns a pointer to the prepared Koliseo.
 * Additional arguments are for extensions.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a debug Koliseo (printing to stderr).
 * @param size The size for Koliseo data field.
 * @param alloc_func The allocation function to use.
 * @param free_func The free function to use.
 * @param err_handlers The error handlers for errors in push calls.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf_alloc_ext()
 */
Koliseo *kls_new_dbg_alloc_handled_ext(ptrdiff_t size, kls_alloc_func alloc_func, kls_free_func free_func, KLS_Err_Handlers err_handlers, KLS_Hooks* ext_handlers, void** user, size_t ext_len)
{
#ifndef KLS_DEBUG_CORE
    fprintf(stderr,
            "[WARN]    %s(): KLS_DEBUG_CORE is not defined. No debugging support.\n",
            __func__);
#endif
    KLS_Conf k = (KLS_Conf) {
        .kls_collect_stats = 1,.kls_verbose_lvl = 0,
#ifndef KOLISEO_HAS_LOCATE
                               .err_handlers.OOM_handler = ( err_handlers.OOM_handler != NULL ? err_handlers.OOM_handler : &KLS_OOM_default_handler__),
                               .err_handlers.PTRDIFF_MAX_handler = ( err_handlers.PTRDIFF_MAX_handler != NULL ? err_handlers.PTRDIFF_MAX_handler : &KLS_PTRDIFF_MAX_default_handler__),
#else
                               .err_handlers.OOM_handler = ( err_handlers.OOM_handler != NULL ? err_handlers.OOM_handler : &KLS_OOM_default_handler_dbg__),
                               .err_handlers.PTRDIFF_MAX_handler = ( err_handlers.PTRDIFF_MAX_handler != NULL ? err_handlers.PTRDIFF_MAX_handler : &KLS_PTRDIFF_MAX_default_handler_dbg__),
#endif // KOLIEO_HAS_LOCATE
    };
    Koliseo * kls = kls_new_conf_alloc_ext(size, k, alloc_func, free_func, ext_handlers, user, ext_len);
    kls->conf.kls_verbose_lvl = 1;
    return kls;
}

/**
 * Takes a ptrdiff_t size, an allocation function pointer and a free function pointer, and returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a debug Koliseo (printing to stderr).
 * @param size The size for Koliseo data field.
 * @param alloc_func The allocation function to use.
 * @param free_func The free function to use.
 * @param err_handlers The error handlers for errors in push calls.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf()
 */
Koliseo *kls_new_dbg_alloc_handled(ptrdiff_t size, kls_alloc_func alloc_func, kls_free_func free_func, KLS_Err_Handlers err_handlers)
{
    return kls_new_dbg_alloc_handled_ext(size, alloc_func, free_func, err_handlers, KLS_DEFAULT_HOOKS, KLS_DEFAULT_EXTENSION_DATA, KLS_DEFAULT_EXTENSIONS_LEN);
}

/**
 * Takes a ptrdiff_t size.
 * Additional arguments are for extensions.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a debug Koliseo (printing to stderr).
 * @param size The size for Koliseo data field.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf()
 */
Koliseo *kls_new_dbg_ext(ptrdiff_t size, KLS_Hooks* ext_handlers, void** user, size_t ext_len)
{
    KLS_Err_Handlers err_handlers = KLS_DEFAULT_ERR_HANDLERS;
    return kls_new_dbg_alloc_handled_ext(size, KLS_DEFAULT_ALLOCF, KLS_DEFAULT_FREEF, err_handlers, ext_handlers, user, ext_len);
}

/**
 * Takes a ptrdiff_t size, an allocation function pointer and a free function pointer, and returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a debug Koliseo (printing to stderr).
 * @param size The size for Koliseo data field.
 * @param alloc_func The allocation function to use.
 * @param free_func The free function to use.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf()
 */
Koliseo *kls_new_dbg_alloc(ptrdiff_t size, kls_alloc_func alloc_func, kls_free_func free_func)
{
    KLS_Err_Handlers err_handlers = KLS_DEFAULT_ERR_HANDLERS;
    return kls_new_dbg_alloc_handled(size, alloc_func, free_func, err_handlers);
}

/**
 * Takes a ptrdiff_t size and returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a debug Koliseo (printing to stderr).
 * @param size The size for Koliseo data field.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf()
 */
Koliseo *kls_new_dbg(ptrdiff_t size)
{
    return kls_new_dbg_alloc(size, KLS_DEFAULT_ALLOCF, KLS_DEFAULT_FREEF);
}

/**
 * Takes a ptrdiff_t size and returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a debug Koliseo (printing to stderr).
 * @param size The size for Koliseo data field.
 * @param err_handlers The error handlers for errors in push calls.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf()
 */
Koliseo *kls_new_dbg_handled(ptrdiff_t size, KLS_Err_Handlers err_handlers)
{
    return kls_new_dbg_alloc_handled(size, KLS_DEFAULT_ALLOCF, KLS_DEFAULT_FREEF, err_handlers);
}

/**
 * Updates the KLS_Conf for the passed Koliseo pointer. Internal usage.
 * @param kls The Koliseo pointer to update.
 * @param conf The KLS_Conf to set.
 * @return A bool representing success.
 */
bool kls_set_conf(Koliseo *kls, KLS_Conf conf)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo was NULL.\n", __func__);
        //TODO: is it better to exit() here?
        return false;
    }

    kls->conf = conf;
    if (kls->conf.kls_log_fp == NULL) {
        kls->conf.kls_log_fp = stderr;
#ifdef KLS_DEBUG_CORE
#ifdef KLS_SETCONF_DEBUG
        kls_log(kls, "KLS",
                "[%s()]:  Preliminary set of conf.kls_log_fp to stderr.",
                __func__);
#endif
#endif // KLS_DEBUG_CORE
    }

    if (conf.err_handlers.OOM_handler == NULL) {
        fprintf(stderr,
                "[ERROR]    at %s():  passed OOM_handler is NULL. Using default.\n",
                __func__);
#ifdef KLS_DEBUG_CORE
#ifdef KLS_SETCONF_DEBUG
        kls_log(kls, "KLS",
                "[%s()]:  Passed OOM_handler was NULL, using default.",
                __func__);
#endif
#endif // KLS_DEBUG_CORE
#ifndef KOLISEO_HAS_LOCATE
        kls->conf.err_handlers.OOM_handler = &KLS_OOM_default_handler__;
#else
        kls->conf.err_handlers.OOM_handler = &KLS_OOM_default_handler_dbg__;
#endif
    }

    if (conf.err_handlers.PTRDIFF_MAX_handler == NULL) {
        fprintf(stderr,
                "[ERROR]    at %s():  passed PTRDIFF_MAX_handler is NULL. Using default.\n",
                __func__);
#ifdef KLS_DEBUG_CORE
#ifdef KLS_SETCONF_DEBUG
        kls_log(kls, "KLS",
                "[%s()]:  Passed PTRDIFF_MAX_handler was NULL, using default.",
                __func__);
#endif
#endif // KLS_DEBUG_CORE
#ifndef KOLISEO_HAS_LOCATE
        kls->conf.err_handlers.PTRDIFF_MAX_handler = &KLS_PTRDIFF_MAX_default_handler__;
#else
        kls->conf.err_handlers.PTRDIFF_MAX_handler = &KLS_PTRDIFF_MAX_default_handler_dbg__;
#endif
    }

#ifndef KLS_DEBUG_CORE
    if (kls->conf.kls_collect_stats == 1) {
        fprintf(stderr,
                "[WARN]    [%s()]: KLS_DEBUG_CORE is not defined. Stats may not be collected in full.\n",
                __func__);
    }
#endif

    if (kls->conf.kls_verbose_lvl > 0) {
        if (kls->conf.kls_log_fp != NULL) {
#ifdef KLS_DEBUG_CORE
#ifdef KLS_SETCONF_DEBUG
            kls_log(kls, "WARN",
                    "[%s()]: kls->conf.kls_log_fp was not NULL. Overriding it.",
                    __func__);
#endif
#endif
            if (kls->conf.kls_collect_stats == 1) {
                kls->stats.tot_hiccups += 1;
            }
        }

        FILE *log_fp = NULL;
        log_fp = fopen(kls->conf.kls_log_filepath, "w");
        if (!log_fp) {
            fprintf(stderr,
                    "[ERROR] [%s()]: Failed opening logfile at {\"%s\"} [write].\n",
                    __func__, kls->conf.kls_log_filepath);
            return false;
        } else {
            fprintf(log_fp, "%s", "");	//Reset log_fp
            fclose(log_fp);
        }
        log_fp = fopen(kls->conf.kls_log_filepath, "a");
        if (!log_fp) {
            fprintf(stderr,
                    "[ERROR] [%s()]: Failed opening logfile at {\"%s\"} [append].\n",
                    __func__, kls->conf.kls_log_filepath);
            return false;
        } else {
            kls->conf.kls_log_fp = log_fp;
        }
    }
    return true;
}

static bool kls__try_grow(Koliseo* kls, ptrdiff_t needed);

/**
 * Takes a Koliseo, a ptrdiff_t size, align and count, and a caller name.
 * Checks if the passed Koliseo can fit the requested allocation.
 * In case of errors, tries calling the appropriate handler.
 * @param kls The Koliseo to check space for.
 * @param size The size of the type to allocate.
 * @param align The alignment of the type to allocate.
 * @param count The count of allocations.
 * @param caller_name Name for caller. Used for error reporting.
 * @return 0 for success, other values for errors. May exit of some errors without a custom handler.
 * @see Koliseo
 */
#ifndef KOLISEO_HAS_LOCATE
int kls__check_available_failable(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, const char* caller_name)
#else
int kls__check_available_failable_dbg(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, const char* caller_name, Koliseo_Loc loc)
#endif // KOLISEO_HAS_LOCATE
{
    assert(kls != NULL);
    assert(caller_name != NULL);
    if (count < 0) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): count [%td] was < 0.\n",
                caller_name,
                count);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): count [%td] was < 0.\n",
                KLS_Loc_Arg(loc),
                caller_name,
                count);
#endif // KOLISEO_HAS_LOCATE
        return -1;
    }
    if (size < 1) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): size [%td] was < 1.\n",
                caller_name,
                size);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): size [%td] was < 1.\n",
                KLS_Loc_Arg(loc),
                caller_name,
                size);
#endif // KOLISEO_HAS_LOCATE
        return -1;
    }
    if (align < 1) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): align [%td] was < 1.\n",
                caller_name,
                align);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): align [%td] was < 1.\n",
                KLS_Loc_Arg(loc),
                caller_name,
                align);
#endif // KOLISEO_HAS_LOCATE
        return -1;
    }
    if (! ((align & (align - 1)) == 0)) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): align [%td] was not a power of 2.\n",
                caller_name,
                align);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): align [%td] was not a power of 2.\n",
                KLS_Loc_Arg(loc),
                caller_name,
                align);
#endif // KOLISEO_HAS_LOCATE
        return -1;
    }
    Koliseo* current = kls;
    while (current->next != NULL) {
        current = current->next;
    }
    const ptrdiff_t available = current->size - current->offset;
    const ptrdiff_t padding = -current->offset & (align - 1);
    bool ZEROCOUNT_happened = false;
    bool ZEROCOUNT_handled = false;
    if (count == 0) {
        if (current->conf.kls_allow_zerocount_push != 1) {
            ZEROCOUNT_happened = true;
            if (current->conf.err_handlers.ZEROCOUNT_handler != NULL) {
#ifndef KOLISEO_HAS_LOCATE
                current->conf.err_handlers.ZEROCOUNT_handler(current, available, padding, size);
#else
                current->conf.err_handlers.ZEROCOUNT_handler(current, available, padding, size, loc);
#endif // KOLISEO_HAS_LOCATE
                ZEROCOUNT_handled = true;
            } else {
#ifndef KOLISEO_HAS_LOCATE
                fprintf(stderr,
                        "[KLS] %s(): Doing a zero-count push. size [%td] padding [%td] available [%td].\n",
                        caller_name,
                        size, padding, available);
#else
                fprintf(stderr,
                        "[KLS] " KLS_Loc_Fmt "%s(): Doing a zero-count push. size [%td] padding [%td] available [%td].\n",
                        KLS_Loc_Arg(loc),
                        caller_name,
                        size, padding, available);
#endif // KOLISEO_HAS_LOCATE
                kls_free(kls);
                exit(EXIT_FAILURE);
            }
        } else {
#ifdef KLS_DEBUG_CORE
            kls_log(current, "DEBUG", "Accepting zero-count push: conf.kls_allow_zerocount_push was 1");
#endif // KLS_DEBUG_CORE
        }
    }

    if (ZEROCOUNT_happened && ZEROCOUNT_handled) {
#ifdef KLS_DEBUG_CORE
        kls_log(current, "DEBUG", "Requested a zero-count push while kls_allow_zerocount_push is not 1, but the error handler returned instead of exiting.");
#endif // KLS_DEBUG_CORE
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): Doing a zero-count push. size [%td] padding [%td] available [%td].\n",
                caller_name,
                size, padding, available);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): Doing a zero-count push. size [%td] padding [%td] available [%td].\n",
                KLS_Loc_Arg(loc),
                caller_name,
                size, padding, available);
#endif // KOLISEO_HAS_LOCATE
        kls_free(kls);
        exit(EXIT_FAILURE);
    }

    bool OOM_happened = false;
    bool OOM_handled = false;
    bool PTRDIFF_MAX_happened = false;
    bool PTRDIFF_MAX_handled = false;
    if (count > PTRDIFF_MAX / size || available - padding < size * count) {
        if (count > PTRDIFF_MAX / size) {
            PTRDIFF_MAX_happened = true;
            if (current->conf.err_handlers.PTRDIFF_MAX_handler != NULL) {
#ifndef KOLISEO_HAS_LOCATE
                current->conf.err_handlers.PTRDIFF_MAX_handler(current, size, count);
#else
                current->conf.err_handlers.PTRDIFF_MAX_handler(current, size, count, loc);
#endif // KOLISEO_HAS_LOCATE
                PTRDIFF_MAX_handled = true;
            } else { // Let's keep this here for now? It's the original part before adding KLS_PTRDIFF_MAX_default_handler__()
#ifndef _WIN32
#ifndef KOLISEO_HAS_LOCATE
                fprintf(stderr,
                        "[KLS] %s(): count [%td] was bigger than PTRDIFF_MAX/size [%li].\n",
                        caller_name,
                        count, PTRDIFF_MAX / size);
#else
                fprintf(stderr,
                        "[KLS] " KLS_Loc_Fmt "%s(): count [%td] was bigger than PTRDIFF_MAX/size [%li].\n",
                        KLS_Loc_Arg(loc),
                        caller_name,
                        count, PTRDIFF_MAX / size);
#endif // KOLISEO_HAS_LOCATE
#else
#ifndef KOLISEO_HAS_LOCATE
                fprintf(stderr,
                        "[KLS] %s(): count [%td] was bigger than PTRDIFF_MAX/size [%lli].\n",
                        caller_name,
                        count, PTRDIFF_MAX / size);
#else
                fprintf(stderr,
                        "[KLS] " KLS_Loc_Fmt "%s(): count [%td] was bigger than PTRDIFF_MAX/size [%lli].\n",
                        KLS_Loc_Arg(loc),
                        caller_name,
                        count, PTRDIFF_MAX / size);
#endif // KOLISEO_HAS_LOCATE
#endif // _WIN32
            }
        } else {
            if (current->conf.kls_growable == 1 && kls__try_grow(current, size + count + padding)) {
                return 0;
            }
            OOM_happened = true;
            if (current->conf.err_handlers.OOM_handler != NULL) {
#ifndef KOLISEO_HAS_LOCATE
                current->conf.err_handlers.OOM_handler(current, available, padding, size, count);
#else
                current->conf.err_handlers.OOM_handler(current, available, padding, size, count, loc);
#endif // KOLISEO_HAS_LOCATE
                OOM_handled = true;
            } else { // Let's keep this here for now? It's the original part before adding KLS_OOM_default_handler__()
#ifndef KOLISEO_HAS_LOCATE
                fprintf(stderr,
                        "[KLS] %s(): Out of memory. size*count [%td] was bigger than available-padding [%td].\n",
                        caller_name,
                        size * count, available - padding);
#else
                fprintf(stderr,
                        "[KLS] " KLS_Loc_Fmt "%s(): Out of memory. size*count [%td] was bigger than available-padding [%td].\n",
                        KLS_Loc_Arg(loc),
                        caller_name,
                        size * count, available - padding);
#endif // KOLISEO_HAS_LOCATE
            }
        }
        if (PTRDIFF_MAX_happened) {
            if (current->conf.err_handlers.PTRDIFF_MAX_handler && PTRDIFF_MAX_handled) {
#ifndef KOLISEO_HAS_LOCATE
                fprintf(stderr, "[KLS] %s(): PTRDIFF_MAX fault happened and was handled.\n", caller_name);
#ifdef KLS_DEBUG_CORE
                kls_log(current, "DEBUG", "%s(): PTRDIFF_MAX fault happened and was handled.", caller_name);
#endif // KLS_DEBUG_CORE
#else
                fprintf(stderr, "[KLS] " KLS_Loc_Fmt "%s(): PTRDIFF_MAX fault happened and was handled.\n", KLS_Loc_Arg(loc), caller_name);
#ifdef KLS_DEBUG_CORE
                kls_log(current, "DEBUG", KLS_Loc_Fmt "%s(): PTRDIFF_MAX fault happened and was handled.", KLS_Loc_Arg(loc), caller_name);
#endif // KLS_DEBUG_CORE
#endif // KOLISEO_HAS_LOCATE
                return -1;
            }
        } else if (OOM_happened) {
            if (current->conf.err_handlers.OOM_handler && OOM_handled) {
#ifndef KOLISEO_HAS_LOCATE
                fprintf(stderr, "[KLS] %s(): OOM fault happened and was handled.\n", caller_name);
#ifdef KLS_DEBUG_CORE
                kls_log(current, "DEBUG", "%s(): OOM fault happened and was handled.", caller_name);
#endif // KLS_DEBUG_CORE
#else
                fprintf(stderr, "[KLS] " KLS_Loc_Fmt "%s(): OOM fault happened and was handled.\n", KLS_Loc_Arg(loc), caller_name);
#ifdef KLS_DEBUG_CORE
                kls_log(current, "DEBUG", KLS_Loc_Fmt "%s(): OOM fault happened and was handled.", KLS_Loc_Arg(loc), caller_name);
#endif // KLS_DEBUG_CORE
#endif // KOLISEO_HAS_LOCATE
                return -1;
            }
        }
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr, "[KLS] Failed %s() call.\n", caller_name);
#else
        fprintf(stderr, "[KLS] " KLS_Loc_Fmt "Failed %s() call.\n", KLS_Loc_Arg(loc), caller_name);
#endif // KOLISEO_HAS_LOCATE
        kls_free(kls);
        exit(EXIT_FAILURE);
    }
    return 0;
}

bool kls__try_grow(Koliseo* kls, ptrdiff_t needed)
{
    ptrdiff_t new_size = KLS_MAX(kls->size * 2, needed);
    Koliseo* new_kls = kls_new_conf_alloc_ext(new_size, kls->conf, KLS_DEFAULT_ALLOCF, KLS_DEFAULT_FREEF, kls->hooks, kls->extension_data, kls->hooks_len);
    kls_log(kls, "DEBUG", "%s(): growing Koliseo, new size: {%td}", __func__, new_size);
    if (!new_kls) return false;
    kls->next = new_kls;
    return true;
}

/**
 * Takes a Koliseo pointer, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the Koliseo data field, or goes to exit() if the operation fails.
 * Notably, it does NOT zero the memory region.
 * @param kls The Koliseo at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @return A void pointer to the start of memory just pushed to the Koliseo, or NULL for errors.
 */
void *kls_push(Koliseo *kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count)
{

#ifdef KLS_DEBUG_CORE
#ifndef _WIN32
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
#else
    LARGE_INTEGER start_time, end_time, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time);
#endif
#endif

    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if ((kls->has_temp == 1) && (kls->conf.kls_block_while_has_temp == 1)) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo has an open Koliseo_Temp session.\n", __func__);
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "ERROR", "[%s()]: Passed Koliseo has an open Koliseo_Temp session.", __func__);
        exit(EXIT_FAILURE);
#endif // KLS_DEBUG_CORE
        return NULL;
    }
#ifndef KOLISEO_HAS_LOCATE
    kls__check_available(kls, size, align, count);
#else
    kls__check_available_dbg(kls, size, align, count, KLS_HERE);
#endif // KOLISEO_HAS_LOCATE
    Koliseo* current = kls;
    while (current->next != NULL) {
        current = current->next;
    }
    ptrdiff_t padding = -current->offset & (align - 1);
    char *p = current->data + current->offset + padding;
    current->prev_offset = current->offset;
    current->offset += padding + size * count;
    char h_size[200];
    kls_formatSize(size * count, h_size, sizeof(h_size));
    //sprintf(msg,"Pushed size (%li) for KLS.",size);
    //kls_log("KLS",msg);
#ifdef KLS_DEBUG_CORE
    kls_log(current, "KLS", "Curr offset: { %p }.", current + current->offset);
    kls_log(current, "KLS", "API Level { %i } -> Pushed size (%s) for KLS.",
            int_koliseo_version(), h_size);
    if (current->conf.kls_verbose_lvl > 0) {
        print_kls_2file(current->conf.kls_log_fp, current);
    }
    if (current->conf.kls_collect_stats == 1) {
#ifndef _WIN32
        clock_gettime(CLOCK_MONOTONIC, &end_time);	// %.9f
        double elapsed_time =
            (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec -
                start_time.tv_nsec) / 1e9;
#else
        QueryPerformanceCounter(&end_time);	// %.7f
        double elapsed_time =
            (double)(end_time.QuadPart -
                     start_time.QuadPart) / frequency.QuadPart;
#endif
        if (elapsed_time > current->stats.worst_pushcall_time) {
            current->stats.worst_pushcall_time = elapsed_time;
        }
    }
#endif
    if (current->conf.kls_collect_stats == 1) {
        current->stats.tot_pushes += 1;
    }
    return p;
}

/**
 * Takes a Koliseo pointer, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the Koliseo data field, or goes to exit() if the operation fails.
 * Notably, it zeroes the memory region.
 * @param kls The Koliseo at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @return A void pointer to the start of memory just pushed to the Koliseo, or NULL for errors.
 */
#ifndef KOLISEO_HAS_LOCATE
void *kls_push_zero(Koliseo *kls, ptrdiff_t size, ptrdiff_t align,
                    ptrdiff_t count)
#else
void *kls_push_zero_dbg(Koliseo *kls, ptrdiff_t size, ptrdiff_t align,
                        ptrdiff_t count, Koliseo_Loc loc)
#endif // KOLISEO_HAS_LOCATE
{

#ifdef KLS_DEBUG_CORE
#ifndef _WIN32
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
#else
    LARGE_INTEGER start_time, end_time, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time);
#endif
#endif

    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if ((kls->has_temp == 1) && (kls->conf.kls_block_while_has_temp == 1)) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo has an open Koliseo_Temp session.\n", __func__);
#else
        fprintf(stderr, "[ERROR] " KLS_Loc_Fmt "[%s()]: Passed Koliseo has an open Koliseo_Temp session.\n", KLS_Loc_Arg(loc), __func__);
        kls_log(kls, "ERROR", KLS_Loc_Fmt "[%s()]: Passed Koliseo has an open Koliseo_Temp session.", KLS_Loc_Arg(loc), __func__);
        exit(EXIT_FAILURE);
#endif // KOLISEO_HAS_LOCATE
        return NULL;
    }
#ifndef KOLISEO_HAS_LOCATE
    kls__check_available(kls, size, align, count);
#else
    kls__check_available_dbg(kls, size, align, count, loc);
#endif
    Koliseo* current = kls;
    while (current->next != NULL) {
        current = current->next;
    }
    ptrdiff_t padding = -current->offset & (align - 1);
    char *p = current->data + current->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    current->prev_offset = current->offset;
    current->offset += padding + size * count;
    char h_size[200];
    kls_formatSize(size * count, h_size, sizeof(h_size));
    //sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
    //kls_log("KLS",msg);
#ifdef KLS_DEBUG_CORE
    kls_log(current, "KLS", "Curr offset: { %p }.", current + current->offset);
    kls_log(current, "KLS", "API Level { %i } -> Pushed zeroes, size (%s) for KLS.",
            int_koliseo_version(), h_size);
    if (current->conf.kls_verbose_lvl > 0) {
        print_kls_2file(current->conf.kls_log_fp, current);
    }
    if (current->conf.kls_collect_stats == 1) {
#ifndef _WIN32
        clock_gettime(CLOCK_MONOTONIC, &end_time);	// %.9f
        double elapsed_time =
            (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec -
                start_time.tv_nsec) / 1e9;
#else
        QueryPerformanceCounter(&end_time);	// %.7f
        double elapsed_time =
            (double)(end_time.QuadPart -
                     start_time.QuadPart) / frequency.QuadPart;
#endif
        if (elapsed_time > current->stats.worst_pushcall_time) {
            current->stats.worst_pushcall_time = elapsed_time;
        }
    }
#endif
    if (current->conf.kls_collect_stats == 1) {
        current->stats.tot_pushes += 1;
    }
    return p;
}

/**
 * Takes a Koliseo pointer, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the Koliseo data field, or goes to exit() if the operation fails.
 * Notably, it zeroes the memory region.
 * @param kls The Koliseo at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @return A void pointer to the start of memory just pushed to the Koliseo, or NULL for errors.
 */
#ifndef KOLISEO_HAS_LOCATE
void *kls_push_zero_ext(Koliseo *kls, ptrdiff_t size, ptrdiff_t align,
                        ptrdiff_t count)
#else
void *kls_push_zero_ext_dbg(Koliseo *kls, ptrdiff_t size, ptrdiff_t align,
                            ptrdiff_t count, Koliseo_Loc loc)
#endif // KOLISEO_HAS_LOCATE
{

#ifdef KLS_DEBUG_CORE
#ifndef _WIN32
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
#else
    LARGE_INTEGER start_time, end_time, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time);
#endif
#endif

    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if ((kls->has_temp == 1) && (kls->conf.kls_block_while_has_temp == 1)) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo has an open Koliseo_Temp session.\n", __func__);
#else
        fprintf(stderr, "[ERROR] " KLS_Loc_Fmt "[%s()]: Passed Koliseo has an open Koliseo_Temp session.\n", KLS_Loc_Arg(loc), __func__);
        kls_log(kls, "ERROR", KLS_Loc_Fmt "[%s()]: Passed Koliseo has an open Koliseo_Temp session.", KLS_Loc_Arg(loc), __func__);
        exit(EXIT_FAILURE);
#endif // KOLISEO_HAS_LOCATE
        return NULL;
    }

#ifndef KOLISEO_HAS_LOCATE
    kls__check_available(kls, size, align, count);
#else
    kls__check_available_dbg(kls, size, align, count, loc);
#endif
    Koliseo* current = kls;
    while (current->next != NULL) {
        current = current->next;
    }
    ptrdiff_t padding = -current->offset & (align - 1);
    char *p = current->data + current->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    current->prev_offset = current->offset;
    current->offset += padding + size * count;

    for (size_t i=0; i < kls->hooks_len; i++) {
        if (current->hooks[i].on_push_handler != NULL) {
            /*
            struct KLS_EXTENSION_AR_DEFAULT_ARGS {
                const char* region_name;
                size_t region_name_len;
                const char* region_desc;
                size_t region_desc_len;
                int region_type;
            };
            struct KLS_EXTENSION_AR_DEFAULT_ARGS ar_args = {
                .region_name = KOLISEO_DEFAULT_REGION_NAME,
                .region_name_len = strlen(KOLISEO_DEFAULT_REGION_NAME),
                .region_desc = KOLISEO_DEFAULT_REGION_DESC,
                .region_desc_len = strlen(KOLISEO_DEFAULT_REGION_DESC),
                .region_type = KLS_None
            };
            kls->hooks.on_push_handler(kls, padding, (void*)&ar_args);
            */
            current->hooks[i].on_push_handler(current, padding, __func__, NULL);
        }
    }

    char h_size[200];
    kls_formatSize(size * count, h_size, sizeof(h_size));
    //sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
    //kls_log("KLS",msg);
#ifdef KLS_DEBUG_CORE
    kls_log(current, "KLS", "Curr offset: { %p }.", current + current->offset);
    kls_log(current, "KLS", "API Level { %i } -> Pushed zeroes, size (%s) for KLS.",
            int_koliseo_version(), h_size);
    if (current->conf.kls_verbose_lvl > 0) {
        print_kls_2file(current->conf.kls_log_fp, current);
    }
    if (current->conf.kls_collect_stats == 1) {
#ifndef _WIN32
        clock_gettime(CLOCK_MONOTONIC, &end_time);	// %.9f
        double elapsed_time =
            (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec -
                start_time.tv_nsec) / 1e9;
#else
        QueryPerformanceCounter(&end_time);	// %.7f
        double elapsed_time =
            (double)(end_time.QuadPart -
                     start_time.QuadPart) / frequency.QuadPart;
#endif
        if (elapsed_time > current->stats.worst_pushcall_time) {
            current->stats.worst_pushcall_time = elapsed_time;
        }
    }
#endif
    if (current->conf.kls_collect_stats == 1) {
        current->stats.tot_pushes += 1;
    }
    return p;
}

#ifndef KOLISEO_HAS_LOCATE
char* kls_vsprintf(Koliseo* kls, const char* fmt, va_list args)
#else
char* kls_vsprintf_dbg(Koliseo* kls, Koliseo_Loc loc, const char* fmt, va_list args)
#endif // KOLISEO_HAS_LOCATE
{
    va_list args_copy;
    va_copy(args_copy, args);
    int len = vsnprintf(NULL, 0, fmt, args);
#ifndef KOLISEO_HAS_LOCATE
    char* str = KLS_PUSH_ARR(kls, char, len+1);
#else
    char* str = kls_push_zero_ext_dbg(kls, sizeof(char), KLS_ALIGNOF(char), len+1, loc);
#endif // KOLISEO_HAS_LOCATE
    vsnprintf(str, len+1, fmt, args_copy);
    va_end(args_copy);
    return str;
}

/**
 * Takes a Koliseo pointer, and a format cstring, plus varargs. Tries using the passed Koliseo to hold the result of standard formatting using the varargs.
 * @param kls The Koliseo at hand.
 * @param fmt The format cstring.
 * @return A char pointer to the start of memory just pushed to the Koliseo, or NULL for errors.
 */
#ifndef KOLISEO_HAS_LOCATE
char* kls_sprintf(Koliseo* kls, const char* fmt, ...)
#else
char* kls_sprintf_dbg(Koliseo* kls, Koliseo_Loc loc, const char* fmt, ...)
#endif // KOLISEO_HAS_LOCATE
{
    va_list args;
    va_start(args, fmt);
#ifndef KOLISEO_HAS_LOCATE
    char* str = kls_vsprintf(kls, fmt, args);
#else
    char* str = kls_vsprintf_dbg(kls, loc, fmt, args);
#endif // KOLISEO_HAS_LOCATE
    va_end(args);
    return str;
}

/**
 * Takes a Koliseo_Temp, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the referred Koliseo data field, or goes to exit() if the operation fails.
 * Notably, it zeroes the memory region.
 * Will be removed in 0.6, leaving kls_temp_push_zero_ext() as its replacement.
 * @param t_kls The Koliseo_Temp at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @return A void pointer to the start of memory just pushed to the referred Koliseo.
 */
#ifndef KOLISEO_HAS_LOCATE
void *kls_temp_push_zero_ext(Koliseo_Temp *t_kls, ptrdiff_t size,
                             ptrdiff_t align, ptrdiff_t count)
#else
void *kls_temp_push_zero_ext_dbg(Koliseo_Temp *t_kls, ptrdiff_t size,
                                 ptrdiff_t align, ptrdiff_t count, Koliseo_Loc loc)
#endif // KOLISEO_HAS_LOCATE
{

#ifdef KLS_DEBUG_CORE
#ifndef _WIN32
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
#else
    LARGE_INTEGER start_time, end_time, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time);
#endif
#endif

    if (t_kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo_Temp was NULL.\n",
                __func__);
        exit(EXIT_FAILURE);
    }
    Koliseo *kls = t_kls->kls;
    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Referred Koliseo was NULL.\n",
                __func__);
        exit(EXIT_FAILURE);
    }
#ifndef KOLISEO_HAS_LOCATE
    kls__check_available(kls, size, align, count);
#else
    kls__check_available_dbg(kls, size, align, count, loc);
#endif
    Koliseo* current = kls;
    while (current->next != NULL) {
        current = current->next;
    }
    ptrdiff_t padding = -current->offset & (align - 1);
    char *p = current->data + current->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    current->prev_offset = current->offset;
    current->offset += padding + size * count;

    for (size_t i=0; i < kls->hooks_len; i++) {
        if (current->hooks[i].on_temp_push_handler != NULL) {
            // Call on_temp_push extension with empty user arg
            current->hooks[i].on_temp_push_handler(t_kls, padding, __func__, NULL);
        }
    }

    char h_size[200];
    kls_formatSize(size * count, h_size, sizeof(h_size));
    //sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
    //kls_log("KLS",msg);
#ifdef KLS_DEBUG_CORE
    if (current->conf.kls_collect_stats == 1) {
#ifndef _WIN32
        clock_gettime(CLOCK_MONOTONIC, &end_time);	// %.9f
        double elapsed_time =
            (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec -
                start_time.tv_nsec) / 1e9;
#else
        QueryPerformanceCounter(&end_time);	// %.7f
        double elapsed_time =
            (double)(end_time.QuadPart -
                     start_time.QuadPart) / frequency.QuadPart;
#endif
        if (elapsed_time > current->stats.worst_pushcall_time) {
            current->stats.worst_pushcall_time = elapsed_time;
        }
    }
    kls_log(current, "KLS", "Curr offset: { %p }.", current + current->offset);
    kls_log(current, "KLS",
            "API Level { %i } -> Pushed zeroes, size (%s) for Temp_KLS.",
            int_koliseo_version(), h_size);
    if (current->conf.kls_verbose_lvl > 0) {
        print_kls_2file(current->conf.kls_log_fp, current);
    }
#endif
    if (current->conf.kls_collect_stats == 1) {
        current->stats.tot_temp_pushes += 1;
    }
    return p;
}

#ifndef KOLISEO_HAS_LOCATE
char* kls_temp_vsprintf(Koliseo_Temp* kls_t, const char* fmt, va_list args)
#else
char* kls_temp_vsprintf_dbg(Koliseo_Temp* kls_t, Koliseo_Loc loc, const char* fmt, va_list args)
#endif // KOLISEO_HAS_LOCATE
{
    va_list args_copy;
    va_copy(args_copy, args);
    int len = vsnprintf(NULL, 0, fmt, args);
#ifndef KOLISEO_HAS_LOCATE
    char* str = KLS_PUSH_ARR_T(kls_t, char, len+1);
#else
    char* str = kls_temp_push_zero_ext_dbg(kls_t, sizeof(char), KLS_ALIGNOF(char), len+1, loc);
#endif // KOLISEO_HAS_LOCATE
    vsnprintf(str, len+1, fmt, args_copy);
    va_end(args_copy);
    return str;
}


/**
 * Takes a Koliseo_Temp pointer, and a format cstring, plus varargs. Tries using the passed Koliseo_Temp to hold the result of standard formatting using the varargs.
 * @param kls_t The Koliseo_Temp at hand.
 * @param fmt The format cstring.
 * @return A char pointer to the start of memory just pushed to the Koliseo_Temp, or NULL for errors.
 */
#ifndef KOLISEO_HAS_LOCATE
char* kls_temp_sprintf(Koliseo_Temp* kls_t, const char* fmt, ...)
#else
char* kls_temp_sprintf_dbg(Koliseo_Temp* kls_t, Koliseo_Loc loc, const char* fmt, ...)
#endif // KOLISEO_HAS_LOCATE
{
    va_list args;
    va_start(args, fmt);
#ifndef KOLISEO_HAS_LOCATE
    char* str = kls_temp_vsprintf(kls_t, fmt, args);
#else
    char* str = kls_temp_vsprintf_dbg(kls_t, loc, fmt, args);
#endif // KOLISEO_HAS_LOCATE
    va_end(args);
    return str;
}

/**
 * Takes a Koliseo pointer, and a void pointer to the old allocation, ptrdiff_t values for size, align and old and new count. Tries repushing the specified amount of memory to the Koliseo data field.
 * Notably, it zeroes the memory region.
 * @param kls The Koliseo at hand.
 * @param old The old allocation.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param old_count The multiplicative quantity to scale data size of old allocation.
 * @param new_count The multiplicative quantity to scale data size of new allocation.
 * @return A void pointer to the start of memory just pushed to the Koliseo, or NULL for errors.
 */
#ifndef KOLISEO_HAS_LOCATE
void *kls_repush(Koliseo *kls, void* old, ptrdiff_t size, ptrdiff_t align,
                 ptrdiff_t old_count, ptrdiff_t new_count)
#else
void *kls_repush_dbg(Koliseo *kls, void* old, ptrdiff_t size, ptrdiff_t align,
                     ptrdiff_t old_count, ptrdiff_t new_count, Koliseo_Loc loc)
#endif // KOLISEO_HAS_LOCATE
{
    if (!old) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): old was NULL.\n",
                __func__);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): old was NULL.\n",
                KLS_Loc_Arg(loc),
                __func__);
#endif // KOLISEO_HAS_LOCATE
        return NULL;
    }
    if (old_count < 0) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): old_count [%td] was < 0.\n",
                __func__,
                old_count);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): old_count [%td] was < 0.\n",
                KLS_Loc_Arg(loc),
                __func__,
                old_count);
#endif // KOLISEO_HAS_LOCATE
        return NULL;
    }
    if (new_count < 0) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): new_count [%td] was < 0.\n",
                __func__,
                new_count);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): new_count [%td] was < 0.\n",
                KLS_Loc_Arg(loc),
                __func__,
                new_count);
#endif // KOLISEO_HAS_LOCATE
        return NULL;
    }
    if (size < 1) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): size [%td] was < 1.\n",
                __func__,
                size);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): size [%td] was < 1.\n",
                KLS_Loc_Arg(loc),
                __func__,
                size);
#endif // KOLISEO_HAS_LOCATE
        return NULL;
    }
    if (align < 1) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): align [%td] was < 1.\n",
                __func__,
                align);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): align [%td] was < 1.\n",
                KLS_Loc_Arg(loc),
                __func__,
                align);
#endif // KOLISEO_HAS_LOCATE
        return NULL;
    }
    if (! ((align & (align - 1)) == 0)) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): align [%td] was not a power of 2.\n",
                __func__,
                align);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): align [%td] was not a power of 2.\n",
                KLS_Loc_Arg(loc),
                __func__,
                align);
#endif // KOLISEO_HAS_LOCATE
        return NULL;
    }
    if (kls->has_temp == 1 && kls->conf.kls_block_while_has_temp == 1) {
        fprintf(stderr, "%s(): kls has an active temp\n", __func__);
        return NULL;
    }
    size_t old_size = old_count * size;
    size_t new_size = new_count * size;
    void *new_ptr = kls_push_zero_ext(kls, size, align, new_count);
    if (new_ptr && old_size > 0) {
        memcpy(new_ptr, old, old_size < new_size ? old_size : new_size);
    }
    return new_ptr;
}

/**
 * Takes a Koliseo_Temp pointer, and a void pointer to the old allocation, ptrdiff_t values for size, align and old and new count. Tries repushing the specified amount of memory to the Koliseo_Temp data field.
 * Notably, it zeroes the memory region.
 * @param t_kls The Koliseo_Temp at hand.
 * @param old The old allocation.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param old_count The multiplicative quantity to scale data size of old allocation.
 * @param new_count The multiplicative quantity to scale data size of new allocation.
 * @return A void pointer to the start of memory just pushed to the Koliseo_Temp, or NULL for errors.
 */
#ifndef KOLISEO_HAS_LOCATE
void *kls_temp_repush(Koliseo_Temp *t_kls, void* old, ptrdiff_t size, ptrdiff_t align,
                      ptrdiff_t old_count, ptrdiff_t new_count)
#else
void *kls_temp_repush_dbg(Koliseo_Temp *t_kls, void* old, ptrdiff_t size, ptrdiff_t align,
                          ptrdiff_t old_count, ptrdiff_t new_count, Koliseo_Loc loc)
#endif // KOLISEO_HAS_LOCATE
{
    if (!old) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): old was NULL.\n",
                __func__);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): old was NULL.\n",
                KLS_Loc_Arg(loc),
                __func__);
#endif // KOLISEO_HAS_LOCATE
        return NULL;
    }
    if (old_count < 0) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): old_count [%td] was < 0.\n",
                __func__,
                old_count);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): old_count [%td] was < 0.\n",
                KLS_Loc_Arg(loc),
                __func__,
                old_count);
#endif // KOLISEO_HAS_LOCATE
        return NULL;
    }
    if (new_count < 0) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): new_count [%td] was < 0.\n",
                __func__,
                new_count);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): new_count [%td] was < 0.\n",
                KLS_Loc_Arg(loc),
                __func__,
                new_count);
#endif // KOLISEO_HAS_LOCATE
        return NULL;
    }
    if (size < 1) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): size [%td] was < 1.\n",
                __func__,
                size);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): size [%td] was < 1.\n",
                KLS_Loc_Arg(loc),
                __func__,
                size);
#endif // KOLISEO_HAS_LOCATE
        return NULL;
    }
    if (align < 1) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): align [%td] was < 1.\n",
                __func__,
                align);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): align [%td] was < 1.\n",
                KLS_Loc_Arg(loc),
                __func__,
                align);
#endif // KOLISEO_HAS_LOCATE
        return NULL;
    }
    if (! ((align & (align - 1)) == 0)) {
#ifndef KOLISEO_HAS_LOCATE
        fprintf(stderr,
                "[KLS] %s(): align [%td] was not a power of 2.\n",
                __func__,
                align);
#else
        fprintf(stderr,
                "[KLS] " KLS_Loc_Fmt "%s(): align [%td] was not a power of 2.\n",
                KLS_Loc_Arg(loc),
                __func__,
                align);
#endif // KOLISEO_HAS_LOCATE
        return NULL;
    }
    size_t old_size = old_count * size;
    size_t new_size = new_count * size;
    void *new_ptr = kls_temp_push_zero_ext(t_kls, size, align, new_count);
    if (new_ptr && old_size > 0) {
        memcpy(new_ptr, old, old_size < new_size ? old_size : new_size);
    }
    return new_ptr;
}

/**
 * Prints header fields from the passed Koliseo pointer, to the passed FILE pointer.
 * @param fp The FILE pointer to print to.
 * @param kls The Koliseo at hand.
 */
void print_kls_2file(FILE *fp, const Koliseo *kls)
{
    if (fp == NULL) {
        fprintf(stderr, "print_kls_2file():  fp was NULL.\n");
        return;
    }
    if (kls == NULL) {
        fprintf(fp, "[KLS] kls was NULL.\n");
    } else {
        fprintf(fp, "\n[KLS] API Level: { %i }\n", int_koliseo_version());
        fprintf(fp, "\n[INFO] Conf: { " KLS_Conf_Fmt " }\n",
                KLS_Conf_Arg(kls->conf));
        fprintf(fp, "\n[INFO] Stats: { " KLS_Stats_Fmt " }\n",
                KLS_Stats_Arg(kls->stats));
        fprintf(fp, "\n[KLS] Size: { %td }\n", kls->size);
        char human_size[200];
        char curr_size[200];
        kls_formatSize(kls->size, human_size, sizeof(human_size));
        fprintf(fp, "[KLS] Size (Human): { %s }\n", human_size);
        kls_formatSize(kls->offset, curr_size, sizeof(curr_size));
        fprintf(fp, "[KLS] Used (Human): { %s }\n", curr_size);
        fprintf(fp, "[KLS] Offset: { %td }\n", kls->offset);
        fprintf(fp, "[KLS] Prev_Offset: { %td }\n", kls->prev_offset);
        fprintf(fp, "\n");
    }
}

/**
 * Prints header fields from the passed Koliseo pointer, to stderr.
 * @param kls The Koliseo at hand.
 */
void print_dbg_kls(const Koliseo *kls)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    print_kls_2file(stderr, kls);
}

/**
 * Prints header fields from the passed Koliseo_Temp pointer, to the passed FILE pointer.
 * @param fp The FILE pointer to print to.
 * @param t_kls The Koliseo_Temp at hand.
 */
void print_temp_kls_2file(FILE *fp, const Koliseo_Temp *t_kls)
{
    if (fp == NULL) {
        fprintf(stderr, "print_temp_kls_2file():  fp was NULL.\n");
        exit(EXIT_FAILURE);
    }
    if (t_kls == NULL) {
        fprintf(fp, "[KLS_T] t_kls was NULL.");
    } else if (t_kls->kls == NULL) {
        fprintf(fp, "[KLS_T] [%s()]: Referred Koliseo was NULL.\n", __func__);
    } else {
        const Koliseo *kls = t_kls->kls;
        fprintf(fp, "\n[KLS_T] API Level: { %i }\n", int_koliseo_version());
        fprintf(fp, "\n[KLS_T] Temp Size: { %td }\n",
                kls->size - t_kls->offset);
        fprintf(fp, "\n[KLS_T] Refer Size: { %td }\n", kls->size);
        char human_size[200];
        char curr_size[200];
        kls_formatSize(kls->size - t_kls->offset, human_size,
                       sizeof(human_size));
        fprintf(fp, "[KLS_T] Temp Size Human: { %s }\n", human_size);
        kls_formatSize(kls->size, human_size, sizeof(human_size));
        fprintf(fp, "[KLS_T] Refer Size Human: { %s }\n", human_size);
        kls_formatSize(kls->offset, curr_size, sizeof(curr_size));
        fprintf(fp, "[KLS_T] Inner Used (Human): { %s }\n", curr_size);
        kls_formatSize(t_kls->offset, curr_size, sizeof(curr_size));
        fprintf(fp, "[KLS_T] Temp Used (Human): { %s }\n", curr_size);
        fprintf(fp, "[KLS_T] Inner Offset: { %td }\n", kls->offset);
        fprintf(fp, "[KLS_T] Temp Offset: { %td }\n", t_kls->offset);
        fprintf(fp, "[KLS_T] Inner Prev_Offset: { %td }\n", kls->prev_offset);
        fprintf(fp, "[KLS_T] Temp Prev_Offset: { %td }\n\n",
                t_kls->prev_offset);
    }
}

/**
 * Prints header fields from the passed Koliseo_Temp pointer, to stderr.
 * @param t_kls The Koliseo_Temp at hand.
 */
void print_dbg_temp_kls(const Koliseo_Temp *t_kls)
{
    print_temp_kls_2file(stderr, t_kls);
}

/**
 * Converts a ptrdiff_t size to human-readable SI units (modulo 1000).
 * Fills outputBuffer with the converted string.
 * @param size The size at hand.
 * @param outputBuffer The output buffer.
 * @param bufferSize The output buffer size.
 */
void kls_formatSize(ptrdiff_t size, char *outputBuffer, size_t bufferSize)
{
    const char *units[] =
    { "bytes", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };
    const int numUnits = sizeof(units) / sizeof(units[0]);

    int unitIndex = 0;
    double sizeValue = (double)size;

    while (sizeValue >= 1000 && unitIndex < numUnits - 1) {
        sizeValue /= 1000;
        unitIndex++;
    }

    snprintf(outputBuffer, bufferSize, "%.2f %s", sizeValue, units[unitIndex]);
}

/**
 * Resets the offset field for the passed Koliseo pointer.
 * Notably, it sets the prev_offset field to the previous offset, thus remembering where last allocation was before the clear.
 * @param kls The Koliseo at hand.
 */
void kls_clear(Koliseo *kls)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    //Reset pointer
    kls->prev_offset = kls->offset;
    kls->offset = sizeof(*kls);
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "API Level { %i } -> Cleared offsets for KLS.",
            int_koliseo_version());
#endif
}

/**
 * Calls kls_clear() on the passed Koliseo pointer and the frees the actual Koliseo.
 * @param kls The Koliseo at hand.
 * @see kls_clear()
 */
void kls_free(Koliseo *kls)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    Koliseo* current = kls;
    while (current) {
        Koliseo* next = current->next;
        current->next = NULL;
        for (size_t i=0; i < current->hooks_len; i++) {
            if (current->hooks[i].on_free_handler != NULL) {
                // Call on_free() extension
                current->hooks[i].on_free_handler(current);
            }
        }
        if (current->has_temp == 1) {
#ifdef KLS_DEBUG_CORE
            kls_log(current, "KLS",
                    "API Level { %i } -> KLS had an active Koliseo_Temp.",
                    int_koliseo_version());
#endif
            kls_temp_end(current->t_kls);
        }
        kls_clear(current);
#ifdef KLS_DEBUG_CORE
        kls_log(current, "KLS", "API Level { %i } -> Freeing KLS.",
                int_koliseo_version());
#endif
        if (current->conf.kls_log_fp != NULL && current->conf.kls_log_fp != stdout
            && current->conf.kls_log_fp != stderr) {
#ifdef KLS_DEBUG_CORE
            kls_log(current, "KLS", "Closing kls log file. Path: {\"%s\"}.",
                    kls->conf.kls_log_filepath);
#endif
            int close_res = fclose(current->conf.kls_log_fp);
            if (close_res != 0) {
                fprintf(stderr,
                        "[ERROR]    %s(): Failed fclose() on log_fp. Path: {\"%s\"}.",
                        __func__, current->conf.kls_log_filepath);
            }
        } else if (current->conf.kls_log_fp == stdout || current->conf.kls_log_fp == stderr) {
            if (current->conf.kls_verbose_lvl > 1) {
                fprintf(stderr,
                        "[INFO]    %s(): kls->conf.kls_log_fp is %s. Not closing it.\n",
                        __func__,
                        (current->conf.kls_log_fp == stdout ? "stdout" : "stderr"));
            }
        }
        if (current->free_func == NULL) {
            fprintf(stderr,
                    "[ERROR]    %s(): free function was NULL.\n", __func__);
            return;
        }
        current->free_func(current);
        current = next;
    }
}

/**
 * Starts a new savestate for the passed Koliseo pointer, by initialising its Koliseo_Temp pointer and returning it.
 * Notably, you should not use the original while using the copy.
 * @param kls The Koliseo at hand.
 * @return A Koliseo_Temp struct.
 * @see Koliseo_Temp
 */
#ifndef KOLISEO_HAS_LOCATE
Koliseo_Temp *kls_temp_start(Koliseo *kls)
#else
Koliseo_Temp *kls_temp_start_dbg(Koliseo *kls, Koliseo_Loc loc)
#endif // KOLISEO_HAS_LOCATE
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    Koliseo* current = kls;
    while (current->next != NULL) {
        current = current->next;
    }
    if (current->has_temp != 0) {
        fprintf(stderr,
                "[ERROR] [%s()]: Passed Koliseo->has_temp is not 0. {%i}\n",
                __func__, current->has_temp);
#ifdef KLS_DEBUG_CORE
        kls_log(current, "ERROR", "[%s()]: Passed Koliseo->has_temp != 0 . {%i}",
                __func__, current->has_temp);
#endif
        if (current->conf.kls_collect_stats == 1) {
            current->stats.tot_hiccups += 1;
        }
        return NULL;
    }
    ptrdiff_t prev = current->prev_offset;
    ptrdiff_t off = current->offset;

    Koliseo_Temp *tmp = KLS_PUSH(current, Koliseo_Temp);
    tmp->kls = current;
    tmp->prev_offset = prev;
    tmp->offset = off;
#ifdef KLS_DEBUG_CORE
    kls_log(current, "INFO", "Passed kls conf: " KLS_Conf_Fmt "\n",
            KLS_Conf_Arg(current->conf));
#endif

    current->has_temp = 1;
    current->t_kls = tmp;
    for (size_t i=0; i < kls->hooks_len; i++) {
        if (current->hooks[i].on_temp_start_handler != NULL) {
            // Call on_temp_start extension
            current->hooks[i].on_temp_start_handler(tmp);
        }
    }
#ifdef KLS_DEBUG_CORE
    kls_log(current, "KLS", "Prepared new Temp KLS.");
#endif
    return tmp;
}

/**
 * Ends passed Koliseo_Temp pointer.
 * @param tmp_kls The Koliseo_Temp at hand.
 */
void kls_temp_end(Koliseo_Temp *tmp_kls)
{
    if (tmp_kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo_Temp was NULL.\n",
                __func__);
        exit(EXIT_FAILURE);
    }

    Koliseo *kls_ref = tmp_kls->kls;
    if (kls_ref == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Referred Koliseo was NULL.\n",
                __func__);
        exit(EXIT_FAILURE);
    }

    for (size_t i=0; i < kls_ref->hooks_len; i++) {
        if (kls_ref->hooks[i].on_temp_free_handler != NULL) {
            // Call on_temp_free() extension
            kls_ref->hooks[i].on_temp_free_handler(tmp_kls);
        }
    }

#ifdef KLS_DEBUG_CORE
    kls_log(kls_ref, "KLS", "Ended Temp KLS.");
#endif
    tmp_kls->kls->has_temp = 0;
    tmp_kls->kls->t_kls = NULL;
    tmp_kls->kls->prev_offset = tmp_kls->prev_offset;
    tmp_kls->kls->offset = tmp_kls->offset;

    // Free any Koliseo chained after the current one
    Koliseo* to_free = tmp_kls->kls->next;
    if (to_free != NULL) {
        kls_free(to_free);
        tmp_kls->kls->next = NULL;
    }

    tmp_kls = NULL; // statement with no effect TODO: Clear tmp_kls from caller
    if (kls_ref->conf.kls_collect_stats == 1) {
        kls_ref->stats.tot_temp_pushes = 0;
        kls_ref->stats.tot_temp_pops = 0;
    }
}

#ifdef KOLISEO_HAS_EXPER
/**
 * Takes a Koliseo pointer, and ptrdiff_t values for size, align and count. Tries popping the specified amount of memory from the Koliseo data field, marking it as free (as far as Koliseo is concerned), or goes to exit() if the operation fails.
 * @param kls The Koliseo at hand.
 * @param size The size for data to pop.
 * @param align The alignment for data to pop.
 * @param count The multiplicative quantity to scale data size to pop for.
 * @return A void pointer to the start of memory just popped from the Koliseo.
 */
void *kls_pop(Koliseo *kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    ptrdiff_t padding = -kls->offset & (align - 1);
    if (count > PTRDIFF_MAX / size
        || (kls->size + kls->offset) < (size * count)) {
        fprintf(stderr, "[KLS] Failed %s() call.\n", __func__);
        kls_free(kls);
        exit(EXIT_FAILURE);
    }
    char *p = kls->data + kls->offset - padding - size * count;
    kls->prev_offset = kls->offset;
    kls->offset -= padding + size * count;
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "API Level { %i } -> Popped (%li) for KLS.",
            int_koliseo_version(), size);
    if (kls->conf.kls_verbose_lvl > 0) {
        print_kls_2file(kls->conf.kls_log_fp, kls);
    }
#endif
    if (kls->conf.kls_collect_stats == 1) {
        kls->stats.tot_pops += 1;
    }
    return p;
}

/**
 * Takes a Koliseo pointer, and ptrdiff_t values for size, align and count. Tries popping the specified amount of memory from the Koliseo data field, marking it as free (as far as Koliseo is concerned), or goes to exit() if the operation fails.
 * @param kls The Koliseo at hand.
 * @param size The size for data to pop.
 * @param align The alignment for data to pop.
 * @param count The multiplicative quantity to scale data size to pop for.
 * @return A void pointer to the start of memory just popped from the Koliseo.
 */
void *kls_pop_AR(Koliseo *kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    ptrdiff_t padding = -kls->offset & (align - 1);
    if (count > PTRDIFF_MAX / size
        || (kls->size + kls->offset) < (size * count)) {
        fprintf(stderr, "[KLS] Failed %s() call.\n", __func__);
        kls_free(kls);
        exit(EXIT_FAILURE);
    }
    char *p = kls->data + kls->offset - padding - size * count;
    kls->prev_offset = kls->offset;
    kls->offset -= padding + size * count;
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "API Level { %i } -> Popped (%li) for KLS.",
            int_koliseo_version(), size);
    if (kls->conf.kls_verbose_lvl > 0) {
        print_kls_2file(kls->conf.kls_log_fp, kls);
    }
#endif
    if (kls->conf.kls_collect_stats == 1) {
        kls->stats.tot_pops += 1;
    }
    return p;
}

/**
 * Takes a Koliseo_Temp, and ptrdiff_t values for size, align and count. Tries popping the specified amount of memory from the Koliseo data field, marking it as free (as far as Koliseo is concerned), or goes to exit() if the operation fails.
 * @param t_kls The Koliseo_Temp at hand.
 * @param size The size for data to pop.
 * @param align The alignment for data to pop.
 * @param count The multiplicative quantity to scale data size to pop for.
 * @return A void pointer to the start of memory just popped from the referred Koliseo.
 */
void *kls_temp_pop(Koliseo_Temp *t_kls, ptrdiff_t size, ptrdiff_t align,
                   ptrdiff_t count)
{
    if (t_kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo_Temp was NULL.\n",
                __func__);
        exit(EXIT_FAILURE);
    }
    Koliseo *kls = t_kls->kls;
    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Referred Koliseo was NULL.\n",
                __func__);
        exit(EXIT_FAILURE);
    }
    ptrdiff_t padding = -kls->offset & (align - 1);
    if (count > PTRDIFF_MAX / size
        || (kls->size + kls->offset) < (size * count)) {
        fprintf(stderr, "[KLS] Failed %s() call.\n", __func__);
        kls_free(kls);
        exit(EXIT_FAILURE);
    }
    char *p = kls->data + kls->offset - padding - size * count;
    kls->prev_offset = kls->offset;
    kls->offset -= padding + size * count;
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "Curr offset: { %p }.", kls + kls->offset);
    kls_log(kls, "KLS", "API Level { %i } -> Popped (%li) for Temp_KLS.",
            int_koliseo_version(), size);
    if (kls->conf.kls_verbose_lvl > 0) {
        print_kls_2file(kls->conf.kls_log_fp, kls);
    }
#endif
    if (kls->conf.kls_collect_stats == 1) {
        kls->stats.tot_temp_pops += 1;
    }
    return p;
}

/**
 * Takes a Koliseo_Temp, and ptrdiff_t values for size, align and count. Tries popping the specified amount of memory from the Koliseo data field, marking it as free (as far as Koliseo is concerned), or goes to exit() if the operation fails.
 * @param t_kls The Koliseo_Temp at hand.
 * @param size The size for data to pop.
 * @param align The alignment for data to pop.
 * @param count The multiplicative quantity to scale data size to pop for.
 * @return A void pointer to the start of memory just popped from the referred Koliseo.
 */
void *kls_temp_pop_AR(Koliseo_Temp *t_kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count)
{
    if (t_kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo_Temp was NULL.\n",
                __func__);
        exit(EXIT_FAILURE);
    }
    Koliseo *kls = t_kls->kls;
    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Referred Koliseo was NULL.\n",
                __func__);
        exit(EXIT_FAILURE);
    }
    ptrdiff_t padding = -kls->offset & (align - 1);
    if (count > PTRDIFF_MAX / size
        || (kls->size + kls->offset) < (size * count)) {
        fprintf(stderr, "[KLS] Failed %s() call.\n", __func__);
        kls_free(kls);
        exit(EXIT_FAILURE);
    }
    char *p = kls->data + kls->offset - padding - size * count;
    kls->prev_offset = kls->offset;
    kls->offset -= padding + size * count;
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "Curr offset: { %p }.", kls + kls->offset);
    kls_log(kls, "KLS", "API Level { %i } -> Popped (%li) for Temp_KLS.",
            int_koliseo_version(), size);
    if (kls->conf.kls_verbose_lvl > 0) {
        print_kls_2file(kls->conf.kls_log_fp, kls);
    }
#endif
    if (kls->conf.kls_collect_stats == 1) {
        kls->stats.tot_temp_pops += 1;
    }
    return p;
}

/**
 * Function to dupe a C string to a Koliseo, and return a pointer to the allocated string.
 * Unsafe, do not use.
 * @see KLS_PUSH_STR()
 * @see KLS__STRCPY()
 */
char* kls_strdup(Koliseo* kls, char* source)
{
    char* dest = KLS_PUSH_STR(kls, source);
    KLS__STRCPY(dest, source);
    return dest;
}

/**
 * Function to dupe a C string array to a Koliseo, and return a pointer to the allocated array.
 * Unsafe, do not use.
 * @see KLS_STRDUP()
 * @see KLS__STRCPY()
 */
char** kls_strdup_arr(Koliseo* kls, size_t count, char** source)
{
    char** strings = NULL;
    strings = KLS_PUSH_ARR(kls, char*, count);
    for (int i=0; i < count; i++) {
        strings[i] = KLS_STRDUP(kls, source[i]);
    }
    return strings;
}

/**
 * Function to dupe a C string to a Koliseo_Temp, and return a pointer to the allocated string.
 * Unsafe, do not use.
 * @see KLS_PUSH_STR_T()
 * @see KLS__STRCPY()
 */
char* kls_t_strdup(Koliseo_Temp* t_kls, char* source)
{
    char* dest = KLS_PUSH_STR_T(t_kls, source);
    KLS__STRCPY(dest, source);
    return dest;
}

/**
 * Function to dupe a C string array to a Koliseo_Temp, and return a pointer to the allocated array.
 * Unsafe, do not use.
 * @see KLS_STRDUP_T()
 * @see KLS__STRCPY()
 */
char** kls_t_strdup_arr(Koliseo_Temp* t_kls, size_t count, char** source)
{
    char** strings = NULL;
    strings = KLS_PUSH_ARR_T(t_kls, char*, count);
    for (int i=0; i < count; i++) {
        strings[i] = KLS_STRDUP_T(t_kls, source[i]);
    }
    return strings;
}
#endif // KOLISEO_HAS_EXPER
