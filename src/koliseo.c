// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
/*
    Copyright (C) 2023  jgabaut

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
    .kls_autoset_regions = 0,
    .kls_reglist_alloc_backend = KLS_REGLIST_ALLOC_LIBC,
    .kls_reglist_kls_size = 0,
    .kls_autoset_temp_regions = 0,
    .kls_collect_stats = 0,
    .kls_verbose_lvl = 0,
    .kls_log_fp = NULL,
    .kls_log_filepath = "",
};

KLS_Stats KLS_STATS_DEFAULT = {
    .tot_pushes = 0,
    .tot_temp_pushes = 0,
    .tot_pops = 0,
    .tot_temp_pops = 0,
    .tot_logcalls = 0,
    .tot_hiccups = 0,
    .avg_region_size = 0,
    .worst_pushcall_time = -1,
};

bool kls_set_conf(Koliseo * kls, KLS_Conf conf);	//Declare function used internally by kls_new() and kls_new_conf()

/**
 * Defines title banner.
 */
char *kls_title[KLS_TITLEROWS + 1] = {
    "                               .',,,.                                                               ",
    "                        ...''',,;;:cl;.                                                             ",
    "                  ..''''''....      .co,                                                            ",
    "              .'',,,'..               'ol.                                                          ",
    "          ..''''..                     .co.                                                         ",
    "        ..'..                            cl.                                                        ",
    "      ..'...               .          .  .:'...       .       .      ..      .....     ...          ",
    "     ....      .      ..:c,'.   .oc .ll.  :dddxo.    ld.     'x:  .cxddx;   :kxodo,  .lddxx;        ",
    "    .,..     .cl,..   .cxd:..   ;Ol;do.  :Ol. c0c   .kx.     lO;  ;0d..::  .xk'     .xx' 'kx.       ",
    "    .,..     'dOl.     .''......lKOkc.  .kx.  ;0l   ;0l     .xk.  'xk;     'Ok;'.   l0;  .xk.       ",
    "    ''.;,    .,;;............. .xKKO'   ;0c   c0:   lO;     'Od.   .lko.   :0koo:. .xk.  'Od.       ",
    "   ',.'c;.  .......            'Oo:ko.  c0;  .xk.  .xx.     :0c  .   ;Od. .oO,     'Od.  c0:        ",
    "   ,;  .......        .::.     :O; lO;  :0l..okc.  'Od...  .oO' .ld'.:Od. 'kk,...  .kk,.:kd.        ",
    "   ;xc,...  .   'd:   .:d;     ;l. .l:  .:dool''c. 'ddolc. .cc.  'ldooc.  'dxoll:.  'odoo:.         ",
    "  .lx;. .  ,d;  .ll.   .''.                     ;l:'                                                ",
    "  :o.   ;,  cc    ..     .                       ,cc:::c,                                           ",
    " .c;    ..  .'           ....',,;;;;;;;;;;,,,,,,,,,,;;;ox;........'cddoollcc:;,,'..                 ",
    "  ,:         ...',:clodxkO0KKXXXXXXKKK000000KKKKXXXXKKKXXXKKKKKKKKKXNNNNNNNNNNNXXKOxoc;'.           ",
    "  .:. ..';cldkOKXXXXK0Okxxdolc::;,''.','......';clc::cloONNNNNNNNNNNNNNNNNNNNNNNNNNNNNNK0xl;.       ",
    "  :xxxOKKK0kxoddl;,';od;.   'cl,    .ckko.    ,d00Ol.   .xXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNKkl;.   ",
    ".lKX0xoc,';;. .ll.   ;xO;   .oX0,    ,ONXl    ,0NNNK;    .lKNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNX0o' ",
    ".xk;.  .  .;.  .:,    .dc    'OK;    .dNNo.   ;0NNNXc      ;kXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNk.",
    " :c        ..   ..    .l,    .k0,    .xXXo    ;0XNNXc       .l0NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNO'",
    " :;                   .'.    .;:.    .,cl;....,loddo;..       'dKNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN0,",
    ".c,        ....',;;:cclllooddddddddddodxxxxxxxdddddddolllccccc:cxXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNK;",
    ".l:',:cloxxkkkOkkxxdollcc::;,,'''.............................',cONNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXc",
    ":00OOxdoc:;,'...                     ..        ..         .     .oNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXl",
    ":o,..',. ..   ';.   .co'     ;d,    'oOk;    .o00kd;    .oOkd,   :KNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXl",
    ":;   ,o'  ..  .l:    ;0o    .dXo.   'xNNk.   'ONNNNO'   :KNNNd.   cKNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXo",
    "c;    ..  ..   ';    'ko.   'ONo.   .kNNO'   ,0NNNNK;   :KNNXd.    :0NNNNNNNNNNNNNNNNNNNNNNNNNNNNNXo",
    "l,             ..    'ko    'OXl    ,0NNO'   'ONNNNK;   ;KNNXl.     ;ONNNNNNNNNNNNNNNNNNNNNNNNNNNNXl",
    "cc.............';'...;do'...:k0c....:0XX0:...'xXXXX0c...:0XK0l.......c0NNNNNNNNNNNNNNNNNNNNXXXXXXKO,",
    ".;:;;;;;;:::::::::cc:::::::::c:;;;;,;ccc:;,,,,:cccc:;;;;;ccc:;,,,,,;;;clloooooooooooooooooollcc:;,. "
};

/**
 * Prints the title banner to the passed FILE pointer.
 * @see kls_title
 * @param fp The FILE to print to.
 */
void kls_print_title_2file(FILE *fp)
{
    if (fp == NULL) {
        fprintf(stderr,
                "[KLS] kls_print_title_2file():  Passed file pointer was NULL.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < KLS_TITLEROWS; i++) {
        fprintf(fp, "%s\n", kls_title[i]);
    }
}

/**
 * Prints the title banner to stdout.
 * @see kls_title
 */
void kls_print_title(void)
{
    kls_print_title_2file(stdout);
}

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
const int int_koliseo_version(void)
{
    return KOLISEO_API_VERSION_INT;
}

/**
 * Returns the current offset (position of pointer bumper) for the passed Koliseo.
 * @param kls The Koliseo at hand.
 * @return A ptrdiff_t value for current position.
 */
ptrdiff_t kls_get_pos(Koliseo *kls)
{
    return kls->offset;
}

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
            struct tm *mytime = localtime(&now);
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
 * Takes a ptrdiff_t size and allocates the backing memory for a Koliseo.
 * Sets the KLS_Conf field to KLS_DEFAULT_CONF.
 * Sets the fields with appropriate values if memory allocation was successful, goes to exit() otherwise.
 * @param size The size for Koliseo data field.
 * @return A pointer to the initialised Koliseo struct.
 * @see Koliseo
 * @see Koliseo_Temp
 * @see KLS_DEFAULT_CONF
 * @see kls_temp_start()
 * @see kls_temp_end()
 */
Koliseo *kls_new(ptrdiff_t size)
{
    if (size < (ptrdiff_t)sizeof(Koliseo)) {
#ifndef _WIN32
        fprintf(stderr,
                "[ERROR]    at %s():  invalid requested kls size (%li). Min accepted is: (%li).\n",
                __func__, size, (ptrdiff_t)sizeof(Koliseo));
#else
        fprintf(stderr,
                "[ERROR]    at %s():  invalid requested kls size (%lli). Min accepted is: (%lli).\n",
                __func__, size, (ptrdiff_t)sizeof(Koliseo));
#endif
        //TODO Is it better to abort the program?
        return NULL;
    }
    void *p = malloc(size);
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
        kls->reglist_kls = NULL;
        kls->max_regions_kls_alloc_basic = 0;
        kls_set_conf(kls, KLS_DEFAULT_CONF);
        kls->stats = KLS_STATS_DEFAULT;
        kls->conf.kls_log_fp = stderr;
        kls->regs = NULL;
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "KLS", "API Level { %i } ->  Allocated (%s) for new KLS.",
                int_koliseo_version(), h_size);
        kls_log(kls, "KLS", "KLS offset: { %p }.", kls);
        kls_log(kls, "KLS", "Allocation begin offset: { %p }.",
                kls + kls->offset);
#endif
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
            kls_header->name[KLS_REGION_MAX_NAME_SIZE - 1] = '\0';
            strncpy(kls_header->desc, "Sizeof Koliseo header",
                    KLS_REGION_MAX_DESC_SIZE);
            kls_header->desc[KLS_REGION_MAX_DESC_SIZE - 1] = '\0';
            //KLS_Region_List reglist = kls_emptyList();
            //reglist = kls_cons(kls,kls_header,reglist);
            //kls->regs = reglist;
            kls->regs = kls_cons(kls, kls_header, kls->regs);
            if (kls->regs == NULL) {
                fprintf(stderr,
                        "[KLS] [%s()]: failed to get a KLS_Region_List.\n",
                        __func__);
                exit(EXIT_FAILURE);
            }
        } else {
            kls->regs = NULL;
        }
    } else {
        fprintf(stderr, "[KLS] Failed kls_new() call.\n");
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
 * Takes a ptrdiff_t size and a KLS_Conf to configure the new Koliseo.
 * Calls kls_new() to initialise the Koliseo, the calls kls_set_conf() to update the config before returning the new Koliseo.
 * @param size The size for Koliseo data field.
 * @param conf The KLS_Conf for the new Koliseo.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see KLS_DEFAULT_CONF
 * @see kls_new()
 * @see kls_set_conf()
 */
Koliseo *kls_new_conf(ptrdiff_t size, KLS_Conf conf)
{
    Koliseo *k = kls_new(size);
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
 * Takes a ptrdiff_t size and a filepath for the trace output file, and returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf() to initialise the Koliseo with the proper config for a traced Koliseo, logging to the passed filepath.
 * @param size The size for Koliseo data field.
 * @param output_path The filepath for log output.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf()
 */
Koliseo *kls_new_traced(ptrdiff_t size, const char *output_path)
{
#ifndef KLS_DEBUG_CORE
    fprintf(stderr,
            "[WARN]    %s(): KLS_DEBUG_CORE is not defined. No tracing allowed.\n",
            __func__);
#endif
    KLS_Conf k = (KLS_Conf) {
        .kls_collect_stats = 1,.kls_verbose_lvl =
                                 1,.kls_log_filepath = output_path
    };
    return kls_new_conf(size, k);
}

/**
 * Takes a ptrdiff_t size and returns a pointer to the prepared Koliseo.
 * Calls kls_new_conf() to initialise the Koliseo with the proper config for a debug Koliseo (printing to stderr).
 * @param size The size for Koliseo data field.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf()
 */
Koliseo *kls_new_dbg(ptrdiff_t size)
{
#ifndef KLS_DEBUG_CORE
    fprintf(stderr,
            "[WARN]    %s(): KLS_DEBUG_CORE is not defined. No debugging support.\n",
            __func__);
#endif
    KLS_Conf k = (KLS_Conf) {
        .kls_collect_stats = 1,.kls_verbose_lvl = 0
    };
    Koliseo * kls = kls_new_conf(size, k);
    kls->conf.kls_verbose_lvl = 1;
    return kls;
}

/**
 * Takes a ptrdiff_t size and a filepath for the trace output file, and the needed parameters for a successful init of the prepared Koliseo.
 * Calls kls_new_conf() to initialise the Koliseo with the proper config for a traced Koliseo, logging to the passed filepath.
 * @param size The size for Koliseo data field.
 * @param output_path The filepath for log output.
 * @return A pointer to the initialised Koliseo struct, with wanted config.
 * @see Koliseo
 * @see KLS_Conf
 * @see kls_new_conf()
 */
Koliseo *kls_new_traced_AR_KLS(ptrdiff_t size, const char *output_path,
                               ptrdiff_t reglist_kls_size)
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
        .kls_reglist_alloc_backend = KLS_REGLIST_ALLOC_KLS_BASIC,
        .kls_reglist_kls_size = reglist_kls_size,
        .kls_autoset_regions = 1,
        .kls_autoset_temp_regions = 1,
    };
    return kls_new_conf(size, k);
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
#endif
    }

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

            kls_freeList(kls->regs);

            Koliseo *reglist_kls = NULL;
            reglist_kls = kls_new(kls->conf.kls_reglist_kls_size);

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
                (KLS_Region *) KLS_PUSH(kls->reglist_kls, KLS_Region, 1);
            kls_header->begin_offset = 0;
            kls_header->end_offset = kls->offset;
            kls_header->size =
                kls_header->end_offset - kls_header->begin_offset;
            kls_header->padding = 0;
            kls_header->type = KLS_Header;
            strncpy(kls_header->name, "KLS_Header",
                    KLS_REGION_MAX_NAME_SIZE);
            kls_header->name[KLS_REGION_MAX_NAME_SIZE - 1] = '\0';
            strncpy(kls_header->desc, "Sizeof Koliseo header",
                    KLS_REGION_MAX_DESC_SIZE);
            kls_header->desc[KLS_REGION_MAX_DESC_SIZE - 1] = '\0';
            //KLS_Region_List reglist = kls_emptyList();
            //reglist = kls_cons(kls,kls_header,reglist);
            //kls->regs = reglist;
            kls->regs = kls_cons(kls, kls_header, kls->regs);
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
        fprintf(stderr, "[KLS] Failed kls_temp_pop() call.\n");
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
 * Takes a Koliseo pointer, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the Koliseo data field, or goes to exit() if the operation fails.
 * Notably, it does NOT zero the memory region.
 * @param kls The Koliseo at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @return A void pointer to the start of memory just pushed to the Koliseo.
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
    ptrdiff_t available = kls->size - kls->offset;
    ptrdiff_t padding = -kls->offset & (align - 1);
    if (count > PTRDIFF_MAX / size || available - padding < size * count) {
        if (count > PTRDIFF_MAX / size) {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n",
                    count, PTRDIFF_MAX / size);
#else
            fprintf(stderr,
                    "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n",
                    count, PTRDIFF_MAX / size);
#endif
        } else {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n",
                    size * count, available - padding);
#else
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n",
                    size * count, available - padding);
#endif
        }
        fprintf(stderr, "[KLS] Failed kls_push() call.\n");
        kls_free(kls);
        exit(EXIT_FAILURE);
    }
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
        kls->stats.avg_region_size = kls_avg_regionSize(kls);
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
 * @return A void pointer to the start of memory just pushed to the Koliseo.
 */
void *kls_push_zero(Koliseo *kls, ptrdiff_t size, ptrdiff_t align,
                    ptrdiff_t count)
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
    ptrdiff_t available = kls->size - kls->offset;
    ptrdiff_t padding = -kls->offset & (align - 1);
    if (count > PTRDIFF_MAX / size || (available - padding) < (size * count)) {
        if (count > PTRDIFF_MAX / size) {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n",
                    count, PTRDIFF_MAX / size);
#else
            fprintf(stderr,
                    "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n",
                    count, PTRDIFF_MAX / size);
#endif
        } else {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n",
                    size * count, available - padding);
#else
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n",
                    size * count, available - padding);
#endif
        }
        fprintf(stderr, "[KLS] Failed kls_push_zero() call.\n");
        kls_free(kls);
        exit(EXIT_FAILURE);
        //return 0;
    }
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
        kls->stats.avg_region_size = kls_avg_regionSize(kls);
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
 * @return A void pointer to the start of memory just pushed to the Koliseo.
 */
void *kls_push_zero_AR(Koliseo *kls, ptrdiff_t size, ptrdiff_t align,
                       ptrdiff_t count)
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

    ptrdiff_t available = kls->size - kls->offset;
    ptrdiff_t padding = -kls->offset & (align - 1);
    if (count > PTRDIFF_MAX / size || (available - padding) < (size * count)) {
        if (count > PTRDIFF_MAX / size) {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n",
                    count, PTRDIFF_MAX / size);
#else
            fprintf(stderr,
                    "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n",
                    count, PTRDIFF_MAX / size);
#endif
        } else {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n",
                    size * count, available - padding);
#else
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n",
                    size * count, available - padding);
#endif
        }
        fprintf(stderr, "[KLS] Failed kls_push_zero() call.\n");
        kls_free(kls);
        exit(EXIT_FAILURE);
        //return 0;
    }
    char *p = kls->data + kls->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    kls->prev_offset = kls->offset;
    kls->offset += padding + size * count;
    if (kls->conf.kls_autoset_regions == 1) {
        KLS_Region *reg = NULL;
        switch (kls->conf.kls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            reg = (KLS_Region *) malloc(sizeof(KLS_Region));
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            if (kls_length(kls->regs) < kls->max_regions_kls_alloc_basic) {
                reg = KLS_PUSH(kls->reglist_kls, KLS_Region, 1);
            } else {
                fprintf(stderr,
                        "[ERROR]    [%s()]:  Exceeding kls->max_regions_kls_alloc_basic: {%i}.\n",
                        __func__, kls->max_regions_kls_alloc_basic);
                if (kls->conf.kls_verbose_lvl > 0) {
                    kls_log(kls, "ERROR",
                            "[%s()]:  Exceeding kls->max_regions_kls_alloc_basic: {%i}.",
                            __func__, kls->max_regions_kls_alloc_basic);
                    kls_showList_toFile(kls->regs, kls->conf.kls_log_fp);
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
        reg->begin_offset = kls->prev_offset;
        reg->end_offset = kls->offset;
        reg->size = reg->end_offset - reg->begin_offset;
        reg->padding = padding;
        reg->type = KLS_None;
        strncpy(reg->name, KOLISEO_DEFAULT_REGION_NAME,
                KLS_REGION_MAX_NAME_SIZE);
        reg->name[KLS_REGION_MAX_NAME_SIZE - 1] = '\0';
        strncpy(reg->desc, KOLISEO_DEFAULT_REGION_DESC,
                KLS_REGION_MAX_DESC_SIZE);
        reg->desc[KLS_REGION_MAX_DESC_SIZE - 1] = '\0';
        //KLS_Region_List reglist = kls_emptyList();
        //reglist = kls_cons(kls,reg,reglist);
        //kls->regs = kls_append(kls,reglist, kls->regs);
        kls->regs = kls_cons(kls, reg, kls->regs);
    }

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
        kls->stats.avg_region_size = kls_avg_regionSize(kls);
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
void *kls_temp_push_zero_AR(Koliseo_Temp *t_kls, ptrdiff_t size,
                            ptrdiff_t align, ptrdiff_t count)
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
    ptrdiff_t available = kls->size - kls->offset;
    ptrdiff_t padding = -kls->offset & (align - 1);
    if (count > PTRDIFF_MAX / size || (available - padding) < (size * count)) {
        if (count > PTRDIFF_MAX / size) {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n",
                    count, PTRDIFF_MAX / size);
#else
            fprintf(stderr,
                    "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n",
                    count, PTRDIFF_MAX / size);
#endif
        } else {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n",
                    size * count, available - padding);
#else
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n",
                    size * count, available - padding);
#endif
        }
        fprintf(stderr, "[KLS] Failed kls_push_zero() call.\n");
        kls_free(kls);
        exit(EXIT_FAILURE);
        //return 0;
    }
    char *p = kls->data + kls->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    kls->prev_offset = kls->offset;
    kls->offset += padding + size * count;
    KLS_Region *reg = NULL;
    if (t_kls->conf.kls_autoset_regions == 1) {
        switch (t_kls->conf.tkls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            reg = (KLS_Region *) malloc(sizeof(KLS_Region));
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            if (kls_length(t_kls->t_regs) <
                t_kls->max_regions_kls_alloc_basic) {
                reg = KLS_PUSH(t_kls->reglist_kls, KLS_Region, 1);
            } else {
                fprintf(stderr,
                        "[ERROR]    [%s()]:  Exceeding t_kls->max_regions_kls_alloc_basic: {%i}.\n",
                        __func__, t_kls->max_regions_kls_alloc_basic);
                if (kls->conf.kls_verbose_lvl > 0) {
                    kls_log(kls, "ERROR",
                            "[%s()]:  Exceeding t_kls->max_regions_kls_alloc_basic: {%i}.",
                            __func__, t_kls->max_regions_kls_alloc_basic);
                    kls_showList_toFile(t_kls->t_regs,
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
                    __func__, t_kls->conf.tkls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "%s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                    __func__, t_kls->conf.tkls_reglist_alloc_backend);
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
        strncpy(reg->name, KOLISEO_DEFAULT_REGION_NAME,
                KLS_REGION_MAX_NAME_SIZE);
        reg->name[KLS_REGION_MAX_NAME_SIZE - 1] = '\0';
        strncpy(reg->desc, KOLISEO_DEFAULT_REGION_DESC,
                KLS_REGION_MAX_DESC_SIZE);
        reg->desc[KLS_REGION_MAX_DESC_SIZE - 1] = '\0';
        //KLS_Region_List reglist = kls_emptyList();
        //reglist = kls_cons(kls,reg,reglist);
        //t_kls->t_regs = kls_append(kls,reglist, t_kls->t_regs);
        t_kls->t_regs = kls_t_cons(t_kls, reg, t_kls->t_regs);
    }

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
        kls->stats.avg_region_size = kls_avg_regionSize(kls);
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

/**
 * Takes a Koliseo pointer, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the Koliseo data field, or goes to exit() if the operation fails.
 * Uses the passed name and desc fields to initialise the allocated KLS_Region fields.
 * Notably, it zeroes the memory region.
 * @param kls The Koliseo at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @return A void pointer to the start of memory just pushed to the Koliseo.
 */
void *kls_push_zero_named(Koliseo *kls, ptrdiff_t size, ptrdiff_t align,
                          ptrdiff_t count, char *name, char *desc)
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
    ptrdiff_t available = kls->size - kls->offset;
    ptrdiff_t padding = -kls->offset & (align - 1);
    if (count > PTRDIFF_MAX / size || (available - padding) < (size * count)) {
        if (count > PTRDIFF_MAX / size) {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n",
                    count, PTRDIFF_MAX / size);
#else
            fprintf(stderr,
                    "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n",
                    count, PTRDIFF_MAX / size);
#endif
        } else {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n",
                    size * count, available - padding);
#else
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n",
                    size * count, available - padding);
#endif
        }
        fprintf(stderr, "[KLS] Failed kls_push_zero() call.\n");
        kls_free(kls);
        exit(EXIT_FAILURE);
        //return 0;
    }
    char *p = kls->data + kls->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    kls->prev_offset = kls->offset;
    kls->offset += padding + size * count;
    if (kls->conf.kls_autoset_regions == 1) {
        KLS_Region *reg = NULL;
        switch (kls->conf.kls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            reg = (KLS_Region *) malloc(sizeof(KLS_Region));
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            if (kls_length(kls->regs) < kls->max_regions_kls_alloc_basic) {
                reg = KLS_PUSH(kls->reglist_kls, KLS_Region, 1);
            } else {
                fprintf(stderr,
                        "[ERROR]    [%s()]:  Exceeding kls->max_regions_kls_alloc_basic: {%i}.\n",
                        __func__, kls->max_regions_kls_alloc_basic);
                if (kls->conf.kls_verbose_lvl > 0) {
                    kls_log(kls, "ERROR",
                            "[%s()]:  Exceeding kls->max_regions_kls_alloc_basic: {%i}.",
                            __func__, kls->max_regions_kls_alloc_basic);
                    kls_showList_toFile(kls->regs, kls->conf.kls_log_fp);
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

        reg->begin_offset = kls->prev_offset;
        reg->end_offset = kls->offset;
        reg->size = reg->end_offset - reg->begin_offset;
        reg->padding = padding;
        reg->type = KLS_None;
        strncpy(reg->name, name, KLS_REGION_MAX_NAME_SIZE);
        reg->name[KLS_REGION_MAX_NAME_SIZE - 1] = '\0';
        strncpy(reg->desc, desc, KLS_REGION_MAX_DESC_SIZE);
        reg->desc[KLS_REGION_MAX_DESC_SIZE - 1] = '\0';
        //KLS_Region_List reglist = kls_emptyList();
        //reglist = kls_cons(kls,reg,reglist);
        //kls->regs = kls_append(kls,reglist, kls->regs);
        kls->regs = kls_cons(kls, reg, kls->regs);

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
    }
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
 * @return A void pointer to the start of memory just pushed to the Koliseo.
 */
void *kls_temp_push_zero_named(Koliseo_Temp *t_kls, ptrdiff_t size,
                               ptrdiff_t align, ptrdiff_t count, char *name,
                               char *desc)
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

    ptrdiff_t available = kls->size - kls->offset;
    ptrdiff_t padding = -kls->offset & (align - 1);
    if (count > PTRDIFF_MAX / size || (available - padding) < (size * count)) {
        if (count > PTRDIFF_MAX / size) {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n",
                    count, PTRDIFF_MAX / size);
#else
            fprintf(stderr,
                    "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n",
                    count, PTRDIFF_MAX / size);
#endif
        } else {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n",
                    size * count, available - padding);
#else
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n",
                    size * count, available - padding);
#endif
        }
        fprintf(stderr, "[KLS] Failed %s() call.\n", __func__);
        kls_free(kls);
        exit(EXIT_FAILURE);
        //return 0;
    }
    char *p = kls->data + kls->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    kls->prev_offset = kls->offset;
    kls->offset += padding + size * count;
    if (t_kls->conf.kls_autoset_regions == 1) {
        KLS_Region *reg = NULL;
        switch (t_kls->conf.tkls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            reg = (KLS_Region *) malloc(sizeof(KLS_Region));
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            if (kls_length(t_kls->t_regs) <
                t_kls->max_regions_kls_alloc_basic) {
                reg = KLS_PUSH(t_kls->reglist_kls, KLS_Region, 1);
            } else {
                fprintf(stderr,
                        "[ERROR]    [%s()]:  Exceeding t_kls->max_regions_kls_alloc_basic: {%i}.\n",
                        __func__, t_kls->max_regions_kls_alloc_basic);
                if (kls->conf.kls_verbose_lvl > 0) {
                    kls_log(kls, "ERROR",
                            "[%s()]:  Exceeding t_kls->max_regions_kls_alloc_basic: {%i}.",
                            __func__, t_kls->max_regions_kls_alloc_basic);
                    kls_showList_toFile(t_kls->t_regs,
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
                    __func__, t_kls->conf.tkls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "%s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                    __func__, t_kls->conf.tkls_reglist_alloc_backend);
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
        strncpy(reg->name, name, KLS_REGION_MAX_NAME_SIZE);
        reg->name[KLS_REGION_MAX_NAME_SIZE - 1] = '\0';
        strncpy(reg->desc, desc, KLS_REGION_MAX_DESC_SIZE);
        reg->desc[KLS_REGION_MAX_DESC_SIZE - 1] = '\0';
        //KLS_Region_List reglist = kls_emptyList();
        //reglist = kls_cons(kls,reg,reglist);
        //t_kls->t_regs = kls_append(kls,reglist, t_kls->t_regs);
        t_kls->t_regs = kls_t_cons(t_kls, reg, t_kls->t_regs);

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
    }
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
 * @return A void pointer to the start of memory just pushed to the referred Koliseo.
 */
void *kls_push_zero_typed(Koliseo *kls, ptrdiff_t size, ptrdiff_t align,
                          ptrdiff_t count, int type, char *name, char *desc)
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
    ptrdiff_t available = kls->size - kls->offset;
    ptrdiff_t padding = -kls->offset & (align - 1);
    if (count > PTRDIFF_MAX / size || (available - padding) < (size * count)) {
        if (count > PTRDIFF_MAX / size) {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n",
                    count, PTRDIFF_MAX / size);
#else
            fprintf(stderr,
                    "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n",
                    count, PTRDIFF_MAX / size);
#endif
        } else {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n",
                    size * count, available - padding);
#else
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n",
                    size * count, available - padding);
#endif
        }
        fprintf(stderr, "[KLS] Failed kls_push_zero() call.\n");
        kls_free(kls);
        exit(EXIT_FAILURE);
        //return 0;
    }
    char *p = kls->data + kls->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    kls->prev_offset = kls->offset;
    kls->offset += padding + size * count;
    if (kls->conf.kls_autoset_regions == 1) {
        KLS_Region *reg = NULL;
        switch (kls->conf.kls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            reg = (KLS_Region *) malloc(sizeof(KLS_Region));
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            if (kls_length(kls->regs) < kls->max_regions_kls_alloc_basic) {
                reg = KLS_PUSH(kls->reglist_kls, KLS_Region, 1);
            } else {
                fprintf(stderr,
                        "[ERROR]    [%s()]:  Exceeding kls->max_regions_kls_alloc_basic: {%i}.\n",
                        __func__, kls->max_regions_kls_alloc_basic);
                if (kls->conf.kls_verbose_lvl > 0) {
                    kls_log(kls, "ERROR",
                            "[%s()]:  Exceeding kls->max_regions_kls_alloc_basic: {%i}.",
                            __func__, kls->max_regions_kls_alloc_basic);
                    kls_showList_toFile(kls->regs, kls->conf.kls_log_fp);
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

        reg->begin_offset = kls->prev_offset;
        reg->end_offset = kls->offset;
        reg->size = reg->end_offset - reg->begin_offset;
        reg->padding = padding;
        reg->type = type;
        strncpy(reg->name, name, KLS_REGION_MAX_NAME_SIZE);
        reg->name[KLS_REGION_MAX_NAME_SIZE - 1] = '\0';
        strncpy(reg->desc, desc, KLS_REGION_MAX_DESC_SIZE);
        reg->desc[KLS_REGION_MAX_DESC_SIZE - 1] = '\0';
        //KLS_Region_List reglist = kls_emptyList();
        //reglist = kls_cons(kls,reg,reglist);
        //kls->regs = kls_append(kls,reglist, kls->regs);
        kls->regs = kls_cons(kls, reg, kls->regs);

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
    }
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
 * @return A void pointer to the start of memory just pushed to the referred Koliseo.
 */
void *kls_temp_push_zero_typed(Koliseo_Temp *t_kls, ptrdiff_t size,
                               ptrdiff_t align, ptrdiff_t count, int type,
                               char *name, char *desc)
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
    ptrdiff_t available = kls->size - kls->offset;
    ptrdiff_t padding = -kls->offset & (align - 1);
    if (count > PTRDIFF_MAX / size || (available - padding) < (size * count)) {
        if (count > PTRDIFF_MAX / size) {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n",
                    count, PTRDIFF_MAX / size);
#else
            fprintf(stderr,
                    "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n",
                    count, PTRDIFF_MAX / size);
#endif
        } else {
#ifndef MINGW32_BUILD
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n",
                    size * count, available - padding);
#else
            fprintf(stderr,
                    "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n",
                    size * count, available - padding);
#endif
        }
        fprintf(stderr, "[KLS] Failed kls_push_zero() call.\n");
        kls_free(kls);
        exit(EXIT_FAILURE);
        //return 0;
    }
    char *p = kls->data + kls->offset + padding;
    //Zero new area
    memset(p, 0, size * count);
    kls->prev_offset = kls->offset;
    kls->offset += padding + size * count;
    if (t_kls->conf.kls_autoset_regions == 1) {
        KLS_Region *reg = NULL;
        switch (t_kls->conf.tkls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            reg = (KLS_Region *) malloc(sizeof(KLS_Region));
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            if (kls_length(t_kls->t_regs) <
                t_kls->max_regions_kls_alloc_basic) {
                reg = KLS_PUSH(t_kls->reglist_kls, KLS_Region, 1);
            } else {
                fprintf(stderr,
                        "[ERROR]    [%s()]:  Exceeding t_kls->max_regions_kls_alloc_basic: {%i}.\n",
                        __func__, t_kls->max_regions_kls_alloc_basic);
                if (kls->conf.kls_verbose_lvl > 0) {
                    kls_log(kls, "ERROR",
                            "[%s()]:  Exceeding t_kls->max_regions_kls_alloc_basic: {%i}.",
                            __func__, t_kls->max_regions_kls_alloc_basic);
                    kls_showList_toFile(t_kls->t_regs,
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
                    __func__, t_kls->conf.tkls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "%s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                    __func__, t_kls->conf.tkls_reglist_alloc_backend);
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
        reg->type = type;
        strncpy(reg->name, name, KLS_REGION_MAX_NAME_SIZE);
        reg->name[KLS_REGION_MAX_NAME_SIZE - 1] = '\0';
        strncpy(reg->desc, desc, KLS_REGION_MAX_DESC_SIZE);
        reg->desc[KLS_REGION_MAX_DESC_SIZE - 1] = '\0';
        //KLS_Region_List reglist = kls_emptyList();
        //reglist = kls_cons(kls,reg,reglist);
        //t_kls->t_regs = kls_append(kls,reglist, t_kls->t_regs);
        t_kls->t_regs = kls_t_cons(t_kls, reg, t_kls->t_regs);

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
    }
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

/**
 * Prints header fields from the passed Koliseo pointer, to the passed FILE pointer.
 * @param kls The Koliseo at hand.
 */
void print_kls_2file(FILE *fp, Koliseo *kls)
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
#ifndef MINGW32_BUILD
        fprintf(fp, "\n[KLS] Size: { %li }\n", kls->size);
#else
        fprintf(fp, "\n[KLS] Size: { %lli }\n", kls->size);
#endif
        char human_size[200];
        char curr_size[200];
        kls_formatSize(kls->size, human_size, sizeof(human_size));
        fprintf(fp, "[KLS] Size (Human): { %s }\n", human_size);
        kls_formatSize(kls->offset, curr_size, sizeof(curr_size));
        fprintf(fp, "[KLS] Used (Human): { %s }\n", curr_size);
#ifndef MINGW32_BUILD
        fprintf(fp, "[KLS] Offset: { %li }\n", kls->offset);
        fprintf(fp, "[KLS] Prev_Offset: { %li }\n", kls->prev_offset);
#else
        fprintf(fp, "[KLS] Offset: { %lli }\n", kls->offset);
        fprintf(fp, "[KLS] Prev_Offset: { %lli }\n", kls->prev_offset);
#endif
        if (kls->conf.kls_reglist_alloc_backend == KLS_REGLIST_ALLOC_KLS_BASIC) {
            fprintf(fp, "[KLS] Max Regions: { %i }\n\n",
                    kls->max_regions_kls_alloc_basic);
        } else {
            fprintf(fp, "\n");
        }
    }
}

/**
 * Prints header fields from the passed Koliseo pointer, to stderr.
 * @param kls The Koliseo at hand.
 */
void print_dbg_kls(Koliseo *kls)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR] [%s()]: Passed Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    print_kls_2file(stderr, kls);
}

/**
 * Prints header fields from the passed Koliseo_Temp pointer, to the passed FILE pointer.
 * @param t_kls The Koliseo_Temp at hand.
 */
void print_temp_kls_2file(FILE *fp, Koliseo_Temp *t_kls)
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
        Koliseo *kls = t_kls->kls;
        fprintf(fp, "\n[KLS_T] API Level: { %i }\n", int_koliseo_version());
#ifndef MINGW32_BUILD
        fprintf(fp, "\n[KLS_T] Temp Size: { %li }\n",
                kls->size - t_kls->offset);
        fprintf(fp, "\n[KLS_T] Refer Size: { %li }\n", kls->size);
#else
        fprintf(fp, "\n[KLS_T] Temp Size: { %lli }\n",
                kls->size - t_kls->offset);
        fprintf(fp, "\n[KLS_T] Refer Size: { %lli }\n", kls->size);
#endif
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
#ifndef MINGW32_BUILD
        fprintf(fp, "[KLS_T] Inner Offset: { %li }\n", kls->offset);
        fprintf(fp, "[KLS_T] Temp Offset: { %li }\n", t_kls->offset);
#else
        fprintf(fp, "[KLS_T] Inner Offset: { %lli }\n", kls->offset);
        fprintf(fp, "[KLS_T] Temp Offset: { %lli }\n", t_kls->offset);
#endif
#ifndef MINGW32_BUILD
        fprintf(fp, "[KLS_T] Inner Prev_Offset: { %li }\n", kls->prev_offset);
        fprintf(fp, "[KLS_T] Temp Prev_Offset: { %li }\n\n",
                t_kls->prev_offset);
#else
        fprintf(fp, "[KLS_T] Inner Prev_Offset: { %lli }\n", kls->prev_offset);
        fprintf(fp, "[KLS_T] Temp Prev_Offset: { %lli }\n\n",
                t_kls->prev_offset);
#endif
    }
}

/**
 * Prints header fields from the passed Koliseo_Temp pointer, to stderr.
 * @param t_kls The Koliseo_Temp at hand.
 */
void print_dbg_temp_kls(Koliseo_Temp *t_kls)
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

#ifdef KOLISEO_HAS_CURSES
/**
 * Prints fields and eventually KLS_Region_List from the passed Koliseo pointer, to the passed WINDOW pointer.
 * @param kls The Koliseo at hand.
 * @param win The Window at hand.
 */
void kls_show_toWin(Koliseo *kls, WINDOW *win)
{
    if (kls == NULL) {
        fprintf(stderr, "kls_show_toWin(): passed Koliseo was null.");
        exit(EXIT_FAILURE);
    }
    if (win == NULL) {
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "ERROR", "kls_show_toWin():  passed WINDOW was null.");
#else
        fprintf(stderr, "kls_show_toWin(): passed WINDOW was null.");
#endif
        exit(EXIT_FAILURE);
    }
    wclear(win);
    box(win, 0, 0);
    wrefresh(win);
    int y = 2;
    int x = 2;
    mvwprintw(win, y++, x, "Koliseo data:");
    mvwprintw(win, y++, x, "API Level: { %i }", int_koliseo_version());
#ifndef MINGW32_BUILD
    mvwprintw(win, y++, x, "Size: { %li }", kls->size);
#else
    mvwprintw(win, y++, x, "Size: { %lli }", kls->size);
#endif
    char h_size[200];
    kls_formatSize(kls->size, h_size, sizeof(h_size));
    mvwprintw(win, y++, x, "Human size: { %s }", h_size);
    char curr_size[200];
    kls_formatSize(kls->offset, curr_size, sizeof(curr_size));
    mvwprintw(win, y++, x, "Used (Human): { %s }", curr_size);
#ifndef MINGW32_BUILD
    mvwprintw(win, y++, x, "Offset: { %li }", kls->offset);
#else
    mvwprintw(win, y++, x, "Offset: { %lli }", kls->offset);
#endif
#ifndef MINGW32_BUILD
    mvwprintw(win, y++, x, "Prev_Offset: { %li }", kls->prev_offset);
#else
    mvwprintw(win, y++, x, "Prev_Offset: { %lli }", kls->prev_offset);
#endif
    mvwprintw(win, y++, x, "KLS_Region_List len: { %i }",
              kls_length(kls->regs));
    mvwprintw(win, y++, x, "Current usage: { %.3f%% }",
              (kls->offset * 100.0) / kls->size);
    mvwprintw(win, y++, x, "%s", "");
    mvwprintw(win, y++, x, "q or Enter to quit.");
    /*
       KLS_Region_List rl = kls_copy(kls->regs);
       while (!kls_empty(rl)) {
       mvwprintw(win, y, x, "Prev_Offset: [%i]",kls->prev_offset);
       }
     */
    wrefresh(win);
    int ch = '?';
    int quit = -1;
    do {
        quit = 0;
        ch = wgetch(win);
        switch (ch) {
        case 10:
        case 'q': {
            quit = 1;
        }
        break;
        default: {
            quit = 0;
        }
        break;
        }
    } while (!quit);
}

/**
 * Takes a Koliseo_Temp pointer and prints fields and eventually KLS_Region_List from the referred Koliseo pointer, to the passed WINDOW pointer.
 * @param t_kls The Koliseo_Temp at hand.
 * @param win The Window at hand.
 */
void kls_temp_show_toWin(Koliseo_Temp *t_kls, WINDOW *win)
{
    if (t_kls == NULL) {
        fprintf(stderr, "kls_temp_show_toWin(): passed Koliseo_Temp was null.");
        exit(EXIT_FAILURE);
    }
    Koliseo *kls = t_kls->kls;
    if (kls == NULL) {
        fprintf(stderr, "kls_temp_show_toWin(): referred Koliseo was null.");
        exit(EXIT_FAILURE);
    }
    if (win == NULL) {
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "ERROR",
                "kls_temp_show_toWin():  passed WINDOW was null.");
#else
        fprintf(stderr, "kls_temp_show_toWin(): passed WINDOW was null.");
#endif
        exit(EXIT_FAILURE);
    }
    wclear(win);
    box(win, 0, 0);
    wrefresh(win);
    int y = 2;
    int x = 2;
    mvwprintw(win, y++, x, "Koliseo_Temp data:");
    mvwprintw(win, y++, x, "API Level: { %i }", int_koliseo_version());
#ifndef MINGW32_BUILD
    mvwprintw(win, y++, x, "Temp Size: { %li }", kls->size - t_kls->offset);
    mvwprintw(win, y++, x, "Refer Size: { %li }", kls->size);
#else
    mvwprintw(win, y++, x, "Temp Size: { %lli }", kls->size - t_kls->offset);
    mvwprintw(win, y++, x, "Refer Size: { %lli }", kls->size);
#endif
    char h_size[200];
    char curr_size[200];
    kls_formatSize(kls->size - t_kls->offset, h_size, sizeof(h_size));
    mvwprintw(win, y++, x, "Temp Human size: { %s }", h_size);
    kls_formatSize(kls->size, h_size, sizeof(h_size));
    mvwprintw(win, y++, x, "Inner Human size: { %s }", h_size);
    kls_formatSize(kls->offset, curr_size, sizeof(curr_size));
    mvwprintw(win, y++, x, "Inner Used (Human): { %s }", curr_size);
    kls_formatSize(t_kls->offset, curr_size, sizeof(curr_size));
    mvwprintw(win, y++, x, "Temp Used (Human): { %s }", curr_size);
#ifndef MINGW32_BUILD
    mvwprintw(win, y++, x, "Inner Offset: { %li }", kls->offset);
    mvwprintw(win, y++, x, "Temp Offset: { %li }", t_kls->offset);
#else
    mvwprintw(win, y++, x, "Inner Offset: { %lli }", kls->offset);
    mvwprintw(win, y++, x, "Temp Offset: { %lli }", t_kls->offset);
#endif
#ifndef MINGW32_BUILD
    mvwprintw(win, y++, x, "Inner Prev_Offset: { %li }", kls->prev_offset);
    mvwprintw(win, y++, x, "Temp Prev_Offset: { %li }", t_kls->prev_offset);
#else
    mvwprintw(win, y++, x, "Inner Prev_Offset: { %lli }", kls->prev_offset);
    mvwprintw(win, y++, x, "Temp Prev_Offset: { %lli }", t_kls->prev_offset);
#endif
    mvwprintw(win, y++, x, "Refer KLS_Region_List len: { %i }",
              kls_length(kls->regs));
    mvwprintw(win, y++, x, "Temp KLS_Region_List len: { %i }",
              kls_length(t_kls->t_regs));
    mvwprintw(win, y++, x, "Current inner usage: { %.3f%% }",
              (kls->offset * 100.0) / kls->size);
    mvwprintw(win, y++, x, "Current refer usage: { %.3f%% }",
              (t_kls->offset * 100.0) / kls->size);
    mvwprintw(win, y++, x, "%s", "");
    mvwprintw(win, y++, x, "q or Enter to quit.");
    /*
       KLS_Region_List rl = kls_copy(kls->regs);
       while (!kls_empty(rl)) {
       mvwprintw(win, y, x, "Prev_Offset: [%i]",kls->prev_offset);
       }
     */
    wrefresh(win);
    int ch = '?';
    int quit = -1;
    do {
        quit = 0;
        ch = wgetch(win);
        switch (ch) {
        case 10:
        case 'q': {
            quit = 1;
        }
        break;
        default: {
            quit = 0;
        }
        break;
        }
    } while (!quit);
}

/**
 * Displays a slideshow of KLS_Region_List from passed Koliseo, to the passed WINDOW pointer.
 * @param kls The Koliseo at hand.
 * @param win The Window at hand.
 */
void kls_showList_toWin(Koliseo *kls, WINDOW *win)
{
    if (kls == NULL) {
        fprintf(stderr, "kls_showList_toWin(): passed Koliseo was null.");
        exit(EXIT_FAILURE);
    }
    if (win == NULL) {
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "ERROR", "kls_showList_toWin():  passed WINDOW was null.");
#else
        fprintf(stderr, "kls_showList_toWin(): passed WINDOW was null.");
#endif
        exit(EXIT_FAILURE);
    }
    wclear(win);
    box(win, 0, 0);
    wrefresh(win);
    int y = 2;
    int x = 2;
    int quit = 0;
    mvwprintw(win, y++, x, "KLS_Region_List data:");
    KLS_Region_List rl = kls->regs;
    while (!quit && !kls_empty(rl)) {
        wclear(win);
        y = 3;
        KLS_list_element e = kls_head(rl);
        mvwprintw(win, y++, x, "Name: { %s }", e->name);
        mvwprintw(win, y++, x, "Desc: { %s }", e->desc);
#ifndef MINGW32_BUILD
        mvwprintw(win, y++, x, "Offsets: { %li } -> { %li }", e->begin_offset,
                  e->end_offset);
        mvwprintw(win, y++, x, "Size: { %li }", e->size);
        mvwprintw(win, y++, x, "Padding: { %li }", e->padding);
#else
        mvwprintw(win, y++, x, "Offsets: { %lli } -> { %lli }", e->begin_offset,
                  e->end_offset);
        mvwprintw(win, y++, x, "Size: { %lli }", e->size);
        mvwprintw(win, y++, x, "Padding: { %lli }", e->padding);
#endif
        mvwprintw(win, y++, x, "KLS_Region_List len: { %i }",
                  kls_length(kls->regs));
        mvwprintw(win, y++, x, "Current usage: { %.3f%% }",
                  kls_usageShare(e, kls));
        char h_size[200];
        ptrdiff_t reg_size = e->end_offset - e->begin_offset;
        kls_formatSize(reg_size, h_size, sizeof(h_size));
        mvwprintw(win, y++, x, "Human size: { %s }", h_size);
        mvwprintw(win, y++, x, "%s", "");
        mvwprintw(win, y++, x, "q to quit, Right arrow to go forward.");
        /*
           KLS_Region_List rl = kls_copy(kls->regs);
           while (!kls_empty(rl)) {
           mvwprintw(win, y, x, "Prev_Offset: [%i]",kls->prev_offset);
           }
         */
        box(win, 0, 0);
        wrefresh(win);
        int ch = '?';
        int picked = -1;
        do {
            picked = 0;
            ch = wgetch(win);
            switch (ch) {
            case KEY_RIGHT: {
                rl = kls_tail(rl);
                picked = 1;
            }
            break;
            case 'q': {
                quit = 1;
                picked = 1;
            }
            break;
            default: {
                picked = 0;
            }
            break;
            }
        } while (!quit && !picked);
    }
}

/**
 * Displays a slideshow of KLS_Region_List from passed Koliseo_Temp, to the passed WINDOW pointer.
 * @param t_kls The Koliseo_Temp at hand.
 * @param win The Window at hand.
 */
void kls_temp_showList_toWin(Koliseo_Temp *t_kls, WINDOW *win)
{
    if (t_kls == NULL) {
        fprintf(stderr,
                "kls_temp_showList_toWin(): passed Koliseo_Temp was null.\n");
        exit(EXIT_FAILURE);
    }
    Koliseo *kls_ref = t_kls->kls;
    if (kls_ref == NULL) {
        fprintf(stderr,
                "kls_temp_showList_toWin(): referred Koliseo was null.\n");
        exit(EXIT_FAILURE);
    }
    if (win == NULL) {
#ifdef KLS_DEBUG_CORE
        kls_log(kls_ref, "ERROR",
                "kls_temp_showList_toWin():  passed WINDOW was null.");
#else
        fprintf(stderr, "kls_temp_showList_toWin(): passed WINDOW was null.\n");
#endif
        exit(EXIT_FAILURE);
    }
    wclear(win);
    box(win, 0, 0);
    wrefresh(win);
    int y = 2;
    int x = 2;
    int quit = 0;
    mvwprintw(win, y++, x, "KLS_Region_List data:");
    KLS_Region_List rl = t_kls->t_regs;
    while (!quit && !kls_empty(rl)) {
        wclear(win);
        y = 3;
        KLS_list_element e = kls_head(rl);
        mvwprintw(win, y++, x, "Name: { %s }", e->name);
        mvwprintw(win, y++, x, "Desc: { %s }", e->desc);
#ifndef MINGW32_BUILD
        mvwprintw(win, y++, x, "Offsets: { %li } -> { %li }", e->begin_offset,
                  e->end_offset);
        mvwprintw(win, y++, x, "Size: { %li }", e->size);
        mvwprintw(win, y++, x, "Padding: { %li }", e->padding);
#else
        mvwprintw(win, y++, x, "Offsets: { %lli } -> { %lli }", e->begin_offset,
                  e->end_offset);
        mvwprintw(win, y++, x, "Size: { %lli }", e->size);
        mvwprintw(win, y++, x, "Padding: { %lli }", e->padding);
#endif
        mvwprintw(win, y++, x, "KLS_Region_List len: { %i }",
                  kls_length(t_kls->t_regs));
        //mvwprintw(win, y++, x, "Current usage: { %.3f%% }", kls_usageShare(e,kls));
        char h_size[200];
        ptrdiff_t reg_size = e->end_offset - e->begin_offset;
        kls_formatSize(reg_size, h_size, sizeof(h_size));
        mvwprintw(win, y++, x, "Human size: { %s }", h_size);
        mvwprintw(win, y++, x, "%s", "");
        mvwprintw(win, y++, x, "q to quit, Right arrow to go forward.");
        /*
           KLS_Region_List rl = kls_copy(kls->regs);
           while (!kls_empty(rl)) {
           mvwprintw(win, y, x, "Prev_Offset: [%i]",kls->prev_offset);
           }
         */
        box(win, 0, 0);
        wrefresh(win);
        int ch = '?';
        int picked = -1;
        do {
            picked = 0;
            ch = wgetch(win);
            switch (ch) {
            case KEY_RIGHT: {
                rl = kls_tail(rl);
                picked = 1;
            }
            break;
            case 'q': {
                quit = 1;
                picked = 1;
            }
            break;
            default: {
                picked = 0;
            }
            break;
            }
        } while (!quit && !picked);
    }
}
#endif //KOLISEO_HAS_CURSES

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
    if (kls->conf.kls_reglist_alloc_backend == KLS_REGLIST_ALLOC_KLS_BASIC) {
        kls_free(kls->reglist_kls);
        //free(kls->reglist_kls);
    } else {
        kls_freeList(kls->regs);
    }
    free(kls);
}

/**
 * Starts a new savestate for the passed Koliseo pointer, by initialising its Koliseo_Temp pointer and returning it.
 * Notably, you should not use the original while using the copy.
 * @param kls The Koliseo at hand.
 * @return A Koliseo_Temp struct.
 * @see Koliseo_Temp
 */
Koliseo_Temp *kls_temp_start(Koliseo *kls)
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

    Koliseo_Temp *tmp = KLS_PUSH(kls, Koliseo_Temp, 1);
    tmp->kls = kls;
    tmp->prev_offset = prev;
    tmp->offset = off;
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "INFO", "Passed kls conf: " KLS_Conf_Fmt "\n",
            KLS_Conf_Arg(kls->conf));
#endif
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
    kls->has_temp = 1;
    kls->t_kls = tmp;
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
            temp_kls_header = KLS_PUSH(tmp->reglist_kls, KLS_Region, 1);
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
        temp_kls_header->name[KLS_REGION_MAX_NAME_SIZE - 1] = '\0';
        strncpy(temp_kls_header->desc, "Last Reg b4 KLS_T",
                KLS_REGION_MAX_DESC_SIZE);
        temp_kls_header->desc[KLS_REGION_MAX_DESC_SIZE - 1] = '\0';
        KLS_Region_List reglist = kls_emptyList();
        reglist = kls_t_cons(tmp, temp_kls_header, reglist);
        tmp->t_regs = reglist;
        if (tmp->t_regs == NULL) {
            fprintf(stderr, "[KLS] [%s()]: failed to get a KLS_Region_List.\n",
                    __func__);
            exit(EXIT_FAILURE);
        }
    } else {
        tmp->t_regs = NULL;
    }
#ifdef KLS_DEBUG_CORE
    kls_log(kls, "KLS", "Prepared new Temp KLS.");
#endif
    return tmp;
}

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

    if (tmp_kls->conf.kls_autoset_regions == 1) {
        switch (tmp_kls->conf.tkls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            kls_freeList(tmp_kls->t_regs);
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
    tmp_kls = NULL;
    if (kls_ref->conf.kls_collect_stats == 1) {
        kls_ref->stats.tot_temp_pushes = 0;
        kls_ref->stats.tot_temp_pops = 0;
    }
}

KLS_Region_List kls_emptyList(void)
{
    return NULL;
}

bool kls_empty(KLS_Region_List l)
{
    if (l == NULL) {
        return true;
    } else {
        return false;
    }
}

KLS_list_element kls_head(KLS_Region_List l)
{
    if (kls_empty(l)) {
        exit(EXIT_FAILURE);
    } else {
        return l->value;
    }
}

KLS_Region_List kls_tail(KLS_Region_List l)
{
    if (kls_empty(l)) {
        exit(EXIT_FAILURE);
    } else {
        return l->next;
    }
}

KLS_Region_List kls_cons(Koliseo *kls, KLS_list_element e, KLS_Region_List l)
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
        t = KLS_PUSH(kls->reglist_kls, KLS_region_list_item, 1);
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

KLS_Region_List kls_t_cons(Koliseo_Temp *t_kls, KLS_list_element e,
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
        t = KLS_PUSH(t_kls->reglist_kls, KLS_region_list_item, 1);
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

void kls_freeList(KLS_Region_List l)
{
    if (kls_empty(l)) {
        return;
    } else {
        kls_freeList(kls_tail(l));
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

void kls_showList_toFile(KLS_Region_List l, FILE *fp)
{
    if (fp == NULL) {
        fprintf(stderr,
                "[KLS]  kls_showList_toFile():  passed file was NULL.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "{");
    while (!kls_empty(l)) {
        fprintf(fp, "\n{ %s }, { %s }    ", kls_head(l)->name,
                kls_head(l)->desc);
#ifndef MINGW32_BUILD
        fprintf(fp, "{ %li } -> { %li }", kls_head(l)->begin_offset,
                kls_head(l)->end_offset);
#else
        fprintf(fp, "{ %lli } -> { %lli }", kls_head(l)->begin_offset,
                kls_head(l)->end_offset);
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

        l = kls_tail(l);
        if (!kls_empty(l)) {
            fprintf(fp, ",\n");
        }
    }
    fprintf(fp, "\n}\n");
}

void kls_showList(KLS_Region_List l)
{
    kls_showList_toFile(l, stdout);
}

bool kls_member(KLS_list_element el, KLS_Region_List l)
{
    if (kls_empty(l)) {
        return false;
    } else {
        if (el == kls_head(l)) {
            return true;
        } else {
            return kls_member(el, kls_tail(l));
        }
    }
}

int kls_length(KLS_Region_List l)
{
    if (kls_empty(l)) {
        return 0;
    } else {
        return 1 + kls_length(kls_tail(l));
    }
}

KLS_Region_List kls_append(Koliseo *kls, KLS_Region_List l1, KLS_Region_List l2)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_empty(l1)) {
        return l2;
    } else {
        return kls_cons(kls, kls_head(l1), kls_append(kls, kls_tail(l1), l2));
    }
}

KLS_Region_List kls_reverse(Koliseo *kls, KLS_Region_List l)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_empty(l)) {
        return kls_emptyList();
    } else {
        return kls_append(kls, kls_reverse(kls, kls_tail(l)),
                          kls_cons(kls, kls_head(l), kls_emptyList()));
    }
}

KLS_Region_List kls_copy(Koliseo *kls, KLS_Region_List l)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_empty(l)) {
        return l;
    } else {
        return kls_cons(kls, kls_head(l), kls_copy(kls, kls_tail(l)));
    }
}

KLS_Region_List kls_delete(Koliseo *kls, KLS_list_element el, KLS_Region_List l)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_empty(l)) {
        return kls_emptyList();
    } else {
        if (el == kls_head(l)) {
            return kls_tail(l);
        } else {
            return kls_cons(kls, kls_head(l), kls_delete(kls, el, kls_tail(l)));
        }
    }
}

KLS_Region_List kls_insord(Koliseo *kls, KLS_list_element el, KLS_Region_List l)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_empty(l)) {
        return kls_cons(kls, el, l);
    } else {
        //Insert KLS_list_element according to its begin_offset
        if (el->begin_offset <= kls_head(l)->begin_offset) {
            return kls_cons(kls, el, l);
        } else {
            return kls_cons(kls, kls_head(l), kls_insord(kls, el, kls_tail(l)));
        }
    }
}

KLS_Region_List kls_insord_p(Koliseo *kls, KLS_list_element el,
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
        paux = KLS_PUSH(kls->reglist_kls, KLS_region_list_item, 1);
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

KLS_Region_List kls_mergeList(Koliseo *kls, KLS_Region_List l1,
                              KLS_Region_List l2)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_empty(l1)) {
        return l2;
    } else {
        if (kls_empty(l2)) {
            return l1;
        } else {
            if (kls_isLess(kls_head(l1), kls_head(l2))) {
                return kls_cons(kls, kls_head(l1),
                                kls_mergeList(kls, kls_tail(l1), l2));
            } else {
                if (kls_isEqual(kls_head(l1), kls_head(l2))) {
                    return kls_cons(kls, kls_head(l1),
                                    kls_mergeList(kls, kls_tail(l1),
                                                  kls_tail(l2)));
                } else {
                    return kls_cons(kls, kls_head(l2),
                                    kls_mergeList(kls, l1, kls_tail(l2)));
                }
            }
        }
    }
}

KLS_Region_List kls_intersect(Koliseo *kls, KLS_Region_List l1,
                              KLS_Region_List l2)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_empty(l1) || kls_empty(l2)) {
        return kls_emptyList();
    }

    if (kls_member(kls_head(l1), l2) && !kls_member(kls_head(l1), kls_tail(l1))) {
        return kls_cons(kls, kls_head(l1),
                        kls_intersect(kls, kls_tail(l1), l2));
    }

    else {
        return kls_intersect(kls, kls_tail(l1), l2);
    }
}

KLS_Region_List kls_diff(Koliseo *kls, KLS_Region_List l1, KLS_Region_List l2)
{
    if (kls == NULL) {
        fprintf(stderr, "[ERROR]  [%s()]: Koliseo was NULL.\n", __func__);
        exit(EXIT_FAILURE);
    }
    if (kls_empty(l1) || kls_empty(l2)) {
        return l1;
    }

    else {
        if (!kls_member(kls_head(l1), l2)
            && !kls_member(kls_head(l1), kls_tail(l1))) {
            return kls_cons(kls, kls_head(l1), kls_diff(kls, kls_tail(l1), l2));
        } else {
            return kls_diff(kls, kls_tail(l1), l2);
        }
    }
}

/**
 * Compares two regions and returns true if the first one has a smaller size.
 * @param r1 The KLS_Region expected to be smaller
 * @param r2 The KLS_Region expected to be bigger
 * @return True if first region size is less than second region size.
 */
bool kls_isLess(KLS_Region *r1, KLS_Region *r2)
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
bool kls_isEqual(KLS_Region *r1, KLS_Region *r2)
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
 * @passed r The KLS_Region.
 * @return Region size as ptrdiff_t.
 */
ptrdiff_t kls_regionSize(KLS_Region *r)
{
    return r->end_offset - r->begin_offset;
}

/**
 * Return average region size in usage for the passed Koliseo.
 * @passed kls The Koliseo to check usage for.
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
    int tot_regs = kls_length(rl);
    if (tot_regs > 0) {
        int tot_size = 0;
        while (!kls_empty(rl)) {
            ptrdiff_t curr_size = 0;
            if (rl->value->size > 0) {
                curr_size = rl->value->size;
            } else {
                curr_size = kls_regionSize(rl->value);
                rl->value->size = curr_size;
            }
            tot_size += curr_size;
            rl = kls_tail(rl);
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
    while (!kls_empty(rl)) {
        fprintf(fp, "Usage for region (%i) [%s]:  [%.3f%%]\n", i,
                rl->value->name, kls_usageShare(rl->value, kls));
        rl = kls_tail(rl);
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

    while (!kls_empty(rl)) {
        KLS_list_element h = kls_head(rl);
        if (h->type == type) {
            res += (h->end_offset - h->begin_offset);
        }
        rl = kls_tail(rl);
    }

    return res;
}

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

Kstr kstr_new(const char* data, size_t len)
{
    return (Kstr) {
        .data = data,
        .len = len,
    };
}

Kstr kstr_from_c_lit(const char* c_lit)
{
    return kstr_new(c_lit, strlen(c_lit));
}

bool kstr_eq(Kstr left, Kstr right) {
    if (left.len != right.len) {
        return false;
    }

    for (size_t i=0; i < left.len; i++) {
        if (left.data[i] != right.data[i]) return false;
    }
    return true;
}

bool kstr_eq_ignorecase(Kstr left, Kstr right) {
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

Kstr kstr_trim_left(Kstr kstr)
{
    size_t i = 0;
    while ( i < kstr.len && isspace(kstr.data[i])) {
        i++;
    }
    return kstr_new(kstr.data + i, kstr.len - i);
}
Kstr kstr_trim_right(Kstr kstr)
{
    size_t i = 0;
    while ( i < kstr.len && isspace(kstr.data[kstr.len - i - 1])) {
        i++;
    }
    return kstr_new(kstr.data, kstr.len - i);
}
Kstr kstr_trim(Kstr kstr)
{
    return kstr_trim_left(kstr_trim_right(kstr));
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

        buffer = KLS_PUSH_NAMED(kls,char,length + 1,"char*","Buffer for file gulp");

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

        buffer = KLS_PUSH_NAMED(kls,char,length + 1,"char*","Buffer for file gulp");

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
    Kstr * res = KLS_PUSH_NAMED(kls,Kstr,1,"Kstr","Kstr for file gulp");
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
