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
#include "kls_region.h"
#include "koliseo.c"

const char* kls_reglist_backend_strings[KLS_REGLIST_TOTAL_BACKENDS] = {
    [KLS_REGLIST_ALLOC_LIBC] = "LIBC",
    [KLS_REGLIST_ALLOC_KLS_BASIC] = "KLS_BASIC",
    [KLS_REGLIST_ALLOC_KLS] = "KLS",
};

static const KLS_Conf KLS_DEFAULT_CONF__ = {
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
}; /**< Inner config used for any Koliseo used to host the regions for another Koliseo in the KLS_BASIC config.*/

const char* kls_reglist_backend_string(KLS_RegList_Alloc_Backend kls_be)
{
    switch(kls_be) {
    case KLS_REGLIST_ALLOC_LIBC:
    case KLS_REGLIST_ALLOC_KLS_BASIC:
    case KLS_REGLIST_ALLOC_KLS: {
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
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    switch (data_pt->conf.kls_reglist_alloc_backend) {
    case KLS_REGLIST_ALLOC_LIBC: {
        t = (KLS_Region_List) malloc(sizeof(KLS_region_list_item));
    }
    break;
    case KLS_REGLIST_ALLOC_KLS_BASIC:
    case KLS_REGLIST_ALLOC_KLS: {
        if (data_pt->reglist_kls == NULL) {
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
        t = KLS_PUSH(data_pt->reglist_kls, KLS_region_list_item);
    }
    break;
    default: {
        fprintf(stderr,
                "[ERROR]    %s():  Invalid conf.kls_reglist_alloc_backend value: {%i}.\n",
                __func__, data_pt->conf.kls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "ERROR",
                "%s():  Invalid conf.kls_reglist_alloc_backend value: {%i}.\n",
                __func__, data_pt->conf.kls_reglist_alloc_backend);
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
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    switch (data_pt->conf.kls_reglist_alloc_backend) {
    case KLS_REGLIST_ALLOC_LIBC: {
        l = data_pt->regs;
    }
    break;
    case KLS_REGLIST_ALLOC_KLS_BASIC:
    case KLS_REGLIST_ALLOC_KLS: {
        if (data_pt->reglist_kls == NULL) {
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
        l = data_pt->regs;
    }
    break;
    default: {
        fprintf(stderr,
                "[ERROR]    %s():  Invalid conf.kls_reglist_alloc_backend value: {%i}.\n",
                __func__, data_pt->conf.kls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "ERROR",
                "%s():  Invalid conf.kls_reglist_alloc_backend value: {%i}.\n",
                __func__, data_pt->conf.kls_reglist_alloc_backend);
#endif
        kls_free(kls);
        exit(EXIT_FAILURE);
    }
    break;
    }
    //TODO: is this enough to correctly pop the list?
    l = kls_rl_tail(l);
    KLS_region_list_item* popped_node = KLS_POP(data_pt->reglist_kls, KLS_region_list_item);
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
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) t_kls->kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    switch (data_pt->conf.tkls_reglist_alloc_backend) {
    case KLS_REGLIST_ALLOC_LIBC: {
        t = (KLS_Region_List) malloc(sizeof(KLS_region_list_item));
    }
    break;
    case KLS_REGLIST_ALLOC_KLS_BASIC:
    case KLS_REGLIST_ALLOC_KLS: {
        t = KLS_PUSH(data_pt->t_reglist_kls, KLS_region_list_item);
    }
    break;
    default: {
        fprintf(stderr,
                "[ERROR]    %s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                __func__, data_pt->conf.tkls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
        kls_log(t_kls->kls, "ERROR",
                "%s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                __func__, data_pt->conf.tkls_reglist_alloc_backend);
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
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    switch (data_pt->conf.kls_reglist_alloc_backend) {
    case KLS_REGLIST_ALLOC_LIBC: {
        paux = (KLS_Region_List) malloc(sizeof(KLS_region_list_item));
    }
    break;
    case KLS_REGLIST_ALLOC_KLS_BASIC:
    case KLS_REGLIST_ALLOC_KLS: {
        if (data_pt->reglist_kls == NULL) {
            fprintf(stderr,
                    "[ERROR]   at %s(): reglist_kls was NULL.\n",
                    __func__);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "at %s(): reglist_kls was NULL.\n", __func__);
#endif
            kls_free(kls);
            exit(EXIT_FAILURE);
        }
        paux = KLS_PUSH(data_pt->reglist_kls, KLS_region_list_item);
    }
    break;
    default: {
        fprintf(stderr,
                "[ERROR]    at %s(): Unexpected conf.kls_reglist_alloc_backend value: {%i}.\n",
                __func__, data_pt->conf.kls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "ERROR",
                "at %s(): Unexpected conf.kls_reglist_alloc_backend value: {%i}.\n",
                __func__, data_pt->conf.kls_reglist_alloc_backend);
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
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    KLS_Region_List rl = data_pt->regs;
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
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    KLS_Region_List rl = data_pt->regs;
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
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    KLS_Region_List rl = data_pt->regs;

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
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    KLS_Region_List rl = data_pt->regs;

    ptrdiff_t res = 0;

    while (!kls_rl_empty(rl)) {
        res += kls_rl_head(rl)->padding;
        rl = kls_rl_tail(rl);
    }
    return res;
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
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    if (data_pt->conf.kls_reglist_alloc_backend != KLS_REGLIST_ALLOC_KLS_BASIC) {
#ifdef KLS_DEBUG_CORE
        fprintf(stderr,
                "[ERROR]    %s(): conf.kls_reglist_backend was {%i}, expected KLS_REGLIST_ALLOC_KLS_BASIC: {%i}.\n",
                __func__, data_pt->conf.kls_reglist_alloc_backend,
                KLS_REGLIST_ALLOC_KLS_BASIC);
#endif
        return -1;
    }
    if (data_pt->reglist_kls == NULL) {
#ifdef KLS_DEBUG_CORE
        fprintf(stderr,
                "[ERROR]    %s(): passed reglist_kls was NULL.\n",
                __func__);
#endif
        return -1;
    }
    return (data_pt->reglist_kls->size - sizeof(Koliseo)) / (sizeof(KLS_Region) +
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
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) t_kls->kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    if (data_pt->conf.tkls_reglist_alloc_backend != KLS_REGLIST_ALLOC_KLS_BASIC) {
#ifdef KLS_DEBUG_CORE
        fprintf(stderr,
                "[ERROR]    %s(): conf.tkls_reglist_backend was {%i}, expected KLS_REGLIST_ALLOC_KLS_BASIC: {%i}.\n",
                __func__, data_pt->conf.tkls_reglist_alloc_backend,
                KLS_REGLIST_ALLOC_KLS_BASIC);
#endif
        return -1;
    }
    if (data_pt->reglist_kls == NULL) {
#ifdef KLS_DEBUG_CORE
        fprintf(stderr,
                "[ERROR]    %s(): passed reglist_kls was NULL.\n",
                __func__);
#endif
        return -1;
    }
    return (data_pt->reglist_kls->size - sizeof(Koliseo)) / (sizeof(KLS_Region) +
            sizeof
            (KLS_region_list_item));
}

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
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    if (data_pt->conf.kls_autoset_regions == 1) {
        KLS_Region *reg = NULL;
        switch (data_pt->conf.kls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            reg = (KLS_Region *) malloc(sizeof(KLS_Region));
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            if (kls_rl_length(data_pt->regs) < data_pt->max_regions_kls_alloc_basic) {
                reg = KLS_PUSH(data_pt->reglist_kls, KLS_Region);
            } else {
                fprintf(stderr,
                        "[ERROR]    [%s()]:  Exceeding max_regions_kls_alloc_basic: {%i}.\n",
                        caller, data_pt->max_regions_kls_alloc_basic);
                if (kls->conf.kls_verbose_lvl > 0) {
                    kls_log(kls, "ERROR",
                            "[%s()]:  Exceeding max_regions_kls_alloc_basic: {%i}.",
                            caller, data_pt->max_regions_kls_alloc_basic);
                    kls_rl_showList_toFile(data_pt->regs, kls->conf.kls_log_fp);
                    print_kls_2file(kls->conf.kls_log_fp, data_pt->reglist_kls);
                    print_kls_2file(kls->conf.kls_log_fp, kls);
                }
                kls_free(kls);
                exit(EXIT_FAILURE);
            }
        }
        break;
        case KLS_REGLIST_ALLOC_KLS: {
            reg = KLS_PUSH(data_pt->reglist_kls, KLS_Region);
        }
        break;
        default: {
            fprintf(stderr,
                    "[ERROR] [%s()]:  Unexpected KLS_RegList_Alloc_Backend value: {%i}.\n",
                    caller, data_pt->conf.kls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "%s():  Invalid KLS_RegList_Alloc_Backend value: {%i}.",
                    caller, data_pt->conf.kls_reglist_alloc_backend);
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
        data_pt->regs = kls_rl_cons(kls, reg, data_pt->regs);
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
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    if (data_pt->conf.kls_autoset_regions == 1) {
        switch (data_pt->conf.tkls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            reg = (KLS_Region *) malloc(sizeof(KLS_Region));
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            if (kls_rl_length(data_pt->t_regs) <
                data_pt->max_regions_kls_alloc_basic) {
                reg = KLS_PUSH(data_pt->t_reglist_kls, KLS_Region);
            } else {
                fprintf(stderr,
                        "[ERROR]    [%s()]:  Exceeding max_regions_kls_alloc_basic: {%i}.\n",
                        caller, data_pt->max_regions_kls_alloc_basic);
                if (kls->conf.kls_verbose_lvl > 0) {
                    kls_log(kls, "ERROR",
                            "[%s()]:  Exceeding max_regions_kls_alloc_basic: {%i}.",
                            caller, data_pt->max_regions_kls_alloc_basic);
                    kls_rl_showList_toFile(data_pt->t_regs,
                                           kls->conf.kls_log_fp);
                    print_kls_2file(kls->conf.kls_log_fp,
                                    data_pt->t_reglist_kls);
                    print_kls_2file(kls->conf.kls_log_fp, kls);
                }
                kls_free(kls);
                exit(EXIT_FAILURE);
            }
        }
        break;
        case KLS_REGLIST_ALLOC_KLS: {
            reg = KLS_PUSH(data_pt->t_reglist_kls, KLS_Region);
        }
        break;
        default: {
            fprintf(stderr,
                    "[ERROR]    %s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                    caller, data_pt->conf.tkls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "%s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                    caller, data_pt->conf.tkls_reglist_alloc_backend);
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
        data_pt->t_regs = kls_rl_t_cons(t_kls, reg, data_pt->t_regs);
    }
}

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
    ptrdiff_t padding = -1;
#ifndef KOLISEO_HAS_LOCATE
    char* p = kls__advance(kls, size, align, count, &padding, __func__);
#else
    char* p = kls__advance_dbg(kls, size, align, count, &padding, __func__, loc);
#endif // KOLISEO_HAS_LOCATE
    if (!p) return NULL;

    //Zero new area
    memset(p, 0, size * count);
    kls__autoregion(__func__, kls, padding, name, strlen(name)+1, desc, strlen(desc)+1, KLS_None);

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
    ptrdiff_t padding = -1;
#ifndef KOLISEO_HAS_LOCATE
    char* p = kls__temp_advance(t_kls, size, align, count, &padding, __func__);
#else
    char* p = kls__temp_advance_dbg(t_kls, size, align, count, &padding, __func__, loc);
#endif // KOLISEO_HAS_LOCATE
    if (!p) return NULL;

    //Zero new area
    memset(p, 0, size * count);
    kls__temp_autoregion(__func__, t_kls, padding, name, strlen(name)+1, desc, strlen(desc)+1, KLS_None);
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
    ptrdiff_t padding = -1;
#ifndef KOLISEO_HAS_LOCATE
    char* p = kls__advance(kls, size, align, count, &padding, __func__);
#else
    char* p = kls__advance_dbg(kls, size, align, count, &padding, __func__, loc);
#endif // KOLISEO_HAS_LOCATE
    if (!p) return NULL;

    //Zero new area
    memset(p, 0, size * count);
    kls__autoregion(__func__, kls, padding, name, strlen(name), desc, strlen(desc), type);

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
    ptrdiff_t padding = -1;
#ifndef KOLISEO_HAS_LOCATE
    char* p = kls__temp_advance(t_kls, size, align, count, &padding, __func__);
#else
    char* p = kls__temp_advance_dbg(t_kls, size, align, count, &padding, __func__, loc);
#endif // KOLISEO_HAS_LOCATE
    if (!p) return NULL;

    //Zero new area
    memset(p, 0, size * count);
    kls__temp_autoregion(__func__, t_kls, padding, name, strlen(name)+1, desc, strlen(desc)+1, type);
    return p;
}

void KLS_autoregion_on_new(struct Koliseo* kls)
{
    assert(kls != NULL);
    if (kls->extension_data[KLS_AUTOREGION_EXT_SLOT] == NULL) { // Need to init kls extension data
        kls->extension_data[KLS_AUTOREGION_EXT_SLOT] = KLS_DEFAULT_ALLOCF(sizeof(KLS_Autoregion_Extension_Data));
        KLS_Autoregion_Extension_Data* data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
        (*data_pt) = (KLS_Autoregion_Extension_Data) {
            .conf = (KLS_Autoregion_Extension_Conf) {
                .kls_autoset_regions = KLS_AUTOREGION_AUTOSET_DEFAULT,
                .kls_reglist_alloc_backend = KLS_AUTOREGION_ALLOC_DEFAULT,
                .kls_reglist_kls_size = KLS_AUTOREGION_REGLIST_SIZE_DEFAULT,
                .kls_autoset_temp_regions = KLS_AUTOREGION_AUTOSET_TEMP_DEFAULT,
                .tkls_reglist_alloc_backend = KLS_AUTOREGION_ALLOC_TEMP_DEFAULT,
                .tkls_reglist_kls_size = KLS_AUTOREGION_REGLIST_TEMP_SIZE_DEFAULT
            }
        };
    }

    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    if (data_pt->conf.kls_autoset_regions == 1) {
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "KLS", "Init of KLS_Region_List for kls.");
#endif
        KLS_Region *kls_header = NULL;
        switch(data_pt->conf.kls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            kls_header = (KLS_Region *) malloc(sizeof(KLS_Region));
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            Koliseo *reglist_kls = NULL;
            KLS_Hooks ext = {0};
            reglist_kls = kls_new_conf_ext(data_pt->conf.kls_reglist_kls_size, KLS_DEFAULT_CONF__, &ext, NULL, 0);
            data_pt->reglist_kls = reglist_kls;
            data_pt->max_regions_kls_alloc_basic =
                kls_get_maxRegions_KLS_BASIC(kls);
            kls_header = (KLS_Region *) KLS_PUSH(data_pt->reglist_kls, KLS_Region);
        }
        break;
        case KLS_REGLIST_ALLOC_KLS: {
            Koliseo *reglist_kls = NULL;
            KLS_Hooks ext = {0};
            reglist_kls = kls_new_conf_ext(data_pt->conf.kls_reglist_kls_size, KLS_DEFAULT_CONF__, &ext, NULL, 0);
            reglist_kls->conf.kls_growable = 1;
            data_pt->reglist_kls = reglist_kls;
            kls_header = (KLS_Region *) KLS_PUSH(data_pt->reglist_kls, KLS_Region);
        }
        break;
        default: {
            fprintf(stderr,
                    "[ERROR] [%s()]:  Unexpected KLS_RegList_Alloc_Backend value: {%i}.\n",
                    __func__, data_pt->conf.tkls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "%s():  Invalid KLS_RegList_Alloc_Backend value: {%i}.",
                    __func__, data_pt->conf.tkls_reglist_alloc_backend);
#endif
            kls_free(kls);
            exit(EXIT_FAILURE);
        }
        break;
        }
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
        data_pt->regs = kls_rl_cons(kls, kls_header, data_pt->regs);
        if (data_pt->regs == NULL) {
            fprintf(stderr,
                    "[KLS] [%s()]: failed to get a KLS_Region_List.\n",
                    __func__);
            exit(EXIT_FAILURE);
        }
    } else {
        data_pt->regs = NULL;
    }
}

void KLS_autoregion_on_free(struct Koliseo* kls)
{
    assert(kls != NULL);
    if (kls->extension_data[KLS_AUTOREGION_EXT_SLOT] == NULL) { // No need to free kls extension data
        return;
    }
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    if (data_pt->conf.kls_reglist_alloc_backend == KLS_REGLIST_ALLOC_KLS_BASIC || data_pt->conf.kls_reglist_alloc_backend == KLS_REGLIST_ALLOC_KLS) {
        kls_free(data_pt->reglist_kls);
        //free(kls->reglist_kls);
    } else {
        kls_rl_freeList(data_pt->regs);
    }

    KLS_DEFAULT_FREEF(kls->extension_data[KLS_AUTOREGION_EXT_SLOT]);
}

void KLS_autoregion_on_push(struct Koliseo* kls, ptrdiff_t padding, const char* caller, void* user)
{
    assert(kls != NULL);
    if (kls->extension_data[KLS_AUTOREGION_EXT_SLOT] == NULL) {
        return;
    }
    struct KLS_EXTENSION_AR_DEFAULT_ARGS {
        const char* region_name;
        size_t region_name_len;
        const char* region_desc;
        size_t region_desc_len;
        int region_type;
    };
    if (user) {
        struct KLS_EXTENSION_AR_DEFAULT_ARGS *ar_args = user;
        kls__autoregion(caller, kls, padding, ar_args->region_name, strlen(ar_args->region_name), ar_args->region_desc, strlen(ar_args->region_desc), ar_args->region_type);
    } else {
        kls__autoregion(caller, kls, padding, KOLISEO_DEFAULT_REGION_NAME, strlen(KOLISEO_DEFAULT_REGION_NAME), KOLISEO_DEFAULT_REGION_DESC, strlen(KOLISEO_DEFAULT_REGION_DESC), KLS_None);
    }
}

void KLS_autoregion_on_temp_start(struct Koliseo_Temp* t_kls)
{
    assert(t_kls != NULL);
    Koliseo* kls = t_kls->kls;
    assert(kls != NULL);
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    if (data_pt->conf.kls_autoset_temp_regions == 1) {
#ifdef KLS_DEBUG_CORE
        kls_log(kls, "KLS", "Init of KLS_Region_List for temp kls.");
#endif
        KLS_Region *temp_kls_header = NULL;
        switch (data_pt->conf.tkls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            temp_kls_header = (KLS_Region *) malloc(sizeof(KLS_Region));
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC: {
            Koliseo *t_reglist_kls = NULL;
            KLS_Hooks ext = {0};
            t_reglist_kls = kls_new_conf_ext(data_pt->conf.kls_reglist_kls_size, KLS_DEFAULT_CONF__, &ext, NULL, 0);
            data_pt->t_reglist_kls = t_reglist_kls;
            data_pt->max_regions_kls_alloc_basic =
                kls_get_maxRegions_KLS_BASIC(kls);
            temp_kls_header = (KLS_Region *) KLS_PUSH(data_pt->t_reglist_kls, KLS_Region);
        }
        break;
        case KLS_REGLIST_ALLOC_KLS: {
            Koliseo *t_reglist_kls = NULL;
            KLS_Hooks ext = {0};
            t_reglist_kls = kls_new_conf_ext(data_pt->conf.kls_reglist_kls_size, KLS_DEFAULT_CONF__, &ext, NULL, 0);
            t_reglist_kls->conf.kls_growable = 1;
            data_pt->t_reglist_kls = t_reglist_kls;
            temp_kls_header = (KLS_Region *) KLS_PUSH(data_pt->t_reglist_kls, KLS_Region);
        }
        break;
        default: {
            fprintf(stderr,
                    "[ERROR]    %s():  Invalid conf.kls_reglist_alloc_backend value: {%i}.\n",
                    __func__, data_pt->conf.kls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
            kls_log(kls, "ERROR",
                    "%s():  Invalid conf.kls_reglist_alloc_backend value: {%i}.\n",
                    __func__, data_pt->conf.kls_reglist_alloc_backend);
#endif
            kls_free(kls);
            exit(EXIT_FAILURE);
        }
        break;
        }
        temp_kls_header->begin_offset = t_kls->prev_offset;
        temp_kls_header->end_offset = t_kls->offset;
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
        reglist = kls_rl_t_cons(t_kls, temp_kls_header, reglist);
        data_pt->t_regs = reglist;
        if (data_pt->t_regs == NULL) {
            fprintf(stderr, "[KLS] [%s()]: failed to get a KLS_Region_List.\n",
                    __func__);
            exit(EXIT_FAILURE);
        }
    } else {
        data_pt->t_regs = NULL;
    }
}

void KLS_autoregion_on_temp_free(struct Koliseo_Temp* t_kls)
{
    assert(t_kls != NULL);
    Koliseo* kls = t_kls->kls;
    assert(kls != NULL);
    KLS_Autoregion_Extension_Data *data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];
    if (data_pt->conf.kls_autoset_regions == 1) {
        switch (data_pt->conf.tkls_reglist_alloc_backend) {
        case KLS_REGLIST_ALLOC_LIBC: {
            kls_rl_freeList(data_pt->t_regs);
        }
        break;
        case KLS_REGLIST_ALLOC_KLS_BASIC:
        case KLS_REGLIST_ALLOC_KLS: {
            kls_free(data_pt->t_reglist_kls);
        }
        break;
        default: {
            fprintf(stderr,
                    "[ERROR]    %s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                    __func__, data_pt->conf.tkls_reglist_alloc_backend);
#ifdef KLS_DEBUG_CORE
            kls_log(t_kls->kls, "ERROR",
                    "%s():  Invalid conf.tkls_reglist_alloc_backend value: {%i}.\n",
                    __func__, data_pt->conf.tkls_reglist_alloc_backend);
#endif
            kls_free(t_kls->kls);
            exit(EXIT_FAILURE);
        }
        break;
        }
    }
}

void KLS_autoregion_on_temp_push(struct Koliseo_Temp* t_kls, ptrdiff_t padding, const char* caller, void* user)
{
    assert(t_kls != NULL);
    Koliseo* kls = t_kls->kls;
    assert(kls != NULL);
    struct KLS_EXTENSION_AR_DEFAULT_ARGS {
        const char* region_name;
        size_t region_name_len;
        const char* region_desc;
        size_t region_desc_len;
        int region_type;
    };
    if (user) {
        struct KLS_EXTENSION_AR_DEFAULT_ARGS *ar_args = user;
        kls__temp_autoregion(caller, t_kls, padding, ar_args->region_name, strlen(ar_args->region_name), ar_args->region_desc, strlen(ar_args->region_desc), ar_args->region_type);
    } else {
        kls__temp_autoregion(caller, t_kls, padding, KOLISEO_DEFAULT_REGION_NAME, strlen(KOLISEO_DEFAULT_REGION_NAME), KOLISEO_DEFAULT_REGION_DESC, strlen(KOLISEO_DEFAULT_REGION_DESC), KLS_None);
    }
}
