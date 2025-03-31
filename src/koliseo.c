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

#ifdef KOLISEO_HAS_REGION
static const KLS_Conf KLS_DEFAULT_CONF__ = {
    .kls_autoset_regions = 0,
    .kls_reglist_alloc_backend = KLS_REGLIST_ALLOC_LIBC,
    .kls_reglist_kls_size = 0,
    .kls_autoset_temp_regions = 0,
    .kls_collect_stats = 0,
    .kls_verbose_lvl = 0,
    .kls_block_while_has_temp = 1,
    .kls_allow_zerocount_push = 0,
    .kls_log_fp = NULL,
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
}; /**< Inner config used for any Koliseo used to host the regions for another Koliseo in the KLS_BASIC config.*/
#endif

KLS_Conf KLS_DEFAULT_CONF = {
#ifdef KOLISEO_HAS_REGION
    .kls_autoset_regions = 0,
    .kls_reglist_alloc_backend = KLS_REGLIST_ALLOC_LIBC,
    .kls_reglist_kls_size = 0,
    .kls_autoset_temp_regions = 0,
#endif // KOLISEO_HAS_REGION
    .kls_collect_stats = 0,
    .kls_verbose_lvl = 0,
    .kls_block_while_has_temp = 1,
    .kls_allow_zerocount_push = 0,
    .kls_log_fp = NULL,
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
#ifdef KOLISEO_HAS_REGION
    .avg_region_size = 0,
#endif
#ifdef KLS_DEBUG_CORE
    .worst_pushcall_time = -1,
#endif
};

#ifdef KOLISEO_HAS_REGION
const char* kls_reglist_backend_strings[KLS_REGLIST_TOTAL_BACKENDS] = {
    [KLS_REGLIST_ALLOC_LIBC] = "LIBC",
    [KLS_REGLIST_ALLOC_KLS_BASIC] = "KLS_BASIC",
};
#endif

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

#ifdef KOLISEO_HAS_REGION
const char* kls_reglist_backend_string(KLS_RegList_Alloc_Backend kls_be)
{
    switch(kls_be) {
    case KLS_REGLIST_ALLOC_LIBC:
    case KLS_REGLIST_ALLOC_KLS_BASIC: {
        return kls_reglist_backend_strings[kls_be];
    }
    break;
    default: {
        fprintf(stderr,"%s():    Unexpected kls_be: {%i}.\n", __func__, kls_be);
        return "";
    }
    break;
    }
}
#endif // KOLISEO_HAS_REGION

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
KLS_Conf kls_conf_init_handled(int autoset_regions, int alloc_backend, ptrdiff_t reglist_kls_size, int autoset_temp_regions, int collect_stats, int verbose_lvl, int block_while_has_temp, int allow_zerocount_push, FILE* log_fp, const char* log_filepath, KLS_Err_Handlers err_handlers)
{
    KLS_Conf res = {0};
#ifdef KOLISEO_HAS_REGION
    res.kls_autoset_regions = autoset_regions;
    res.kls_reglist_alloc_backend = alloc_backend;
    res.kls_reglist_kls_size = reglist_kls_size;
    res.kls_autoset_temp_regions = autoset_temp_regions;
#else
    (void) autoset_regions;
    (void) alloc_backend;
    (void) reglist_kls_size;
    (void) autoset_temp_regions;
#endif // KOLISEO_HAS_REGION
    res.kls_collect_stats = collect_stats;
    res.kls_verbose_lvl = verbose_lvl;
    res.kls_block_while_has_temp = block_while_has_temp;
    res.kls_allow_zerocount_push = allow_zerocount_push;
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
KLS_Conf kls_conf_init(int autoset_regions, int alloc_backend, ptrdiff_t reglist_kls_size, int autoset_temp_regions, int collect_stats, int verbose_lvl, int block_while_has_temp, int allow_zerocount_push, FILE* log_fp, const char* log_filepath)
{
    KLS_Err_Handlers err_handlers = KLS_DEFAULT_ERR_HANDLERS;
    return kls_conf_init_handled(autoset_regions, alloc_backend, reglist_kls_size, autoset_temp_regions, collect_stats, verbose_lvl, block_while_has_temp, allow_zerocount_push, log_fp, log_filepath, err_handlers);
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
#ifdef KOLISEO_HAS_GULP
    bool kls_gulp = true;
#else
    bool kls_gulp = false;
#endif
#ifdef KOLISEO_HAS_REGION
    bool kls_region = true;
#else
    bool kls_region = false;
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
    bool features[5] = {
        [0] = kls_debug,
        [1] = kls_locate,
        [2] = kls_region,
        [3] = kls_gulp,
        [4] = kls_exper,
    };
    int total_enabled = 0;
    for (int i=0; i<5; i++) {
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
        if (kls_region) {
            fprintf(stderr, "region%s", (total_enabled > 1 ? ", " : ""));
            total_enabled -= 1;
        }
        if (kls_gulp) {
            fprintf(stderr, "gulp%s", (total_enabled > 1 ? ", " : ""));
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

#ifdef KOLISEO_HAS_REGION
/**
 * Calcs the max number of possible KLS_PUSH ops when using KLS_BASIC reglist alloc backend.
 * @return The max number of push ops possible, or -1 in case of error.
 */
int kls_get_maxRegions_KLS_BASIC(Koliseo *kls)
{
    if (kls == NULL) {
#ifdef KLS_DEBUG_CORE
        fprintf(stderr, "[ERROR]    %s(): passed Koliseo was NULL.\n",
                __func__);
#endif
        return -1;
    }
    if (kls->conf.kls_reglist_alloc_backend != KLS_REGLIST_ALLOC_KLS_BASIC) {
#ifdef KLS_DEBUG_CORE
        fprintf(stderr,
                "[ERROR]    %s(): conf.kls_reglist_backend was {%i}, expected KLS_REGLIST_ALLOC_KLS_BASIC: {%i}.\n",
                __func__, kls->conf.kls_reglist_alloc_backend,
                KLS_REGLIST_ALLOC_KLS_BASIC);
#endif
        return -1;
    }
    if (kls->reglist_kls == NULL) {
#ifdef KLS_DEBUG_CORE
        fprintf(stderr,
                "[ERROR]    %s(): passed Koliseo->reglist_kls was NULL.\n",
                __func__);
#endif
        return -1;
    }
    return (kls->reglist_kls->size - sizeof(Koliseo)) / (sizeof(KLS_Region) +
            sizeof
            (KLS_region_list_item));
}

/**
 * Calcs the max number of possible KLS_PUSH_T ops when using KLS_BASIC reglist alloc backend.
 * @return The max number of temp push ops possible, or -1 in case of error.
 */
int kls_temp_get_maxRegions_KLS_BASIC(Koliseo_Temp *t_kls)
{
    if (t_kls == NULL) {
#ifdef KLS_DEBUG_CORE
        fprintf(stderr, "[ERROR]    %s(): passed Koliseo_Temp was NULL.\n",
                __func__);
#endif
        return -1;
    }
    if (t_kls->conf.tkls_reglist_alloc_backend != KLS_REGLIST_ALLOC_KLS_BASIC) {
#ifdef KLS_DEBUG_CORE
        fprintf(stderr,
                "[ERROR]    %s(): conf.tkls_reglist_backend was {%i}, expected KLS_REGLIST_ALLOC_KLS_BASIC: {%i}.\n",
                __func__, t_kls->conf.tkls_reglist_alloc_backend,
                KLS_REGLIST_ALLOC_KLS_BASIC);
#endif
        return -1;
    }
    if (t_kls->reglist_kls == NULL) {
#ifdef KLS_DEBUG_CORE
        fprintf(stderr,
                "[ERROR]    %s(): passed Koliseo_Temp->reglist_kls was NULL.\n",
                __func__);
#endif
        return -1;
    }
    return (t_kls->reglist_kls->size - sizeof(Koliseo)) / (sizeof(KLS_Region) +
            sizeof
            (KLS_region_list_item));
}
#endif // KOLISEO_HAS_REGION

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
Koliseo *kls_new_alloc(ptrdiff_t size, kls_alloc_func alloc_func)
#else
Koliseo *kls_new_alloc_dbg(ptrdiff_t size, kls_alloc_func alloc_func, Koliseo_Loc loc)
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
#ifdef KOLISEO_HAS_REGION
        kls->reglist_kls = NULL;
        kls->regs = NULL;
        kls->max_regions_kls_alloc_basic = 0;
#endif
        kls_set_conf(kls, KLS_DEFAULT_CONF);
        kls->stats = KLS_STATS_DEFAULT;
        kls->conf.kls_log_fp = stderr;
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "KLS", "API Level { %i } ->  Allocated (%s) for new KLS.",
                int_koliseo_version(), h_size);
        kls_log(kls, "KLS", "KLS offset: { %p }.", kls);
        kls_log(kls, "KLS", "Allocation begin offset: { %p }.",
                kls + kls->offset);
#endif

#ifdef KOLISEO_HAS_REGION
        if (kls->conf.kls_autoset_regions == 1) {
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "KLS", "Init of KLS_Region_List for kls.");
#endif

            KLS_Region *kls_header = (KLS_Region *) malloc(sizeof(KLS_Region));
            kls_header->begin_offset = 0;
            kls_header->end_offset = kls->offset;
            kls_header->size =
                kls_header->end_offset - kls_header->begin_offset;
            kls_header->padding = 0;
            kls_header->type = KLS_Header;
            strncpy(kls_header->name, "KLS_Header", KLS_REGION_MAX_NAME_SIZE);
            kls_header->name[KLS_REGION_MAX_NAME_SIZE] = '\0';
            strncpy(kls_header->desc, "Sizeof Koliseo header",
                    KLS_REGION_MAX_DESC_SIZE);
            kls_header->desc[KLS_REGION_MAX_DESC_SIZE] = '\0';
            //KLS_Region_List reglist = kls_emptyList();
            //reglist = kls_cons(kls,kls_header,reglist);
            //kls->regs = reglist;
            kls->regs = kls_rl_cons(kls, kls_header, kls->regs);
            if (kls->regs == NULL) {
                fprintf(stderr,
                        "[KLS] [%s()]: failed to get a KLS_Region_List.\n",
                        __func__);
                exit(EXIT_FAILURE);
            }
        } else {
            kls->regs = NULL;
        }
#endif // KOLISEO_HAS_REGION
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
 * Takes a ptrdiff_t size, a KLS_Conf to configure the new Koliseo, and an allocation function pointer.
 * Calls kls_new_alloc() to initialise the Koliseo, then calls kls_set_conf() to update the config.
 * Returns the new Koliseo.
 * @param size The size for Koliseo data field.
 * @param conf The KLS_Conf for the new Koliseo.
 * @param alloc_func The allocation function to use.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see KLS_DEFAULT_CONF
 * @see kls_new_alloc()
 * @see kls_set_conf()
 */
Koliseo *kls_new_conf_alloc(ptrdiff_t size, KLS_Conf conf, kls_alloc_func alloc_func)
{
    Koliseo *k = kls_new_alloc(size, alloc_func);
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
 * Takes a ptrdiff_t size, a filepath for the trace output file, and an allocation function pointer.
 * Returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a traced Koliseo, logging to the passed filepath.
 * @param size The size for Koliseo data field.
 * @param output_path The filepath for log output.
 * @param alloc_func The allocation function to use.
 * @param err_handlers The error handlers struct for errors in push calls.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf_alloc()
 */
Koliseo *kls_new_traced_alloc_handled(ptrdiff_t size, const char *output_path, kls_alloc_func alloc_func, KLS_Err_Handlers err_handlers)
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
    return kls_new_conf_alloc(size, k, alloc_func);
}

/**
 * Takes a ptrdiff_t size, a filepath for the trace output file, and an allocation function pointer.
 * Returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a traced Koliseo, logging to the passed filepath.
 * @param size The size for Koliseo data field.
 * @param output_path The filepath for log output.
 * @param alloc_func The allocation function to use.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf_alloc()
 */
Koliseo *kls_new_traced_alloc(ptrdiff_t size, const char *output_path, kls_alloc_func alloc_func)
{
    KLS_Err_Handlers err_handlers = KLS_DEFAULT_ERR_HANDLERS;
    return kls_new_traced_alloc_handled(size, output_path, alloc_func, err_handlers);
}

/**
 * Takes a ptrdiff_t size and an allocation function pointer, and returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a debug Koliseo (printing to stderr).
 * @param size The size for Koliseo data field.
 * @param alloc_func The allocation function to use.
 * @param err_handlers The error handlers for errors in push calls.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf()
 */
Koliseo *kls_new_dbg_alloc_handled(ptrdiff_t size, kls_alloc_func alloc_func, KLS_Err_Handlers err_handlers)
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
    Koliseo * kls = kls_new_conf_alloc(size, k, alloc_func);
    kls->conf.kls_verbose_lvl = 1;
    return kls;
}

/**
 * Takes a ptrdiff_t size and an allocation function pointer, and returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a debug Koliseo (printing to stderr).
 * @param size The size for Koliseo data field.
 * @param alloc_func The allocation function to use.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf()
 */
Koliseo *kls_new_dbg_alloc(ptrdiff_t size, kls_alloc_func alloc_func)
{
    KLS_Err_Handlers err_handlers = KLS_DEFAULT_ERR_HANDLERS;
    return kls_new_dbg_alloc_handled(size, alloc_func, err_handlers);
}

/**
 * Takes a ptrdiff_t size and a filepath for the trace output file, and the needed parameters for a successful init of the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a traced Koliseo, logging to the passed filepath.
 * @param size The size for Koliseo data field.
 * @param output_path The filepath for log output.
 * @param reglist_kls_size The size to use for the inner reglist Koliseo.
 * @param alloc_func The allocation function to use.
 * @param err_handlers The error handlers for errors in push calls.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf_alloc()
 */
Koliseo *kls_new_traced_AR_KLS_alloc_handled(ptrdiff_t size, const char *output_path,
        ptrdiff_t reglist_kls_size, kls_alloc_func alloc_func, KLS_Err_Handlers err_handlers)
{
#ifndef KLS_DEBUG_CORE
    fprintf(stderr,
            "[WARN]    %s(): KLS_DEBUG_CORE is not defined. No tracing allowed.\n",
            __func__);
#endif
    KLS_Conf k = (KLS_Conf) {
        .kls_collect_stats = 1,
        .kls_verbose_lvl = 1,
        .kls_log_filepath = output_path,
#ifdef KOLISEO_HAS_REGION
        .kls_reglist_alloc_backend = KLS_REGLIST_ALLOC_KLS_BASIC,
        .kls_reglist_kls_size = reglist_kls_size,
        .kls_autoset_regions = 1,
        .kls_autoset_temp_regions = 1,
#endif // KOLISEO_HAS_REGION
        .err_handlers = (KLS_Err_Handlers) {
#ifndef KOLISEO_HAS_LOCATE
            .OOM_handler = (err_handlers.OOM_handler != NULL ? err_handlers.OOM_handler : &KLS_OOM_default_handler__),
            .PTRDIFF_MAX_handler = (err_handlers.PTRDIFF_MAX_handler != NULL ? err_handlers.PTRDIFF_MAX_handler : &KLS_PTRDIFF_MAX_default_handler__),
#else
            .OOM_handler = (err_handlers.OOM_handler != NULL ? err_handlers.OOM_handler : &KLS_OOM_default_handler_dbg__),
            .PTRDIFF_MAX_handler = (err_handlers.PTRDIFF_MAX_handler != NULL ? err_handlers.PTRDIFF_MAX_handler : &KLS_PTRDIFF_MAX_default_handler_dbg__),
#endif // KOLISEO_HAS_LOCATE
        },
    };
    return kls_new_conf_alloc(size, k, alloc_func);
}

/**
 * Takes a ptrdiff_t size and a filepath for the trace output file, and the needed parameters for a successful init of the prepared Koliseo.
 * Calls kls_new_conf_alloc() to initialise the Koliseo with the proper config for a traced Koliseo, logging to the passed filepath.
 * @param size The size for Koliseo data field.
 * @param output_path The filepath for log output.
 * @param reglist_kls_size The size to use for the inner reglist Koliseo.
 * @param alloc_func The allocation function to use.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf_alloc()
 */
Koliseo *kls_new_traced_AR_KLS_alloc(ptrdiff_t size, const char *output_path,
                                     ptrdiff_t reglist_kls_size, kls_alloc_func alloc_func)
{
    KLS_Err_Handlers err_handlers = KLS_DEFAULT_ERR_HANDLERS;
    return kls_new_traced_AR_KLS_alloc_handled(size, output_path,
            reglist_kls_size, alloc_func, err_handlers);
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

#ifdef KOLISEO_HAS_REGION
    switch (kls->conf.kls_reglist_alloc_backend) {
    case KLS_REGLIST_ALLOC_LIBC: {
    }
    break;
    case KLS_REGLIST_ALLOC_KLS_BASIC: {
        if (kls->conf.kls_autoset_regions == 1) {

#ifdef KLS_DEBUG_CORE
#ifdef KLS_SETCONF_DEBUG
            kls_log(kls, "KLS",
                    "[%s()]:  Prepping reglist_kls. Cleaning previous malloc allocation.",
                    __func__);
#endif
#endif

            kls_rl_freeList(kls->regs);

            Koliseo *reglist_kls = NULL;
            reglist_kls = kls_new_conf(kls->conf.kls_reglist_kls_size, KLS_DEFAULT_CONF__);

            if (!reglist_kls) {
                fprintf(stderr,
                        "[ERROR] [%s()]: Failed to allocate reglist_kls for new Koliseo.\n",
                        __func__);
                kls_free(kls);
                exit(EXIT_FAILURE);
            }
            kls->reglist_kls = reglist_kls;
            kls->max_regions_kls_alloc_basic =
                kls_get_maxRegions_KLS_BASIC(kls);

#ifdef KLS_DEBUG_CORE
#ifdef KLS_SETCONF_DEBUG
            kls_log(kls, "KLS",
                    "%s():  Re-Init of KLS_Region_List for kls. Max regions: {%i}.",
                    __func__, kls->max_regions_kls_alloc_basic);
#endif
#endif
            kls->regs = NULL;
            KLS_Region *kls_header =
                (KLS_Region *) KLS_PUSH(kls->reglist_kls, KLS_Region);
            kls_header->begin_offset = 0;
            kls_header->end_offset = kls->offset;
            kls_header->size =
                kls_header->end_offset - kls_header->begin_offset;
            kls_header->padding = 0;
            kls_header->type = KLS_Header;
            strncpy(kls_header->name, "KLS_Header",
                    KLS_REGION_MAX_NAME_SIZE);
            kls_header->name[KLS_REGION_MAX_NAME_SIZE] = '\0';
            strncpy(kls_header->desc, "Sizeof Koliseo header",
                    KLS_REGION_MAX_DESC_SIZE);
            kls_header->desc[KLS_REGION_MAX_DESC_SIZE] = '\0';
            //KLS_Region_List reglist = kls_emptyList();
            //reglist = kls_cons(kls,kls_header,reglist);
            //kls->regs = reglist;
            kls->regs = kls_rl_cons(kls, kls_header, kls->regs);
            if (kls->regs == NULL) {
                fprintf(stderr,
                        "[KLS] %s() failed to get a KLS_Region_List.\n.",
                        __func__);
                kls_free(kls);
                exit(EXIT_FAILURE);
            }
        } else {
#ifdef KLS_DEBUG_CORE
#ifdef KLS_SETCONF_DEBUG
            kls_log(kls, "KLS",
                    "[%s()]:  Skip prepping reglist_kls. Autoset Regions was: {%i}.",
                    __func__, kls->conf.kls_autoset_regions);
#endif
#endif
        }
    }
    break;
    default: {
        fprintf(stderr,
                "[ERROR]    [%s()]: unexpected KLS_Reglist_Alloc_Backend. {%i}.\n",
                __func__, kls->conf.kls_reglist_alloc_backend);
        return false;
    }
    break;
    }
#endif // KOLISEO_HAS_REGION

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

/**
 * Internal usage only. Return value should be checked to catch errors.
 */
#ifndef KOLISEO_HAS_LOCATE
static inline int kls__check_available_failable(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, const char* caller_name)
#else
static inline int kls__check_available_failable_dbg(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, const char* caller_name, Koliseo_Loc loc)
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
    const ptrdiff_t available = kls->size - kls->offset;
    const ptrdiff_t padding = -kls->offset & (align - 1);
    bool ZEROCOUNT_happened = false;
    bool ZEROCOUNT_handled = false;
    if (count == 0) {
        if (kls->conf.kls_allow_zerocount_push != 1) {
            ZEROCOUNT_happened = true;
            if (kls->conf.err_handlers.ZEROCOUNT_handler != NULL) {
#ifndef KOLISEO_HAS_LOCATE
                kls->conf.err_handlers.ZEROCOUNT_handler(kls, available, padding, size);
#else
                kls->conf.err_handlers.ZEROCOUNT_handler(kls, available, padding, size, loc);
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
            kls_log(kls, "DEBUG", "Accepting zero-count push: conf.kls_allow_zerocount_push was 1");
#endif // KLS_DEBUG_CORE
        }
    }

    if (ZEROCOUNT_happened && ZEROCOUNT_handled) {
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "DEBUG", "Requested a zero-count push while kls_allow_zerocount_push is not 1, but the error handler returned instead of exiting.");
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
            if (kls->conf.err_handlers.PTRDIFF_MAX_handler != NULL) {
#ifndef KOLISEO_HAS_LOCATE
                kls->conf.err_handlers.PTRDIFF_MAX_handler(kls, size, count);
#else
                kls->conf.err_handlers.PTRDIFF_MAX_handler(kls, size, count, loc);
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
            OOM_happened = true;
            if (kls->conf.err_handlers.OOM_handler != NULL) {
#ifndef KOLISEO_HAS_LOCATE
                kls->conf.err_handlers.OOM_handler(kls, available, padding, size, count);
#else
                kls->conf.err_handlers.OOM_handler(kls, available, padding, size, count, loc);
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
            if (kls->conf.err_handlers.PTRDIFF_MAX_handler && PTRDIFF_MAX_handled) {
#ifndef KOLISEO_HAS_LOCATE
                fprintf(stderr, "[KLS] %s(): PTRDIFF_MAX fault happened and was handled.\n", caller_name);
#ifdef KLS_DEBUG_CORE
                kls_log(kls, "DEBUG", "%s(): PTRDIFF_MAX fault happened and was handled.", caller_name);
#endif // KLS_DEBUG_CORE
#else
                fprintf(stderr, "[KLS] " KLS_Loc_Fmt "%s(): PTRDIFF_MAX fault happened and was handled.\n", KLS_Loc_Arg(loc), caller_name);
#ifdef KLS_DEBUG_CORE
                kls_log(kls, "DEBUG", KLS_Loc_Fmt "%s(): PTRDIFF_MAX fault happened and was handled.", KLS_Loc_Arg(loc), caller_name);
#endif // KLS_DEBUG_CORE
#endif // KOLISEO_HAS_LOCATE
                return -1;
            }
        } else if (OOM_happened) {
            if (kls->conf.err_handlers.PTRDIFF_MAX_handler && OOM_handled) {
#ifndef KOLISEO_HAS_LOCATE
                fprintf(stderr, "[KLS] %s(): OOM fault happened and was handled.\n", caller_name);
#ifdef KLS_DEBUG_CORE
                kls_log(kls, "DEBUG", "%s(): OOM fault happened and was handled.", caller_name);
#endif // KLS_DEBUG_CORE
#else
                fprintf(stderr, "[KLS] " KLS_Loc_Fmt "%s(): OOM fault happened and was handled.\n", KLS_Loc_Arg(loc), caller_name);
#ifdef KLS_DEBUG_CORE
                kls_log(kls, "DEBUG", KLS_Loc_Fmt "%s(): OOM fault happened and was handled.", KLS_Loc_Arg(loc), caller_name);
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

/**
 * Internal usage only.
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
    ptrdiff_t padding = -kls->offset & (align - 1);
    char *p = kls->data + kls->offset + padding;
    kls->prev_offset = kls->offset;
    kls->offset += padding + size * count;
    char h_size[200];
    kls_formatSize(size * count, h_size, sizeof(h_size));
    //sprintf(msg,"Pushed size (%li) for KLS.",size);
    //kls_log("KLS",msg);
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "Curr offset: { %p }.", kls + kls->offset);
    kls_log(kls, "KLS", "API Level { %i } -> Pushed size (%s) for KLS.",
            int_koliseo_version(), h_size);
    if (kls->conf.kls_verbose_lvl > 0) {
        print_kls_2file(kls->conf.kls_log_fp, kls);
    }
    if (kls->conf.kls_collect_stats == 1) {
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
        if (elapsed_time > kls->stats.worst_pushcall_time) {
            kls->stats.worst_pushcall_time = elapsed_time;
        }
    }
#endif
    if (kls->conf.kls_collect_stats == 1) {
        kls->stats.tot_pushes += 1;
#ifdef KOLISEO_HAS_REGION
        kls->stats.avg_region_size = kls_avg_regionSize(kls);
#endif
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

    ptrdiff_t padding = -kls->offset & (align - 1);
    char *p = kls->data + kls->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    kls->prev_offset = kls->offset;
    kls->offset += padding + size * count;
    char h_size[200];
    kls_formatSize(size * count, h_size, sizeof(h_size));
    //sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
    //kls_log("KLS",msg);
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "Curr offset: { %p }.", kls + kls->offset);
    kls_log(kls, "KLS", "API Level { %i } -> Pushed zeroes, size (%s) for KLS.",
            int_koliseo_version(), h_size);
    if (kls->conf.kls_verbose_lvl > 0) {
        print_kls_2file(kls->conf.kls_log_fp, kls);
    }
    if (kls->conf.kls_collect_stats == 1) {
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
        if (elapsed_time > kls->stats.worst_pushcall_time) {
            kls->stats.worst_pushcall_time = elapsed_time;
        }
    }
#endif
    if (kls->conf.kls_collect_stats == 1) {
        kls->stats.tot_pushes += 1;
#ifdef KOLISEO_HAS_REGION
        kls->stats.avg_region_size = kls_avg_regionSize(kls);
#endif
    }
    return p;
}

#ifdef KOLISEO_HAS_REGION
static inline void kls__autoregion(const char* caller, Koliseo* kls, ptrdiff_t padding, const char* region_name, size_t region_name_len, const char* region_desc, size_t region_desc_len, int region_type)
{
    assert(caller != NULL);
    assert(kls != NULL);
    assert(region_name != NULL);
    assert(region_name_len > 0);
    size_t name_len = (region_name_len <= KLS_REGION_MAX_NAME_SIZE ? region_name_len : KLS_REGION_MAX_NAME_SIZE);
    //assert(region_name_len <= KLS_REGION_MAX_NAME_SIZE);
    assert(region_desc != NULL);
    assert(region_desc_len > 0);
    size_t desc_len = (region_desc_len <= KLS_REGION_MAX_DESC_SIZE ? region_desc_len : KLS_REGION_MAX_DESC_SIZE);
    //assert(region_desc_len <= KLS_REGION_MAX_DESC_SIZE);
    if (kls->conf.kls_autoset_regions == 1) {
        KLS_Region *reg = NULL;
        switch (kls->conf.kls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            reg = (KLS_Region *) malloc(sizeof(KLS_Region));
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            if (kls_rl_length(kls->regs) < kls->max_regions_kls_alloc_basic) {
                reg = KLS_PUSH(kls->reglist_kls, KLS_Region);
            } else {
                fprintf(stderr,
                        "[ERROR]    [%s()]:  Exceeding kls->max_regions_kls_alloc_basic: {%i}.\n",
                        caller, kls->max_regions_kls_alloc_basic);
                if (kls->conf.kls_verbose_lvl > 0) {
                    kls_log(kls, "ERROR",
                            "[%s()]:  Exceeding kls->max_regions_kls_alloc_basic: {%i}.",
                            caller, kls->max_regions_kls_alloc_basic);
                    kls_rl_showList_toFile(kls->regs, kls->conf.kls_log_fp);
                    print_kls_2file(kls->conf.kls_log_fp, kls->reglist_kls);
                    print_kls_2file(kls->conf.kls_log_fp, kls);
                }
                kls_free(kls);
                exit(EXIT_FAILURE);
            }
        }
        break;
        default: {
            fprintf(stderr,
                    "[ERROR] [%s()]:  Unexpected KLS_RegList_Alloc_Backend value: {%i}.\n",
                    caller, kls->conf.kls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "%s():  Invalid KLS_RegList_Alloc_Backend value: {%i}.",
                    caller, kls->conf.kls_reglist_alloc_backend);
#endif
            kls_free(kls);
            exit(EXIT_FAILURE);
        }
        break;
        }
        reg->begin_offset = kls->prev_offset;
        reg->end_offset = kls->offset;
        reg->size = reg->end_offset - reg->begin_offset;
        reg->padding = padding;
        reg->type = region_type;
        strncpy(reg->name, region_name,
                name_len);
        reg->name[name_len] = '\0';
        strncpy(reg->desc, region_desc,
                desc_len);
        reg->desc[desc_len] = '\0';
        //KLS_Region_List reglist = kls_emptyList();
        //reglist = kls_cons(kls,reg,reglist);
        //kls->regs = kls_append(kls,reglist, kls->regs);
        kls->regs = kls_rl_cons(kls, reg, kls->regs);
    }
}

static inline void kls__temp_autoregion(const char* caller, Koliseo_Temp* t_kls, ptrdiff_t padding, const char* region_name, size_t region_name_len, const char* region_desc, size_t region_desc_len, int region_type)
{
    assert(caller != NULL);
    assert(t_kls != NULL);
    Koliseo* kls = t_kls->kls;
    assert(kls != NULL);
    assert(region_name != NULL);
    assert(region_name_len > 0);
    size_t name_len = (region_name_len <= KLS_REGION_MAX_NAME_SIZE ? region_name_len : KLS_REGION_MAX_NAME_SIZE);
    //assert(region_name_len <= KLS_REGION_MAX_NAME_SIZE);
    assert(region_desc != NULL);
    assert(region_desc_len > 0);
    size_t desc_len = (region_desc_len <= KLS_REGION_MAX_DESC_SIZE ? region_desc_len : KLS_REGION_MAX_DESC_SIZE);
    //assert(region_desc_len <= KLS_REGION_MAX_DESC_SIZE);
    KLS_Region *reg = NULL;
    if (t_kls->conf.kls_autoset_regions == 1) {
        switch (t_kls->conf.tkls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            reg = (KLS_Region *) malloc(sizeof(KLS_Region));
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            if (kls_rl_length(t_kls->t_regs) <
                t_kls->max_regions_kls_alloc_basic) {
                reg = KLS_PUSH(t_kls->reglist_kls, KLS_Region);
            } else {
                fprintf(stderr,
                        "[ERROR]    [%s()]:  Exceeding t_kls->max_regions_kls_alloc_basic: {%i}.\n",
                        caller, t_kls->max_regions_kls_alloc_basic);
                if (kls->conf.kls_verbose_lvl > 0) {
                    kls_log(kls, "ERROR",
                            "[%s()]:  Exceeding t_kls->max_regions_kls_alloc_basic: {%i}.",
                            caller, t_kls->max_regions_kls_alloc_basic);
                    kls_rl_showList_toFile(t_kls->t_regs,
                                           kls->conf.kls_log_fp);
                    print_kls_2file(kls->conf.kls_log_fp,
                                    t_kls->reglist_kls);
                    print_kls_2file(kls->conf.kls_log_fp, kls);
                }
                kls_free(kls);
                exit(EXIT_FAILURE);
            }
        }
        break;
        default: {
            fprintf(stderr,
                    "[ERROR]    %s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                    caller, t_kls->conf.tkls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "%s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                    caller, t_kls->conf.tkls_reglist_alloc_backend);
#endif
            kls_free(kls);
            exit(EXIT_FAILURE);
        }
        break;
        }
        reg->begin_offset = kls->prev_offset;
        reg->end_offset = kls->offset;
        reg->size = reg->end_offset - reg->begin_offset;
        reg->padding = padding;
        reg->type = KLS_None;
        strncpy(reg->name, region_name,
                name_len);
        reg->name[name_len] = '\0';
        strncpy(reg->desc, region_desc,
                desc_len);
        reg->desc[desc_len] = '\0';
        //KLS_Region_List reglist = kls_emptyList();
        //reglist = kls_cons(kls,reg,reglist);
        //t_kls->t_regs = kls_append(kls,reglist, t_kls->t_regs);
        t_kls->t_regs = kls_rl_t_cons(t_kls, reg, t_kls->t_regs);
    }
}
#endif // KOLISEO_HAS_REGION

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
void *kls_push_zero_AR(Koliseo *kls, ptrdiff_t size, ptrdiff_t align,
                       ptrdiff_t count)
#else
void *kls_push_zero_AR_dbg(Koliseo *kls, ptrdiff_t size, ptrdiff_t align,
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
    ptrdiff_t padding = -kls->offset & (align - 1);
    char *p = kls->data + kls->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    kls->prev_offset = kls->offset;
    kls->offset += padding + size * count;

#ifdef KOLISEO_HAS_REGION
    kls__autoregion(__func__, kls, padding, KOLISEO_DEFAULT_REGION_NAME, strlen(KOLISEO_DEFAULT_REGION_NAME), KOLISEO_DEFAULT_REGION_DESC, strlen(KOLISEO_DEFAULT_REGION_DESC), KLS_None);
#endif // KOLISEO_HAS_REGION

    char h_size[200];
    kls_formatSize(size * count, h_size, sizeof(h_size));
    //sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
    //kls_log("KLS",msg);
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "Curr offset: { %p }.", kls + kls->offset);
    kls_log(kls, "KLS", "API Level { %i } -> Pushed zeroes, size (%s) for KLS.",
            int_koliseo_version(), h_size);
    if (kls->conf.kls_verbose_lvl > 0) {
        print_kls_2file(kls->conf.kls_log_fp, kls);
    }
    if (kls->conf.kls_collect_stats == 1) {
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
        if (elapsed_time > kls->stats.worst_pushcall_time) {
            kls->stats.worst_pushcall_time = elapsed_time;
        }
    }
#endif
    if (kls->conf.kls_collect_stats == 1) {
        kls->stats.tot_pushes += 1;
#ifdef KOLISEO_HAS_REGION
        kls->stats.avg_region_size = kls_avg_regionSize(kls);
#endif
    }
    return p;
}

/**
 * Takes a Koliseo_Temp, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the referred Koliseo data field, or goes to exit() if the operation fails.
 * Notably, it zeroes the memory region.
 * @param t_kls The Koliseo_Temp at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @return A void pointer to the start of memory just pushed to the referred Koliseo.
 */
#ifndef KOLISEO_HAS_LOCATE
void *kls_temp_push_zero_AR(Koliseo_Temp *t_kls, ptrdiff_t size,
                            ptrdiff_t align, ptrdiff_t count)
#else
void *kls_temp_push_zero_AR_dbg(Koliseo_Temp *t_kls, ptrdiff_t size,
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
    ptrdiff_t padding = -kls->offset & (align - 1);
    char *p = kls->data + kls->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    kls->prev_offset = kls->offset;
    kls->offset += padding + size * count;

#ifdef KOLISEO_HAS_REGION
    kls__temp_autoregion(__func__, t_kls, padding, KOLISEO_DEFAULT_REGION_NAME, strlen(KOLISEO_DEFAULT_REGION_NAME), KOLISEO_DEFAULT_REGION_DESC, strlen(KOLISEO_DEFAULT_REGION_DESC), KLS_None);
#endif // KOLISEO_HAS_REGION

    char h_size[200];
    kls_formatSize(size * count, h_size, sizeof(h_size));
    //sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
    //kls_log("KLS",msg);
#ifdef KLS_DEBUG_CORE
    if (kls->conf.kls_collect_stats == 1) {
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
        if (elapsed_time > kls->stats.worst_pushcall_time) {
            kls->stats.worst_pushcall_time = elapsed_time;
        }
#ifdef KOLISEO_HAS_REGION
        kls->stats.avg_region_size = kls_avg_regionSize(kls);
#endif
    }
    kls_log(kls, "KLS", "Curr offset: { %p }.", kls + kls->offset);
    kls_log(kls, "KLS",
            "API Level { %i } -> Pushed zeroes, size (%s) for Temp_KLS.",
            int_koliseo_version(), h_size);
    if (kls->conf.kls_verbose_lvl > 0) {
        print_kls_2file(kls->conf.kls_log_fp, kls);
    }
#endif
    if (kls->conf.kls_collect_stats == 1) {
        kls->stats.tot_temp_pushes += 1;
    }
    return p;
}

#ifdef KOLISEO_HAS_REGION
/**
 * Takes a Koliseo pointer, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the Koliseo data field, or goes to exit() if the operation fails.
 * Uses the passed name and desc fields to initialise the allocated KLS_Region fields.
 * Notably, it zeroes the memory region.
 * @param kls The Koliseo at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @param name The name to assign to the resulting KLS_Region.
 * @param desc The desc to assign to the resulting KLS_Region.
 * @return A void pointer to the start of memory just pushed to the Koliseo.
 */
#ifndef KOLISEO_HAS_LOCATE
void *kls_push_zero_named(Koliseo *kls, ptrdiff_t size, ptrdiff_t align,
                          ptrdiff_t count, char *name, char *desc)
#else
void *kls_push_zero_named_dbg(Koliseo *kls, ptrdiff_t size, ptrdiff_t align,
                              ptrdiff_t count, char *name, char *desc, Koliseo_Loc loc)
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
    ptrdiff_t padding = -kls->offset & (align - 1);
    char *p = kls->data + kls->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    kls->prev_offset = kls->offset;
    kls->offset += padding + size * count;

    kls__autoregion(__func__, kls, padding, name, strlen(name)+1, desc, strlen(desc)+1, KLS_None);

    char h_size[200];
    kls_formatSize(size * count, h_size, sizeof(h_size));
    //sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
    //kls_log("KLS",msg);
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "Curr offset: { %p }.", kls + kls->offset);
    kls_log(kls, "KLS",
            "API Level { %i } -> Pushed zeroes, size (%s) for KLS.",
            int_koliseo_version(), h_size);
    if (kls->conf.kls_verbose_lvl > 0) {
        print_kls_2file(kls->conf.kls_log_fp, kls);
    }
#endif
#ifdef KLS_DEBUG_CORE
    if (kls->conf.kls_collect_stats == 1) {
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
        if (elapsed_time > kls->stats.worst_pushcall_time) {
            kls->stats.worst_pushcall_time = elapsed_time;
        }
    }
#endif
    if (kls->conf.kls_collect_stats == 1) {
        kls->stats.tot_pushes += 1;
        kls->stats.avg_region_size = kls_avg_regionSize(kls);
    }
    return p;
}

/**
 * Takes a Koliseo_Temp, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the referred Koliseo data field, or goes to exit() if the operation fails.
 * Uses the passed name and desc fields to initialise the allocated KLS_Region fields.
 * Notably, it zeroes the memory region.
 * @param t_kls The Koliseo_Temp at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @param name The name to assign to the resulting KLS_Region.
 * @param desc The desc to assign to the resulting KLS_Region.
 * @return A void pointer to the start of memory just pushed to the Koliseo.
 */
#ifndef KOLISEO_HAS_LOCATE
void *kls_temp_push_zero_named(Koliseo_Temp *t_kls, ptrdiff_t size,
                               ptrdiff_t align, ptrdiff_t count, char *name,
                               char *desc)
#else
void *kls_temp_push_zero_named_dbg(Koliseo_Temp *t_kls, ptrdiff_t size,
                                   ptrdiff_t align, ptrdiff_t count, char *name,
                                   char *desc, Koliseo_Loc loc)
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
    ptrdiff_t padding = -kls->offset & (align - 1);
    char *p = kls->data + kls->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    kls->prev_offset = kls->offset;
    kls->offset += padding + size * count;

    kls__temp_autoregion(__func__, t_kls, padding, name, strlen(name)+1, desc, strlen(desc)+1, KLS_None);

    char h_size[200];
    kls_formatSize(size, h_size, sizeof(h_size));
    //sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
    //kls_log("KLS",msg);
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "Curr offset: { %p }.", kls + kls->offset);
    kls_log(kls, "KLS",
            "API Level { %i } -> Pushed zeroes, size (%s) for Temp_KLS.",
            int_koliseo_version(), h_size);
    if (kls->conf.kls_verbose_lvl > 0) {
        print_kls_2file(kls->conf.kls_log_fp, kls);
    }
#endif
#ifdef KLS_DEBUG_CORE
    if (kls->conf.kls_collect_stats == 1) {
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
        if (elapsed_time > kls->stats.worst_pushcall_time) {
            kls->stats.worst_pushcall_time = elapsed_time;
        }
    }
#endif
    if (kls->conf.kls_collect_stats == 1) {
        kls->stats.tot_temp_pushes += 1;
    }
    return p;
}

/**
 * Takes a Koliseo pointer, a KLS_Region_Type index, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the Koliseo data field, or goes to exit() if the operation fails.
 * Uses the passed name and desc fields to initialise the allocated KLS_Region fields.
 * Notably, it zeroes the memory region.
 * @param kls The Koliseo at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @param type The type index for pushed KLS_Region.
 * @param name The name to assign to the resulting KLS_Region.
 * @param desc The desc to assign to the resulting KLS_Region.
 * @return A void pointer to the start of memory just pushed to the Koliseo.
 */
#ifndef KOLISEO_HAS_LOCATE
void *kls_push_zero_typed(Koliseo *kls, ptrdiff_t size, ptrdiff_t align,
                          ptrdiff_t count, int type, char *name, char *desc)
#else
void *kls_push_zero_typed_dbg(Koliseo *kls, ptrdiff_t size, ptrdiff_t align,
                              ptrdiff_t count, int type, char *name, char *desc, Koliseo_Loc loc)
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
    ptrdiff_t padding = -kls->offset & (align - 1);
    char *p = kls->data + kls->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    kls->prev_offset = kls->offset;
    kls->offset += padding + size * count;

    kls__autoregion(__func__, kls, padding, name, strlen(name), desc, strlen(desc), type);

    char h_size[200];
    kls_formatSize(size * count, h_size, sizeof(h_size));
    //sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
    //kls_log("KLS",msg);
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "Curr offset: { %p }.", kls + kls->offset);
    kls_log(kls, "KLS",
            "API Level { %i } -> Pushed zeroes, size (%s) for KLS.",
            int_koliseo_version(), h_size);
    if (kls->conf.kls_verbose_lvl > 0) {
        print_kls_2file(kls->conf.kls_log_fp, kls);
    }
#endif
#ifdef KLS_DEBUG_CORE
    if (kls->conf.kls_collect_stats == 1) {
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
        if (elapsed_time > kls->stats.worst_pushcall_time) {
            kls->stats.worst_pushcall_time = elapsed_time;
        }
    }
#endif
    if (kls->conf.kls_collect_stats == 1) {
        kls->stats.tot_pushes += 1;
        kls->stats.avg_region_size = kls_avg_regionSize(kls);
    }
    return p;
}

/**
 * Takes a Koliseo_Temp, a KLS_Region_Type index, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the referred Koliseo data field, or goes to exit() if the operation fails.
 * Uses the passed name and desc fields to initialise the allocated KLS_Region fields.
 * Notably, it zeroes the memory region.
 * @param t_kls The Koliseo_Temp at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @param type The type index for pushed KLS_Region.
 * @param name The name to assign to the resulting KLS_Region.
 * @param desc The desc to assign to the resulting KLS_Region.
 * @return A void pointer to the start of memory just pushed to the referred Koliseo.
 */
#ifndef KOLISEO_HAS_LOCATE
void *kls_temp_push_zero_typed(Koliseo_Temp *t_kls, ptrdiff_t size,
                               ptrdiff_t align, ptrdiff_t count, int type,
                               char *name, char *desc)
#else
void *kls_temp_push_zero_typed_dbg(Koliseo_Temp *t_kls, ptrdiff_t size,
                                   ptrdiff_t align, ptrdiff_t count, int type,
                                   char *name, char *desc, Koliseo_Loc loc)
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
    ptrdiff_t padding = -kls->offset & (align - 1);
    char *p = kls->data + kls->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    kls->prev_offset = kls->offset;
    kls->offset += padding + size * count;

    kls__temp_autoregion(__func__, t_kls, padding, name, strlen(name)+1, desc, strlen(desc)+1, type);

    char h_size[200];
    kls_formatSize(size * count, h_size, sizeof(h_size));
    //sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
    //kls_log("KLS",msg);
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "Curr offset: { %p }.", kls + kls->offset);
    kls_log(kls, "KLS",
            "API Level { %i } -> Pushed zeroes, size (%s) for Temp_KLS.",
            int_koliseo_version(), h_size);
    if (kls->conf.kls_verbose_lvl > 0) {
        print_kls_2file(kls->conf.kls_log_fp, kls);
    }
#endif
#ifdef KLS_DEBUG_CORE
    if (kls->conf.kls_collect_stats == 1) {
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
        if (elapsed_time > kls->stats.worst_pushcall_time) {
            kls->stats.worst_pushcall_time = elapsed_time;
        }
    }
#endif
    if (kls->conf.kls_collect_stats == 1) {
        kls->stats.tot_temp_pushes += 1;
        kls->stats.avg_region_size = kls_avg_regionSize(kls);
    }
    return p;
}

#endif // KOLISEO_HAS_REGION

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
#ifdef KOLISEO_HAS_REGION
        if (kls->conf.kls_reglist_alloc_backend == KLS_REGLIST_ALLOC_KLS_BASIC) {
            fprintf(fp, "[KLS] Max Regions: { %i }\n\n",
                    kls->max_regions_kls_alloc_basic);
        } else {
            fprintf(fp, "\n");
        }
#else
        fprintf(fp, "\n");
#endif // KOLISEO_HAS_REGION
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
    if (kls->has_temp == 1) {
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "KLS",
                "API Level { %i } -> KLS had an active Koliseo_Temp.",
                int_koliseo_version());
#endif
        kls_temp_end(kls->t_kls);
    }
    kls_clear(kls);
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "API Level { %i } -> Freeing KLS.",
            int_koliseo_version());
#endif
    if (kls->conf.kls_log_fp != NULL && kls->conf.kls_log_fp != stdout
        && kls->conf.kls_log_fp != stderr) {
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "KLS", "Closing kls log file. Path: {\"%s\"}.",
                kls->conf.kls_log_filepath);
#endif
        int close_res = fclose(kls->conf.kls_log_fp);
        if (close_res != 0) {
            fprintf(stderr,
                    "[ERROR]    %s(): Failed fclose() on log_fp. Path: {\"%s\"}.",
                    __func__, kls->conf.kls_log_filepath);
        }
    } else if (kls->conf.kls_log_fp == stdout || kls->conf.kls_log_fp == stderr) {
        if (kls->conf.kls_verbose_lvl > 1) {
            fprintf(stderr,
                    "[INFO]    %s(): kls->conf.kls_log_fp is %s. Not closing it.\n",
                    __func__,
                    (kls->conf.kls_log_fp == stdout ? "stdout" : "stderr"));
        }
    }
#ifdef KOLISEO_HAS_REGION
    if (kls->conf.kls_reglist_alloc_backend == KLS_REGLIST_ALLOC_KLS_BASIC) {
        kls_free(kls->reglist_kls);
        //free(kls->reglist_kls);
    } else {
        kls_rl_freeList(kls->regs);
    }
#endif
    free(kls);
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
    if (kls->has_temp != 0) {
        fprintf(stderr,
                "[ERROR] [%s()]: Passed Koliseo->has_temp is not 0. {%i}\n",
                __func__, kls->has_temp);
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "ERROR", "[%s()]: Passed Koliseo->has_temp != 0 . {%i}",
                __func__, kls->has_temp);
#endif
        if (kls->conf.kls_collect_stats == 1) {
            kls->stats.tot_hiccups += 1;
        }
        return NULL;
    }
    ptrdiff_t prev = kls->prev_offset;
    ptrdiff_t off = kls->offset;

    Koliseo_Temp *tmp = KLS_PUSH(kls, Koliseo_Temp);
    tmp->kls = kls;
    tmp->prev_offset = prev;
    tmp->offset = off;
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "INFO", "Passed kls conf: " KLS_Conf_Fmt "\n",
            KLS_Conf_Arg(kls->conf));
#endif
#ifdef KOLISEO_HAS_REGION
    switch (kls->conf.kls_reglist_alloc_backend) {
    case KLS_REGLIST_ALLOC_LIBC: {
        tmp->conf = (KLS_Temp_Conf) {
            .kls_autoset_regions =
                kls->conf.kls_autoset_temp_regions,.
                tkls_reglist_alloc_backend = KLS_REGLIST_ALLOC_LIBC,
        };
    }
    break;
    case KLS_REGLIST_ALLOC_KLS_BASIC: {
        tmp->conf = (KLS_Temp_Conf) {
            .kls_autoset_regions =
                kls->conf.kls_autoset_temp_regions,.
                tkls_reglist_alloc_backend =
                    KLS_REGLIST_ALLOC_KLS_BASIC,.kls_reglist_kls_size =
                        kls->conf.kls_reglist_kls_size,
        };
        tmp->reglist_kls = kls_new(tmp->conf.kls_reglist_kls_size);
        tmp->max_regions_kls_alloc_basic =
            kls_temp_get_maxRegions_KLS_BASIC(tmp);
    }
    break;
    default: {
        fprintf(stderr,
                "[ERROR] [%s()]: Invalid kls->conf.kls_reglist_alloc_backend value: {%i}.\n",
                __func__, kls->conf.kls_reglist_alloc_backend);
        kls_free(kls);
        exit(EXIT_FAILURE);
    }
    break;
    }
#endif // KOLISEO_HAS_REGION
    kls->has_temp = 1;
    kls->t_kls = tmp;
#ifdef KOLISEO_HAS_REGION
    if (kls->conf.kls_autoset_temp_regions == 1) {
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "KLS", "Init of KLS_Region_List for temp kls.");
#endif
        KLS_Region *temp_kls_header = NULL;
        switch (tmp->conf.tkls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            temp_kls_header = (KLS_Region *) malloc(sizeof(KLS_Region));
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            temp_kls_header = KLS_PUSH(tmp->reglist_kls, KLS_Region);
        }
        break;
        default: {
            fprintf(stderr,
                    "[ERROR]    %s():  Invalid conf.kls_reglist_alloc_backend value: {%i}.\n",
                    __func__, kls->conf.kls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "%s():  Invalid conf.kls_reglist_alloc_backend value: {%i}.\n",
                    __func__, kls->conf.kls_reglist_alloc_backend);
#endif
            kls_free(kls);
            exit(EXIT_FAILURE);
        }
        break;
        }
        temp_kls_header->begin_offset = tmp->prev_offset;
        temp_kls_header->end_offset = tmp->offset;
        temp_kls_header->size =
            temp_kls_header->end_offset - temp_kls_header->begin_offset;
        //TODO Padding??
        temp_kls_header->type = Temp_KLS_Header;
        strncpy(temp_kls_header->name, "T_KLS_Header",
                KLS_REGION_MAX_NAME_SIZE);
        temp_kls_header->name[KLS_REGION_MAX_NAME_SIZE] = '\0';
        strncpy(temp_kls_header->desc, "Last Reg b4 KLS_T",
                KLS_REGION_MAX_DESC_SIZE);
        temp_kls_header->desc[KLS_REGION_MAX_DESC_SIZE] = '\0';
        KLS_Region_List reglist = kls_rl_emptyList();
        reglist = kls_rl_t_cons(tmp, temp_kls_header, reglist);
        tmp->t_regs = reglist;
        if (tmp->t_regs == NULL) {
            fprintf(stderr, "[KLS] [%s()]: failed to get a KLS_Region_List.\n",
                    __func__);
            exit(EXIT_FAILURE);
        }
    } else {
        tmp->t_regs = NULL;
    }
#endif // KOLISEO_HAS_REGION
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "Prepared new Temp KLS.");
#endif
    return tmp;
}

#ifdef KOLISEO_HAS_REGION
/**
 * Updates the KLS_Temp_Conf for the passed Koliseo_Temp pointer.
 * @param t_kls The Koliseo_Temp pointer to update.
 * @param conf The KLS_Temp_Conf to set.
 * @return A bool representing success.
 */
bool kls_temp_set_conf(Koliseo_Temp *t_kls, KLS_Temp_Conf conf)
{
    if (t_kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo_Temp was NULL.\n",
                __func__);
        //TODO: is it better to exit() here?
        return false;
    }

    t_kls->conf = conf;
    return true;
}
#endif // KOLISEO_HAS_REGION

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

#ifdef KOLISEO_HAS_REGION
    if (tmp_kls->conf.kls_autoset_regions == 1) {
        switch (tmp_kls->conf.tkls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            kls_rl_freeList(tmp_kls->t_regs);
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            kls_free(tmp_kls->reglist_kls);
        }
        break;
        default: {
            fprintf(stderr,
                    "[ERROR]    %s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                    __func__, tmp_kls->conf.tkls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
            kls_log(tmp_kls->kls, "ERROR",
                    "%s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                    __func__, tmp_kls->conf.tkls_reglist_alloc_backend);
#endif
            kls_free(tmp_kls->kls);
            exit(EXIT_FAILURE);
        }
        break;
        }
    }
#endif // KOLISEO_HAS_REGION
    Koliseo *kls_ref = tmp_kls->kls;
#ifdef KLS_DEBUG_CORE
    if (kls_ref == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Referred Koliseo was NULL.\n",
                __func__);
        exit(EXIT_FAILURE);
    }
    kls_log(kls_ref, "KLS", "Ended Temp KLS.");
#endif
    tmp_kls->kls->has_temp = 0;
    tmp_kls->kls->t_kls = NULL;
    tmp_kls->kls->prev_offset = tmp_kls->prev_offset;
    tmp_kls->kls->offset = tmp_kls->offset;
    tmp_kls = NULL; // statement with no effect TODO: Clear tmp_kls from caller
    if (kls_ref->conf.kls_collect_stats == 1) {
        kls_ref->stats.tot_temp_pushes = 0;
        kls_ref->stats.tot_temp_pops = 0;
    }
}

#ifdef KOLISEO_HAS_REGION

KLS_Region_List kls_rl_emptyList(void)
{
    return NULL;
}

bool kls_rl_empty(KLS_Region_List l)
{
    if (l == NULL) {
        return true;
    } else {
        return false;
    }
}

KLS_list_element kls_rl_head(KLS_Region_List l)
{
    if (kls_rl_empty(l)) {
        exit(EXIT_FAILURE);
    } else {
        return l->value;
    }
}

KLS_Region_List kls_rl_tail(KLS_Region_List l)
{
    if (kls_rl_empty(l)) {
        exit(EXIT_FAILURE);
    } else {
        return l->next;
    }
}

KLS_Region_List kls_rl_cons(Koliseo *kls, KLS_list_element e, KLS_Region_List l)
{
    if (e == NULL) {
#ifdef KLS_DEBUG_CORE
        fprintf(stderr, "[KLS]    %s():  KLS_list_element was NULL.\n",
                __func__);
#endif
    }
    if (kls == NULL) {
        fprintf(stderr, "[KLS]    %s():  Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    KLS_Region_List t;
    switch (kls->conf.kls_reglist_alloc_backend) {
    case KLS_REGLIST_ALLOC_LIBC: {
        t = (KLS_Region_List) malloc(sizeof(KLS_region_list_item));
    }
    break;
    case KLS_REGLIST_ALLOC_KLS_BASIC: {
        if (kls->reglist_kls == NULL) {
            fprintf(stderr,
                    "[ERROR]   at %s(): Koliseo->reglist_kls was NULL.\n",
                    __func__);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "at %s(): Koliseo->reglist_kls was NULL.\n", __func__);
#endif
            kls_free(kls);
            exit(EXIT_FAILURE);
        }
        t = KLS_PUSH(kls->reglist_kls, KLS_region_list_item);
    }
    break;
    default: {
        fprintf(stderr,
                "[ERROR]    %s():  Invalid conf.kls_reglist_alloc_backend value: {%i}.\n",
                __func__, kls->conf.kls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "ERROR",
                "%s():  Invalid conf.kls_reglist_alloc_backend value: {%i}.\n",
                __func__, kls->conf.kls_reglist_alloc_backend);
#endif
        kls_free(kls);
        exit(EXIT_FAILURE);
    }
    break;
    }
    t->value = e;
    t->next = l;
    return t;
}

#ifdef KOLISEO_HAS_EXPER
KLS_region_list_item* kls_list_pop(Koliseo *kls)
{
    if (kls == NULL) {
        fprintf(stderr, "[KLS]    %s():  Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    KLS_Region_List l;
    switch (kls->conf.kls_reglist_alloc_backend) {
    case KLS_REGLIST_ALLOC_LIBC: {
        l = kls->regs;
    }
    break;
    case KLS_REGLIST_ALLOC_KLS_BASIC: {
        if (kls->reglist_kls == NULL) {
            fprintf(stderr,
                    "[ERROR]   at %s(): Koliseo->reglist_kls was NULL.\n",
                    __func__);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "at %s(): Koliseo->reglist_kls was NULL.\n", __func__);
#endif
            kls_free(kls);
            exit(EXIT_FAILURE);
        }
        l = kls->regs;
    }
    break;
    default: {
        fprintf(stderr,
                "[ERROR]    %s():  Invalid conf.kls_reglist_alloc_backend value: {%i}.\n",
                __func__, kls->conf.kls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "ERROR",
                "%s():  Invalid conf.kls_reglist_alloc_backend value: {%i}.\n",
                __func__, kls->conf.kls_reglist_alloc_backend);
#endif
        kls_free(kls);
        exit(EXIT_FAILURE);
    }
    break;
    }
    //TODO: is this enough to correctly pop the list?
    l = kls_tail(l);
    KLS_region_list_item* popped_node = KLS_POP(kls->reglist_kls, KLS_region_list_item);
    return popped_node;
}
#endif // KOLISEO_HAS_EXPER

KLS_Region_List kls_rl_t_cons(Koliseo_Temp *t_kls, KLS_list_element e,
                              KLS_Region_List l)
{
    if (e == NULL) {
#ifdef KLS_DEBUG_CORE
        fprintf(stderr, "[KLS]    %s():  KLS_list_element was NULL.\n",
                __func__);
#endif
    }
    if (t_kls == NULL) {
        fprintf(stderr, "[KLS]    %s():  Koliseo_Temp was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    KLS_Region_List t;
    switch (t_kls->conf.tkls_reglist_alloc_backend) {
    case KLS_REGLIST_ALLOC_LIBC: {
        t = (KLS_Region_List) malloc(sizeof(KLS_region_list_item));
    }
    break;
    case KLS_REGLIST_ALLOC_KLS_BASIC: {
        t = KLS_PUSH(t_kls->reglist_kls, KLS_region_list_item);
    }
    break;
    default: {
        fprintf(stderr,
                "[ERROR]    %s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                __func__, t_kls->conf.tkls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
        kls_log(t_kls->kls, "ERROR",
                "%s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                __func__, t_kls->conf.tkls_reglist_alloc_backend);
#endif
        kls_free(t_kls->kls);
        exit(EXIT_FAILURE);
    }
    break;
    }
    t->value = e;
    t->next = l;
    return t;
}

#ifdef KOLISEO_HAS_EXPER
KLS_region_list_item* kls_t_list_pop(Koliseo_Temp *t_kls)
{
    if (t_kls == NULL) {
        fprintf(stderr, "[KLS]    %s():  Koliseo_Temp was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    KLS_Region_List l;
    switch (t_kls->conf.tkls_reglist_alloc_backend) {
    case KLS_REGLIST_ALLOC_LIBC: {
        l = t_kls->t_regs;
    }
    break;
    case KLS_REGLIST_ALLOC_KLS_BASIC: {
        if (t_kls->reglist_kls == NULL) {
            fprintf(stderr,
                    "[ERROR]   at %s(): Koliseo_Temp->reglist_kls was NULL.\n",
                    __func__);
#ifdef KLS_DEBUG_CORE
            kls_log(t_kls->kls, "ERROR",
                    "at %s(): Koliseo_Temp->reglist_kls was NULL.\n", __func__);
#endif
            kls_free(t_kls->kls);
            exit(EXIT_FAILURE);
        }
        l = t_kls->t_regs;
    }
    break;
    default: {
        fprintf(stderr,
                "[ERROR]    %s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                __func__, t_kls->conf.tkls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
        kls_log(t_kls->kls, "ERROR",
                "%s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                __func__, t_kls->conf.tkls_reglist_alloc_backend);
#endif
        kls_free(t_kls->kls);
        exit(EXIT_FAILURE);
    }
    break;
    }
    //TODO: is this enough to correctly pop the list?
    l = kls_tail(l);
    KLS_region_list_item* popped_node = KLS_POP(t_kls->reglist_kls, KLS_region_list_item);
    return popped_node;
}
#endif // KOLISEO_HAS_EXPER

/**
 * Frees all values and nodes for passed Region list.
 * Should only be used internally for operations with ALLOC_LIBC for allocation backend.
 * @param l The list to free (allocated by using malloc).
 */
void kls_rl_freeList(KLS_Region_List l)
{
    if (kls_rl_empty(l)) {
        return;
    } else {
        kls_rl_freeList(kls_rl_tail(l));
#ifdef KLS_DEBUG_CORE
        fprintf(stderr, "[KLS]    %s(): Freeing KLS_Region_List->value.\n",
                __func__);
#endif
        free(l->value);
#ifdef KLS_DEBUG_CORE
        fprintf(stderr, "[KLS]    %s(): Freeing KLS_Region_List.\n", __func__);
#endif
        free(l);
    }
    return;
}

void kls_rl_showList_toFile(KLS_Region_List l, FILE *fp)
{
    if (fp == NULL) {
        fprintf(stderr,
                "[KLS]  kls_showList_toFile():  passed file was NULL.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "{");
    while (!kls_rl_empty(l)) {
        fprintf(fp, "\n{ %s }, { %s }    ", kls_rl_head(l)->name,
                kls_rl_head(l)->desc);
#ifndef _WIN32
        fprintf(fp, "{ %li } -> { %li }", kls_rl_head(l)->begin_offset,
                kls_rl_head(l)->end_offset);
#else
        fprintf(fp, "{ %lli } -> { %lli }", kls_rl_head(l)->begin_offset,
                kls_rl_head(l)->end_offset);
#endif
        /*
           #ifdef KLS_DEBUG_CORE
           kls_log("KLS-Region","    KLS_Region {");
           kls_log("KLS-Region","{ %s }, { %s }",kls_head(l)->name,kls_head(l)->desc);
           char h_size[200];
           ptrdiff_t r_size = kls_head(l)->end_offset - kls_head(l)->begin_offset;
           kls_formatSize(r_size,h_size,sizeof(h_size));
           kls_log("KLS-Region","{ %s }",h_size);
           kls_log("KLS-Region","{ %li } -> { %li }",kls_head(l)->begin_offset,kls_head(l)->end_offset);
           kls_log("KLS-Region","    }");
           #endif
         */

        l = kls_rl_tail(l);
        if (!kls_rl_empty(l)) {
            fprintf(fp, ",\n");
        }
    }
    fprintf(fp, "\n}\n");
}

void kls_rl_showList(KLS_Region_List l)
{
    kls_rl_showList_toFile(l, stdout);
}

bool kls_rl_member(KLS_list_element el, KLS_Region_List l)
{
    if (kls_rl_empty(l)) {
        return false;
    } else {
        if (el == kls_rl_head(l)) {
            return true;
        } else {
            return kls_rl_member(el, kls_rl_tail(l));
        }
    }
}

int kls_rl_length(KLS_Region_List l)
{
    if (kls_rl_empty(l)) {
        return 0;
    } else {
        return 1 + kls_rl_length(kls_rl_tail(l));
    }
}

KLS_Region_List kls_rl_append(Koliseo *kls, KLS_Region_List l1, KLS_Region_List l2)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_rl_empty(l1)) {
        return l2;
    } else {
        return kls_rl_cons(kls, kls_rl_head(l1), kls_rl_append(kls, kls_rl_tail(l1), l2));
    }
}

KLS_Region_List kls_rl_reverse(Koliseo *kls, KLS_Region_List l)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_rl_empty(l)) {
        return kls_rl_emptyList();
    } else {
        return kls_rl_append(kls, kls_rl_reverse(kls, kls_rl_tail(l)),
                             kls_rl_cons(kls, kls_rl_head(l), kls_rl_emptyList()));
    }
}

KLS_Region_List kls_rl_copy(Koliseo *kls, KLS_Region_List l)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_rl_empty(l)) {
        return l;
    } else {
        return kls_rl_cons(kls, kls_rl_head(l), kls_rl_copy(kls, kls_rl_tail(l)));
    }
}

KLS_Region_List kls_rl_delete(Koliseo *kls, KLS_list_element el, KLS_Region_List l)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_rl_empty(l)) {
        return kls_rl_emptyList();
    } else {
        if (el == kls_rl_head(l)) {
            return kls_rl_tail(l);
        } else {
            return kls_rl_cons(kls, kls_rl_head(l), kls_rl_delete(kls, el, kls_rl_tail(l)));
        }
    }
}

KLS_Region_List kls_rl_insord(Koliseo *kls, KLS_list_element el, KLS_Region_List l)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_rl_empty(l)) {
        return kls_rl_cons(kls, el, l);
    } else {
        //Insert KLS_list_element according to its begin_offset
        if (el->begin_offset <= kls_rl_head(l)->begin_offset) {
            return kls_rl_cons(kls, el, l);
        } else {
            return kls_rl_cons(kls, kls_rl_head(l), kls_rl_insord(kls, el, kls_rl_tail(l)));
        }
    }
}

KLS_Region_List kls_rl_insord_p(Koliseo *kls, KLS_list_element el,
                                KLS_Region_List l)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    KLS_Region_List pprec, patt = l, paux;
    bool found = false;
    pprec = NULL;

    while (patt != NULL && !found) {
        if (el < patt->value) {
            found = true;
        } else {
            pprec = patt;
            patt = patt->next;
        }
    }
    switch (kls->conf.kls_reglist_alloc_backend) {
    case KLS_REGLIST_ALLOC_LIBC: {
        paux = (KLS_Region_List) malloc(sizeof(KLS_region_list_item));
    }
    break;
    case KLS_REGLIST_ALLOC_KLS_BASIC: {
        if (kls->reglist_kls == NULL) {
            fprintf(stderr,
                    "[ERROR]   at %s(): Koliseo->reglist_kls was NULL.\n",
                    __func__);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "at %s(): Koliseo->reglist_kls was NULL.\n", __func__);
#endif
            kls_free(kls);
            exit(EXIT_FAILURE);
        }
        paux = KLS_PUSH(kls->reglist_kls, KLS_region_list_item);
    }
    break;
    default: {
        fprintf(stderr,
                "[ERROR]    at %s(): Unexpected conf.kls_reglist_alloc_backend value: {%i}.\n",
                __func__, kls->conf.kls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "ERROR",
                "at %s(): Unexpected conf.kls_reglist_alloc_backend value: {%i}.\n",
                __func__, kls->conf.kls_reglist_alloc_backend);
#endif
        kls_free(kls);
        exit(EXIT_FAILURE);
    }
    break;
    }
    paux->value = el;
    paux->next = patt;
    if (patt == l) {
        return paux;
    } else {
        pprec->next = paux;
        return l;
    }
}

KLS_Region_List kls_rl_mergeList(Koliseo *kls, KLS_Region_List l1,
                                 KLS_Region_List l2)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_rl_empty(l1)) {
        return l2;
    } else {
        if (kls_rl_empty(l2)) {
            return l1;
        } else {
            if (kls_rl_isLess(kls_rl_head(l1), kls_rl_head(l2))) {
                return kls_rl_cons(kls, kls_rl_head(l1),
                                   kls_rl_mergeList(kls, kls_rl_tail(l1), l2));
            } else {
                if (kls_rl_isEqual(kls_rl_head(l1), kls_rl_head(l2))) {
                    return kls_rl_cons(kls, kls_rl_head(l1),
                                       kls_rl_mergeList(kls, kls_rl_tail(l1),
                                                        kls_rl_tail(l2)));
                } else {
                    return kls_rl_cons(kls, kls_rl_head(l2),
                                       kls_rl_mergeList(kls, l1, kls_rl_tail(l2)));
                }
            }
        }
    }
}

KLS_Region_List kls_rl_intersect(Koliseo *kls, KLS_Region_List l1,
                                 KLS_Region_List l2)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_rl_empty(l1) || kls_rl_empty(l2)) {
        return kls_rl_emptyList();
    }

    if (kls_rl_member(kls_rl_head(l1), l2) && !kls_rl_member(kls_rl_head(l1), kls_rl_tail(l1))) {
        return kls_rl_cons(kls, kls_rl_head(l1),
                           kls_rl_intersect(kls, kls_rl_tail(l1), l2));
    }

    else {
        return kls_rl_intersect(kls, kls_rl_tail(l1), l2);
    }
}

KLS_Region_List kls_rl_diff(Koliseo *kls, KLS_Region_List l1, KLS_Region_List l2)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_rl_empty(l1) || kls_rl_empty(l2)) {
        return l1;
    }

    else {
        if (!kls_rl_member(kls_rl_head(l1), l2)
            && !kls_rl_member(kls_rl_head(l1), kls_rl_tail(l1))) {
            return kls_rl_cons(kls, kls_rl_head(l1), kls_rl_diff(kls, kls_rl_tail(l1), l2));
        } else {
            return kls_rl_diff(kls, kls_rl_tail(l1), l2);
        }
    }
}

/**
 * Compares two regions and returns true if the first one has a smaller size.
 * @param r1 The KLS_Region expected to be smaller
 * @param r2 The KLS_Region expected to be bigger
 * @return True if first region size is less than second region size.
 */
bool kls_rl_isLess(KLS_Region *r1, KLS_Region *r2)
{
    //Compare regions by their effective size
    ptrdiff_t s1 = r1->end_offset - r1->begin_offset;
    ptrdiff_t s2 = r2->end_offset - r2->begin_offset;
    return (s1 < s2);
}

/**
 * Compares two regions and returns true if their size is equal.
 * @param r1 The first KLS_Region
 * @param r2 The second KLS_Region
 * @return True if first region size is equal than second region size.
 */
bool kls_rl_isEqual(KLS_Region *r1, KLS_Region *r2)
{
    //Compare regions by their effective size
    ptrdiff_t s1 = r1->end_offset - r1->begin_offset;
    ptrdiff_t s2 = r2->end_offset - r2->begin_offset;
    return (s1 == s2);
}

/**
 * Returns the ratio of memory used by the passed KLS_Region relative to the passed Koliseo as a double.
 * @param r The KLS_Region to check relative size for.
 * @param kls The Koliseo to check on.
 * @return A double representing percentage usage.
 */
double kls_usageShare(KLS_Region *r, Koliseo *kls)
{
    if (kls == NULL) {
        fprintf(stderr, "[KLS]    %s():  passed Koliseo was NULL.\n", __func__);
        return -1;
    }
    if (r == NULL) {
        kls_log(kls, "ERROR", "kls_usageShare():  passed KLS_Region was NULL");
        return -1;
    }
    ptrdiff_t r_size = r->end_offset - r->begin_offset;
    double res = (r_size * 100.0) / kls->size;
    return res;
}

/**
 * Return size of a passed KLS_Region. Sugar.
 * @param r The KLS_Region.
 * @return Region size as ptrdiff_t.
 */
ptrdiff_t kls_regionSize(KLS_Region *r)
{
    return r->end_offset - r->begin_offset;
}

/**
 * Return average region size in usage for the passed Koliseo.
 * @param kls The Koliseo to check usage for.
 * @return Average region size as ptrdiff_t.
 */
ptrdiff_t kls_avg_regionSize(Koliseo *kls)
{
    if (kls == NULL) {
        fprintf(stderr, "[KLS]    %s():  passed Koliseo was NULL.\n", __func__);
        return -1;
    }
    KLS_Region_List rl = kls->regs;
    ptrdiff_t res = 0;
    int tot_regs = kls_rl_length(rl);
    if (tot_regs > 0) {
        int tot_size = 0;
        while (!kls_rl_empty(rl)) {
            ptrdiff_t curr_size = 0;
            if (rl->value->size > 0) {
                curr_size = rl->value->size;
            } else {
                curr_size = kls_regionSize(rl->value);
                rl->value->size = curr_size;
            }
            tot_size += curr_size;
            rl = kls_rl_tail(rl);
        }
        res = (ptrdiff_t)((double)tot_size / tot_regs);
    }
    return res;
}

/**
 * Prints an usage report for the passed Koliseo to the passed file.
 * @see kls_usageShare()
 * @param kls The Koliseo to check.
 * @param fp The file pointer to print to.
 */
void kls_usageReport_toFile(Koliseo *kls, FILE *fp)
{
    if (kls == NULL) {
#ifdef KLS_DEBUG_CORE
        fprintf(stderr, "[KLS]    %s():  passed Koliseo was NULL", __func__);
#endif
        return;
    }
    if (fp == NULL) {
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "ERROR",
                "kls_usageReport_toFile():  passed file was NULL");
#endif
        return;
    }
    KLS_Region_List rl = kls->regs;
    int i = 0;
    while (!kls_rl_empty(rl)) {
        fprintf(fp, "Usage for region (%i) [%s]:  [%.3f%%]\n", i,
                rl->value->name, kls_usageShare(rl->value, kls));
        rl = kls_rl_tail(rl);
        i++;
    }
}

/**
 * Print usage report for passed Koliseo to stdout.
 * @see kls_usageReport_toFile()
 * @param kls The Koliseo to print info for.
 */
void kls_usageReport(Koliseo *kls)
{
    kls_usageReport_toFile(kls, stdout);
}

/**
 * Calc memory used by the specific type of KLS_list_element.
 * @see KLS_Region_List
 * @param type The integer corresponding to element->type
 * @param kls The Koliseo to check usage for.
 * @return The used memory size as ptrdiff_t.
 */
ptrdiff_t kls_type_usage(int type, Koliseo *kls)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    KLS_Region_List rl = kls->regs;

    ptrdiff_t res = 0;

    while (!kls_rl_empty(rl)) {
        KLS_list_element h = kls_rl_head(rl);
        if (h->type == type) {
            res += (h->end_offset - h->begin_offset);
        }
        rl = kls_rl_tail(rl);
    }

    return res;
}

/**
 * Calc memory used as padding for the passed Koliseo.
 * @see KLS_Region_List
 * @param kls The Koliseo to check total padding for.
 * @return The total padding size as ptrdiff_t.
 */
ptrdiff_t kls_total_padding(Koliseo * kls)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    KLS_Region_List rl = kls->regs;

    ptrdiff_t res = 0;

    while (!kls_rl_empty(rl)) {
        res += kls_rl_head(rl)->padding;
        rl = kls_rl_tail(rl);
    }
    return res;
}
#endif // KOLISEO_HAS_REGION

#ifdef KOLISEO_HAS_GULP

/**
 * Contains the constant string representation of Gulp_Res values.
 * @see Gulp_Res
 */
const char* gulp_res_names[TOT_GULP_RES+1] = {
    [GULP_FILE_OK] = "Success",
    [GULP_FILE_NOT_EXIST] = "File does not exist",
    [GULP_FILE_TOO_LARGE] = "File is too large",
    [GULP_FILE_READ_ERROR] = "File could not be read",
    [GULP_FILE_CONTAINS_NULLCHAR] = "File contains nullchar",
    [GULP_FILE_KLS_NULL] = "Koliseo was NULL",
    [TOT_GULP_RES] = "Total of Gulp_Res values",
};

/**
 * Return a constant string for the passed Gulp_Res.
 * @see gulp_res_names
 * @see Gulp_Res
 * @param g The Gulp_Res to get a string for.
 * @return A constant string representation of passed Gulp_Res.
 */
const char* string_from_Gulp_Res(Gulp_Res g)
{
    assert(g >= 0 && g < TOT_GULP_RES && "Unexpected Gulp_Res value");
    return gulp_res_names[g];
}

/**
 * Returns a new Kstr with the passed args set.
 * @see Kstr
 * @param data The string pointer to set.
 * @param len The len to set.
 * @return The resulting Kstr.
 */
Kstr kstr_new(const char* data, size_t len)
{
    return (Kstr) {
        .data = data,
        .len = len,
    };
}

/**
 * Returns a new Kstr from the passed null-terminated string.
 * @see Kstr
 * @param c_lit The cstring pointer to set.
 * @return The resulting Kstr.
 */
Kstr kstr_from_c_lit(const char* c_lit)
{
    return kstr_new(c_lit, strlen(c_lit));
}

/**
 * Checks if the two passed Kstr have exactly equal data.
 * @see Kstr
 * @param left The first Kstr to compare.
 * @param right The second Kstr to compare.
 * @return A bool result for the comparation.
 */
bool kstr_eq(Kstr left, Kstr right)
{
    if (left.len != right.len) {
        return false;
    }

    for (size_t i=0; i < left.len; i++) {
        if (left.data[i] != right.data[i]) return false;
    }
    return true;
}

/**
 * Checks if the two passed Kstr have equal data, ignoring case.
 * @see Kstr
 * @param left The first Kstr to compare.
 * @param right The second Kstr to compare.
 * @return A bool result for the comparation.
 */
bool kstr_eq_ignorecase(Kstr left, Kstr right)
{
    if (left.len != right.len) {
        return false;
    }

    char l, r;
    for (size_t i=0; i < left.len; i++) {
        l = 'A' <= left.data[i] && 'Z' >= left.data[i]
            ? left.data[i] + 32
            : left.data[i];
        r = 'A' <= right.data[i] && 'Z' >= right.data[i]
            ? right.data[i] + 32
            : right.data[i];
        if (l != r) return false;
    }
    return true;
}

/**
 * Cuts the passed Kstr by up to n chars, from the left. Returns cut portion as a new Kstr.
 * @see Kstr
 * @param k The Kstr to cut.
 * @param n How many chars to cut.
 * @return The cut part as a new Kstr.
 */
Kstr kstr_cut_l(Kstr *k, size_t n)
{
    if (n > k->len) {
        n = k->len;
    }
    Kstr res = kstr_new(k->data, n);
    k->data += n;
    k->len -= n;

    return res;
}

/**
 * Cuts the passed Kstr by up to n chars, from the right. Returns cut portion as a new Kstr.
 * @see Kstr
 * @param k The Kstr to cut.
 * @param n How many chars to cut.
 * @return The cut part as a new Kstr.
 */
Kstr kstr_cut_r(Kstr *k, size_t n)
{
    if (n > k->len) {
        n = k->len;
    }
    Kstr res = kstr_new(k->data + k->len - n, n);
    k->len -= n;

    return res;
}

/**
 * Returns a new Kstr after removing heading spaces from the passed one.
 * @see Kstr
 * @param kstr The Kstr to trim.
 * @return The resulting Kstr.
 */
Kstr kstr_trim_left(Kstr kstr)
{
    size_t i = 0;
    while ( i < kstr.len && isspace(kstr.data[i])) {
        i++;
    }
    return kstr_new(kstr.data + i, kstr.len - i);
}

/**
 * Returns a new Kstr after removing trailing spaces from the passed one.
 * @see Kstr
 * @param kstr The Kstr to trim.
 * @return The resulting Kstr.
 */
Kstr kstr_trim_right(Kstr kstr)
{
    size_t i = 0;
    while ( i < kstr.len && isspace(kstr.data[kstr.len - i - 1])) {
        i++;
    }
    return kstr_new(kstr.data, kstr.len - i);
}

/**
 * Returns a new Kstr after removing heading and trailing spaces from the passed one.
 * @see Kstr
 * @see kstr_trim_l()
 * @see kstr_trim_r()
 * @param kstr The Kstr to trim.
 * @return The resulting Kstr.
 */
Kstr kstr_trim(Kstr kstr)
{
    return kstr_trim_left(kstr_trim_right(kstr));
}

/**
 * Checks if passed Kstr contains the passed char, and if so, sets the value pointed by idx to the first occurrence.
 * @see Kstr
 * @param k The Kstr to scan.
 * @param c The char to look for.
 * @param idx Pointer to the value to be set as index.
 * @return false if the passed Kstr doesn't contain the passed char, true otherwise.
 */
bool kstr_indexof(Kstr k, char c, int* idx)
{
    if (k.len == 0) {
        return false;
    } else {
        size_t i = 0;
        while (i < k.len) {
            if (k.data[i] == c) {
                *idx = i;
                return true;
            }

            i++;
        }
        return false;
    }
}

/**
 * Scans the first passed Kstr and if the passed char is present, the old Kstr is set to second pointer and the first one is cut at the first occurrence of it.
 * @see Kstr
 * @param k The Kstr to scan.
 * @param delim The char to look for.
 * @param part The Kstr to set to the original data, if the delimiter is found.
 * @return false if the passed Kstr doesn't contain the passed char, true otherwise.
 */
bool kstr_try_token(Kstr *k, char delim, Kstr* part)
{
    size_t i = 0;
    while (i < k->len && k->data[i] != delim) {
        i++;
    }

    Kstr res = kstr_new(k->data,i);

    if (i < k->len) {
        k->len -= i +1;
        k->data += i +1;
        if (part) {
            *part = res;
        }
        return true;
    }

    return false;
}

/**
 * Scans the passed Kstr and cuts it up to the first occurrence of passed char, even if it is not present. Returns a new Kstr with the original data.
 * @see Kstr
 * @param k The Kstr to scan.
 * @param delim The char to look for.
 * @return A new Kstr with the original data.
 */
Kstr kstr_token(Kstr *k, char delim)
{
    size_t i = 0;
    while (i < k->len && k->data[i] != delim) {
        i++;
    }

    Kstr res = kstr_new(k->data,i);

    if (i < k->len) {
        k->len -= i +1;
        k->data += i +1;
    } else {
        k->len -= i;
        k->data += i;
    }

    return res;
}

Kstr kstr_token_kstr(Kstr* k, Kstr delim)
{

    //Kstr to scroll k data, sized as the delimiter
    Kstr win = kstr_new(k->data, delim.len);

    size_t i = 0;

    //Loop checking if k data can still be scrolled and if current window is equal to the delimiter
    while (i + delim.len < k->len &&
           !(kstr_eq(win, delim))) {
        i++;
        win.data++;
    }

    //New Kstr just up to the delimiter position
    Kstr res = kstr_new(k->data, i);

    //If we don't cleanly empty k, we increase result len so that it holds the remaining chars
    if (i + delim.len == k->len) {
        res.len += delim.len;
    }

    //Advance k by the delimiter size, plus its starting position
    k->data += i + delim.len;
    k->len += i + delim.len;

    return res;
}

static char * kls_read_file(Koliseo* kls, const char * f_name, Gulp_Res * err, size_t * f_size, ...)
{
    if (!kls) {
        *err = GULP_FILE_KLS_NULL;
        return NULL;
    }
    char * buffer;
    size_t length = 0;
    FILE * f = fopen(f_name, "rb");
    size_t read_length;

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);

        va_list args;
        va_start(args, f_size);
        size_t max_size = va_arg(args, size_t);
        if (length > max_size) {
            *err = GULP_FILE_TOO_LARGE;

            return NULL;
        }
        va_end(args);

        buffer = KLS_PUSH_ARR_NAMED(kls,char,length + 1,"char*","Buffer for file gulp");

        if (buffer == NULL) {
            assert(0 && "KLS_PUSH_NAMED() failed\n");
        }

        if (length) {
            read_length = fread(buffer, 1, length, f);

            if (length != read_length) {
                *err = GULP_FILE_READ_ERROR;
                return NULL;
            }
        }

        fclose(f);

        *err = GULP_FILE_OK;
        buffer[length] = '\0';
        *f_size = length;
    } else {
        *err = GULP_FILE_NOT_EXIST;

        return NULL;
    }

    if (strlen(buffer) == length) {
        return buffer;
    } else {
        *err = GULP_FILE_CONTAINS_NULLCHAR;
        return buffer;
    }
}

/**
 * Tries mapping the passed file on the Koliseo.
 * Sets the passed Gulp_Res to the result of the operation, .
 * @param kls The Koliseo to push to.
 * @param filepath Path to the file to gulp.
 * @param err Pointer to the Gulp_Res variable to store result.
 * @param max_size Max size allowed for the read file.
 * @see KLS_GULP_FILE()
 */
char * kls_gulp_file_sized(Koliseo* kls, const char * filepath, Gulp_Res * err, size_t max_size)
{
    static_assert(TOT_GULP_RES == 6, "Number of Gulp_Res changed");
    size_t f_size;
    char * data = NULL;
    data = kls_read_file(kls, filepath, err, &f_size, max_size);
    if (*err != GULP_FILE_OK) {
        switch (*err) {
        case GULP_FILE_NOT_EXIST:
        case GULP_FILE_TOO_LARGE:
        case GULP_FILE_READ_ERROR:
        case GULP_FILE_CONTAINS_NULLCHAR:
        case GULP_FILE_KLS_NULL: {
            fprintf(stderr,"[ERROR]    %s():  {" Gulp_Res_Fmt "}.\n",__func__, Gulp_Res_Arg(*err));
        }
        break;
        default: {
            fprintf(stderr,"[ERROR]    %s():  Unexpected error {%i}.\n",__func__, *err);
        }
        break;
        }
        if (*err != GULP_FILE_CONTAINS_NULLCHAR) return NULL;
    } else {
        assert(strlen(data) == f_size && "data len should be equal to f_size here!");
        if (!data) {
            assert(0 && "kls_read_file() failed\n");
        }
        //printf("%s\n\n",data);
        //printf("SIZE: {%i}\n",f_size);
    }
    return data;
}

/**
 * Tries mapping the passed file on the Koliseo.
 * @param kls The Koliseo to push to.
 * @param filepath Path to the file to gulp.
 * @param max_size Max size allowed for the read file.
 * @see KLS_GULP_FILE()
 * @return A pointer to the string with file contents.
 */
char * try_kls_gulp_file(Koliseo* kls, const char * filepath, size_t max_size)
{
    Gulp_Res err = -1;

    char* res = kls_gulp_file_sized(kls, filepath, &err, max_size);

    if (err != GULP_FILE_OK && err != GULP_FILE_CONTAINS_NULLCHAR) {
        fprintf(stderr, "%s():  kls_gulp_file_sized() failed with err {%s}.\n",__func__,string_from_Gulp_Res(err));
    }

    return res;
}

static Kstr * kls_read_file_to_kstr(Koliseo* kls, const char * f_name, Gulp_Res * err, size_t * f_size, ...)
{
    if (!kls) {
        *err = GULP_FILE_KLS_NULL;
        return NULL;
    }
    char * buffer = NULL;
    size_t length = 0;
    FILE * f = fopen(f_name, "rb");
    size_t read_length;
    bool allow_nullchar = false;

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);

        va_list args;
        va_start(args, f_size);
        size_t max_size = va_arg(args, size_t);
        if (length > max_size) {
            *err = GULP_FILE_TOO_LARGE;

            return NULL;
        }
        bool allow_nulls = va_arg(args, int);
        allow_nullchar = allow_nulls;
        va_end(args);

        buffer = KLS_PUSH_ARR_NAMED(kls,char,length + 1,"char*","Buffer for file gulp");

        if (buffer == NULL) {
            assert(0 && "KLS_PUSH_NAMED() failed\n");
        }

        if (length) {
            read_length = fread(buffer, 1, length, f);

            if (length != read_length) {
                *err = GULP_FILE_READ_ERROR;
                return NULL;
            }
        }

        fclose(f);

        *err = GULP_FILE_OK;
        buffer[length] = '\0';
        *f_size = length;
    } else {
        *err = GULP_FILE_NOT_EXIST;

        return NULL;
    }

    if (strlen(buffer) == length) {
    } else {
        *err = GULP_FILE_CONTAINS_NULLCHAR;
        if (!allow_nullchar) {
            return NULL;
        }
    }
    Kstr * res = KLS_PUSH_NAMED(kls,Kstr,"Kstr","Kstr for file gulp");
    if (res == NULL) {
        assert(0 && "KLS_PUSH_NAMED() failed\n");
    }
    res->data = buffer;
    if (*err == GULP_FILE_CONTAINS_NULLCHAR) {
        res->len = length;
    } else {
        res->len = strlen(buffer);
    }
    return res;
}

/**
 * Tries mapping the passed file on the Koliseo.
 * Sets the passed Gulp_Res to the result of the operation.
 * @param kls The Koliseo to push to.
 * @param filepath Path to the file to gulp.
 * @param err Pointer to the Gulp_Res variable to store result.
 * @param max_size Max size allowed for the read file.
 * @param allow_nullchar Bool to avoid returning NULL for a binary file.
 * @see KLS_GULP_FILE()
 * @return A Kstr for the passed filepath contents.
 */
Kstr * kls_gulp_file_sized_to_kstr(Koliseo* kls, const char * filepath, Gulp_Res * err, size_t max_size, bool allow_nullchar)
{
    static_assert(TOT_GULP_RES == 6, "Number of Gulp_Res changed");
    size_t f_size;
    Kstr * data = NULL;
    data = kls_read_file_to_kstr(kls, filepath, err, &f_size, max_size, allow_nullchar);
    if (*err != GULP_FILE_OK) {
        switch (*err) {
        case GULP_FILE_NOT_EXIST:
        case GULP_FILE_TOO_LARGE:
        case GULP_FILE_READ_ERROR:
        case GULP_FILE_CONTAINS_NULLCHAR:
        case GULP_FILE_KLS_NULL: {
            fprintf(stderr,"[ERROR]    %s():  {" Gulp_Res_Fmt "}.\n",__func__, Gulp_Res_Arg(*err));
        }
        break;
        default: {
            fprintf(stderr,"[ERROR]    %s():  Unexpected error {%i}.\n",__func__, *err);
        }
        break;
        }
        return data;
    } else {
        assert(data->len == f_size && "data len should be equal to f_size here!");
        if (!data) {
            assert(0 && "kls_read_file_to_kstr() failed\n");
        }
        //printf("%s\n\n",data->data);
        //printf("SIZE: {%i}\n",f_size);
    }
    return data;
}

/**
 * Tries mapping the passed file on the Koliseo.
 * @param kls The Koliseo to push to.
 * @param filepath Path to the file to gulp.
 * @param max_size Max size allowed for the read file.
 * @param allow_nullchar Boolean to avoid returning NULL for a binary file.
 * @see KLS_GULP_FILE()
 * @return A pointer to the Kstr with file contents.
 */
Kstr * try_kls_gulp_file_to_kstr(Koliseo* kls, const char * filepath, size_t max_size, bool allow_nullchar)
{
    Gulp_Res err = -1;

    Kstr * res = NULL;
    res = kls_gulp_file_sized_to_kstr(kls, filepath, &err, max_size, allow_nullchar);

    if (err != GULP_FILE_OK) {
        fprintf(stderr, "%s():  kls_gulp_file_sized_to_kstr() failed with err {%s}.\n",__func__,string_from_Gulp_Res(err));
    }

    return res;
}

#endif //KOLISEO_HAS_GULP

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
#ifdef KOLISEO_HAS_REGION
    if (kls->conf.kls_autoset_regions == 1) {
        KLS_region_list_item *reg = NULL;
        switch (kls->conf.kls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC:
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            reg = kls_list_pop(kls);
        }
        break;
        default: {
            fprintf(stderr,
                    "[ERROR] [%s()]:  Unexpected KLS_RegList_Alloc_Backend value: {%i}.\n",
                    __func__, kls->conf.kls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "%s():  Invalid KLS_RegList_Alloc_Backend value: {%i}.",
                    __func__, kls->conf.kls_reglist_alloc_backend);
#endif
            kls_free(kls);
            exit(EXIT_FAILURE);
        }
        break;
        }
        //void* region_pointer = kls->begin + reg->begin_offset;
        //TODO: do something with the region item before returning
        (void) reg;
    }
#endif // KOLISEO_HAS_REGION
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "API Level { %i } -> Popped (%li) for KLS.",
            int_koliseo_version(), size);
    if (kls->conf.kls_verbose_lvl > 0) {
        print_kls_2file(kls->conf.kls_log_fp, kls);
    }
#endif
    if (kls->conf.kls_collect_stats == 1) {
        kls->stats.tot_pops += 1;
#ifdef KOLISEO_HAS_REGION
        kls->stats.avg_region_size = kls_avg_regionSize(kls);
#endif
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
#ifdef KOLISEO_HAS_REGION
    if (t_kls->conf.kls_autoset_regions == 1) {
        KLS_region_list_item *reg = NULL;
        switch (t_kls->conf.tkls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC:
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            reg = kls_t_list_pop(t_kls);
        }
        break;
        default: {
            fprintf(stderr,
                    "[ERROR] [%s()]:  Unexpected KLS_RegList_Alloc_Backend value: {%i}.\n",
                    __func__, t_kls->conf.tkls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "%s():  Invalid KLS_RegList_Alloc_Backend value: {%i}.",
                    __func__, t_kls->conf.tkls_reglist_alloc_backend);
#endif
            kls_free(t_kls->kls);
            exit(EXIT_FAILURE);
        }
        break;
        }
        //void* region_pointer = kls->begin + reg->begin_offset;
        //TODO: do something with the region item before returning
        (void) reg;
    }
#endif // KOLISEO_HAS_REGION
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
