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
// least LIST_T to the type the linked list can hold.
// If you define LIST_CMP_DEFAULT_FN, it will be used as default comparator for
// functions not accepting one and as a fallback on NULL argument for the ones who do.
// See LIST_NAME, LIST_PREFIX and LIST_LINKAGE for
// other customization points.
//
// If you define LIST_DECLS_ONLY, only the declarations
// of the type and its function will be declared.
//
// Functions ending with _gl use malloc() for the nodes.
// Functions ending with _kls expect a Koliseo arg to use for allocating nodes.
//
// CHANGELOG
//
// 0.1.2 - LIST_CMP_DEFAULT_FN customization
//         LIST_CMP_FN, typedef bool(LIST_cmp)(LIST_T*, LIST_T*)
//         New API with _fn suffix taking a comparator, LIST_CMP_DEFAULT_FN
//         New API with _p doing pointer comparison explicitly
// 0.1.1 - LIST_NAME_free_gl() now also frees all elements
// 0.1.0 - First release

#ifndef LIST_HEADER_H
#define LIST_HEADER_H
// Inline functions, #defines and includes that will be
// needed for all instantiations can go up here.
#include <stdbool.h> // bool
#include <stdlib.h> // malloc, size_t

#define LIST_IMPL(word) LIST_COMB1(LIST_PREFIX,word)
#define LIST_COMB1(pre, word) LIST_COMB2(pre, word)
#define LIST_COMB2(pre, word) pre##word

#define LIST_HEADER_VERSION "0.1.2"

#endif // LIST_HEADER_H

// NOTE: this section is *not* guarded as it is intended
// to be included multiple times.

#ifndef LIST_T
#error "LIST_T must be defined to use this list template"
#endif

#ifndef KOLISEO_H_ // Using code must include koliseo.h before this file
#error "This list template needs koliseo.h inclusion before usage"
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
#define LIST_cmp LIST_IMPL(cmp)
#define LIST_dumb_cmp LIST_IMPL(dumb_cmp)
#define LIST_member_fn LIST_IMPL(member_fn)
#define LIST_member LIST_IMPL(member)
#define LIST_member_p LIST_IMPL(member_p)
#define LIST_length LIST_IMPL(length)
#define LIST_append_gl LIST_IMPL(append_gl)
#define LIST_append_kls LIST_IMPL(append_kls)
#define LIST_reverse_gl LIST_IMPL(reverse_gl)
#define LIST_reverse_kls LIST_IMPL(reverse_kls)
#define LIST_copy_gl LIST_IMPL(copy_gl)
#define LIST_copy_kls LIST_IMPL(copy_kls)
#define LIST_remove_gl_fn LIST_IMPL(remove_gl_fn)
#define LIST_remove_gl LIST_IMPL(remove_gl)
#define LIST_remove_p_gl LIST_IMPL(remove_p_gl)
#define LIST_remove_kls_fn LIST_IMPL(remove_kls_fn)
#define LIST_remove_kls LIST_IMPL(remove_kls)
#define LIST_remove_p_kls LIST_IMPL(remove_p_kls)
#define LIST_intersect_gl_fn LIST_IMPL(intersect_gl_fn)
#define LIST_intersect_gl LIST_IMPL(intersect_gl)
#define LIST_intersect_p_gl LIST_IMPL(intersect_p_gl)
#define LIST_intersect_kls_fn LIST_IMPL(intersect_kls_fn)
#define LIST_intersect_kls LIST_IMPL(intersect_kls)
#define LIST_intersect_p_kls LIST_IMPL(intersect_p_kls)
#define LIST_diff_gl_fn LIST_IMPL(diff_gl_fn)
#define LIST_diff_gl LIST_IMPL(diff_gl)
#define LIST_diff_p_gl LIST_IMPL(diff_p_gl)
#define LIST_diff_kls_fn LIST_IMPL(diff_kls_fn)
#define LIST_diff_kls LIST_IMPL(diff_kls)
#define LIST_diff_p_kls LIST_IMPL(diff_p_kls)

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

#define LIST_CMP_FN bool(LIST_cmp)(const LIST_T*, const LIST_T*)
typedef LIST_CMP_FN;

LIST_LINKAGE
bool
LIST_dumb_cmp(const LIST_T* a, const LIST_T* b);

#ifndef LIST_CMP_DEFAULT_FN
#define LIST_CMP_DEFAULT_FN &LIST_dumb_cmp
#endif // LIST_CMP_DEFAULT_FN

LIST_LINKAGE
bool
LIST_member_fn(LIST_T* element, LIST_NAME list, LIST_cmp* comparator );

LIST_LINKAGE
bool
LIST_member(LIST_T* element, LIST_NAME list);

LIST_LINKAGE
bool
LIST_member_p(LIST_T* element, LIST_NAME list);

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
LIST_remove_gl_fn(LIST_T* element, LIST_NAME list, LIST_cmp* comparator);

LIST_LINKAGE
LIST_NAME
LIST_remove_gl(LIST_T* element, LIST_NAME list);

LIST_LINKAGE
LIST_NAME
LIST_remove_p_gl(LIST_T* element, LIST_NAME list);

LIST_LINKAGE
LIST_NAME
LIST_remove_kls_fn(Koliseo* kls, LIST_T* element, LIST_NAME list, LIST_cmp* comparator);

LIST_LINKAGE
LIST_NAME
LIST_remove_kls(Koliseo* kls, LIST_T* element, LIST_NAME list);

LIST_LINKAGE
LIST_NAME
LIST_remove_p_kls(Koliseo* kls, LIST_T* element, LIST_NAME list);

LIST_LINKAGE
LIST_NAME
LIST_intersect_gl_fn(LIST_NAME l1, LIST_NAME l2, LIST_cmp* comparator);

LIST_LINKAGE
LIST_NAME
LIST_intersect_gl(LIST_NAME l1, LIST_NAME l2);

LIST_LINKAGE
LIST_NAME
LIST_intersect_p_gl(LIST_NAME l1, LIST_NAME l2);

LIST_LINKAGE
LIST_NAME
LIST_intersect_kls_fn(Koliseo* kls, LIST_NAME l1, LIST_NAME l2, LIST_cmp* comparator);

LIST_LINKAGE
LIST_NAME
LIST_intersect_kls(Koliseo* kls, LIST_NAME l1, LIST_NAME l2);

LIST_LINKAGE
LIST_NAME
LIST_intersect_p_kls(Koliseo* kls, LIST_NAME l1, LIST_NAME l2);

LIST_LINKAGE
LIST_NAME
LIST_diff_gl_fn(LIST_NAME l1, LIST_NAME l2, LIST_cmp* comparator);

LIST_LINKAGE
LIST_NAME
LIST_diff_gl(LIST_NAME l1, LIST_NAME l2);

LIST_LINKAGE
LIST_NAME
LIST_diff_p_gl(LIST_NAME l1, LIST_NAME l2);

LIST_LINKAGE
LIST_NAME
LIST_diff_kls_fn(Koliseo* kls, LIST_NAME l1, LIST_NAME l2, LIST_cmp* comparator);

LIST_LINKAGE
LIST_NAME
LIST_diff_kls(Koliseo* kls, LIST_NAME l1, LIST_NAME l2);

LIST_LINKAGE
LIST_NAME
LIST_diff_p_kls(Koliseo* kls, LIST_NAME l1, LIST_NAME l2);

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
        free(list->value);
        free(list);
    }
    return;
}

#define LIST_CMP_FN bool(LIST_cmp)(const LIST_T*, const LIST_T*)
typedef LIST_CMP_FN;

LIST_LINKAGE
bool
LIST_dumb_cmp(const LIST_T* a, const LIST_T* b)
{
    // ! Pointer comparison ! Avoid resorting to this.
    fprintf(stderr, "%s at %i: %s(): Doing pointer comparison.\n", __FILE__, __LINE__, __func__);
    fprintf(stderr, "%s at %i: %s(): Try defining LIST_CMP_DEFAULT_FN before importing the list.\n", __FILE__, __LINE__, __func__);
    return a == b;
}

LIST_LINKAGE
bool
LIST_member_fn(LIST_T* element, LIST_NAME list, LIST_cmp* comparator)
{
    if (LIST_isEmpty(list)) {
        return false;
    } else {
        if (comparator) {
            if (comparator(element, LIST_head(list))) {
                return true;
            } else {
                return LIST_member_fn(element, LIST_tail(list), comparator);
            }
        } else {
            // Fallback to default comparator
            LIST_cmp* cmp = LIST_CMP_DEFAULT_FN;
            if (cmp(element, LIST_head(list))) {
                return true;
            } else {
                return LIST_member_fn(element, LIST_tail(list), cmp);
            }
        }
    }
}

LIST_LINKAGE
bool
LIST_member(LIST_T* element, LIST_NAME list)
{
    return LIST_member_fn(element, list, LIST_CMP_DEFAULT_FN);
}

LIST_LINKAGE
bool
LIST_member_p(LIST_T* element, LIST_NAME list)
{
    if (LIST_isEmpty(list)) {
        return false;
    } else {
        if (element == LIST_head(list)) {
            return true;
        } else {
            return LIST_member_p(element, LIST_tail(list));
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
LIST_remove_gl_fn(LIST_T* element, LIST_NAME list, LIST_cmp* comparator)
{
    if (LIST_isEmpty(list)) {
        return LIST_nullList();
    } else {
        if (comparator) {
            if (comparator(element, LIST_head(list))) {
                return LIST_tail(list);
            } else {
                return LIST_cons_gl(LIST_head(list), LIST_remove_gl_fn(element, LIST_tail(list), comparator));
            }
        } else {
            // Fallback to default comparator
            LIST_cmp* cmp = LIST_CMP_DEFAULT_FN;
            if (cmp(element, LIST_head(list))) {
                return LIST_tail(list);
            } else {
                return LIST_cons_gl(LIST_head(list), LIST_remove_gl_fn(element, LIST_tail(list), cmp));
            }
        }
    }
}

LIST_LINKAGE
LIST_NAME
LIST_remove_gl(LIST_T* element, LIST_NAME list)
{
    return LIST_remove_gl_fn(element, list, LIST_CMP_DEFAULT_FN);
}

LIST_LINKAGE
LIST_NAME
LIST_remove_p_gl(LIST_T* element, LIST_NAME list)
{
    if (LIST_isEmpty(list)) {
        return LIST_nullList();
    } else {
        if (element == LIST_head(list)) {
            return LIST_tail(list);
        } else {
            return LIST_cons_gl(LIST_head(list), LIST_remove_p_gl(element, LIST_tail(list)));
        }
    }
}

LIST_LINKAGE
LIST_NAME
LIST_remove_kls_fn(Koliseo* kls, LIST_T* element, LIST_NAME list, LIST_cmp* comparator)
{
    if (kls == NULL) {
        fprintf(stderr, "%s at %i: %s(): Koliseo is NULL.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    if (LIST_isEmpty(list)) {
        return LIST_nullList();
    } else {
        if (comparator) {
            if (comparator(element, LIST_head(list))) {
                return LIST_tail(list);
            } else {
                return LIST_cons_kls(kls, LIST_head(list), LIST_remove_kls_fn(kls, element, LIST_tail(list), comparator));
            }
        } else {
            // Fallback to default comparator
            LIST_cmp* cmp = LIST_CMP_DEFAULT_FN;
            if (cmp(element, LIST_head(list))) {
                return LIST_tail(list);
            } else {
                return LIST_cons_kls(kls, LIST_head(list), LIST_remove_kls_fn(kls, element, LIST_tail(list), cmp));
            }
        }
    }
}

LIST_LINKAGE
LIST_NAME
LIST_remove_kls(Koliseo* kls, LIST_T* element, LIST_NAME list)
{
    return LIST_remove_kls_fn(kls, element, list, LIST_CMP_DEFAULT_FN);
}

LIST_LINKAGE
LIST_NAME
LIST_remove_p_kls(Koliseo* kls, LIST_T* element, LIST_NAME list)
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
            return LIST_cons_kls(kls, LIST_head(list), LIST_remove_p_kls(kls, element, LIST_tail(list)));
        }
    }
}

LIST_LINKAGE
LIST_NAME
LIST_intersect_gl_fn(LIST_NAME l1, LIST_NAME l2, LIST_cmp* comparator)
{
    if (LIST_isEmpty(l1) || LIST_isEmpty(l2)) {
        return LIST_nullList();
    }
    if (LIST_member_fn(LIST_head(l1), l2, comparator) && !(LIST_member_fn(LIST_head(l1), LIST_tail(l2), comparator))) {
        return LIST_cons_gl(LIST_head(l1), LIST_intersect_gl_fn(LIST_tail(l1), l2, comparator));
    } else {
        return LIST_intersect_gl_fn(LIST_tail(l1), l2, comparator);
    }
}

LIST_LINKAGE
LIST_NAME
LIST_intersect_gl(LIST_NAME l1, LIST_NAME l2)
{
    return LIST_intersect_gl_fn(l1, l2, LIST_CMP_DEFAULT_FN);
}

LIST_LINKAGE
LIST_NAME
LIST_intersect_p_gl(LIST_NAME l1, LIST_NAME l2)
{
    if (LIST_isEmpty(l1) || LIST_isEmpty(l2)) {
        return LIST_nullList();
    }
    if (LIST_member_p(LIST_head(l1), l2) && !(LIST_member_p(LIST_head(l1), LIST_tail(l2)))) {
        return LIST_cons_gl(LIST_head(l1), LIST_intersect_p_gl(LIST_tail(l1), l2));
    } else {
        return LIST_intersect_p_gl(LIST_tail(l1), l2);
    }
}

LIST_LINKAGE
LIST_NAME
LIST_intersect_kls_fn(Koliseo* kls, LIST_NAME l1, LIST_NAME l2, LIST_cmp* comparator)
{
    if (kls == NULL) {
        fprintf(stderr, "%s at %i: %s(): Koliseo is NULL.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    if (LIST_isEmpty(l1) || LIST_isEmpty(l2)) {
        return LIST_nullList();
    }
    if (LIST_member_fn(LIST_head(l1), l2, comparator) && !(LIST_member_fn(LIST_head(l1), LIST_tail(l2), comparator))) {
        return LIST_cons_kls(kls, LIST_head(l1), LIST_intersect_kls_fn(kls, LIST_tail(l1), l2, comparator));
    } else {
        return LIST_intersect_kls_fn(kls, LIST_tail(l1), l2, comparator);
    }
}

LIST_LINKAGE
LIST_NAME
LIST_intersect_kls(Koliseo* kls, LIST_NAME l1, LIST_NAME l2)
{
    return LIST_intersect_kls_fn(kls, l1, l2, LIST_CMP_DEFAULT_FN);
}

LIST_LINKAGE
LIST_NAME
LIST_intersect_p_kls(Koliseo* kls, LIST_NAME l1, LIST_NAME l2)
{
    if (kls == NULL) {
        fprintf(stderr, "%s at %i: %s(): Koliseo is NULL.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    if (LIST_isEmpty(l1) || LIST_isEmpty(l2)) {
        return LIST_nullList();
    }
    if (LIST_member_p(LIST_head(l1), l2) && !(LIST_member_p(LIST_head(l1), LIST_tail(l2)))) {
        return LIST_cons_kls(kls, LIST_head(l1), LIST_intersect_p_kls(kls, LIST_tail(l1), l2));
    } else {
        return LIST_intersect_p_kls(kls, LIST_tail(l1), l2);
    }
}

LIST_LINKAGE
LIST_NAME
LIST_diff_gl_fn(LIST_NAME l1, LIST_NAME l2, LIST_cmp* comparator)
{
    if (LIST_isEmpty(l1) || LIST_isEmpty(l2)) {
        return l1;
    } else {
        if (!LIST_member_fn(LIST_head(l1), l2, comparator) && !LIST_member_fn(LIST_head(l1), LIST_tail(l1), comparator)) {
            return LIST_cons_gl(LIST_head(l1), LIST_diff_gl_fn(LIST_tail(l1), l2, comparator));
        } else {
            return LIST_diff_gl_fn(LIST_tail(l1), l2, comparator);
        }
    }
}

LIST_LINKAGE
LIST_NAME
LIST_diff_gl(LIST_NAME l1, LIST_NAME l2)
{
    return LIST_diff_gl_fn(l1, l2, LIST_CMP_DEFAULT_FN);
}

LIST_LINKAGE
LIST_NAME
LIST_diff_p_gl(LIST_NAME l1, LIST_NAME l2)
{
    if (LIST_isEmpty(l1) || LIST_isEmpty(l2)) {
        return l1;
    } else {
        if (!LIST_member_p(LIST_head(l1), l2) && !LIST_member_p(LIST_head(l1), LIST_tail(l1))) {
            return LIST_cons_gl(LIST_head(l1), LIST_diff_p_gl(LIST_tail(l1), l2));
        } else {
            return LIST_diff_p_gl(LIST_tail(l1), l2);
        }
    }
}

LIST_LINKAGE
LIST_NAME
LIST_diff_kls_fn(Koliseo* kls, LIST_NAME l1, LIST_NAME l2, LIST_cmp* comparator)
{
    if (kls == NULL) {
        fprintf(stderr, "%s at %i: %s(): Koliseo is NULL.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    if (LIST_isEmpty(l1) || LIST_isEmpty(l2)) {
        return l1;
    } else {
        if (!LIST_member_fn(LIST_head(l1), l2, comparator) && !LIST_member_fn(LIST_head(l1), LIST_tail(l1), comparator)) {
            return LIST_cons_kls(kls, LIST_head(l1), LIST_diff_kls_fn(kls, LIST_tail(l1), l2, comparator));
        } else {
            return LIST_diff_kls_fn(kls, LIST_tail(l1), l2, comparator);
        }
    }
}

LIST_LINKAGE
LIST_NAME
LIST_diff_kls(Koliseo* kls, LIST_NAME l1, LIST_NAME l2)
{
    //TODO: add macro to define default comparator
    return LIST_diff_kls_fn(kls, l1, l2, NULL);
}

LIST_LINKAGE
LIST_NAME
LIST_diff_p_kls(Koliseo* kls, LIST_NAME l1, LIST_NAME l2)
{
    if (kls == NULL) {
        fprintf(stderr, "%s at %i: %s(): Koliseo is NULL.\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
    if (LIST_isEmpty(l1) || LIST_isEmpty(l2)) {
        return l1;
    } else {
        if (!LIST_member_p(LIST_head(l1), l2) && !LIST_member_p(LIST_head(l1), LIST_tail(l1))) {
            return LIST_cons_kls(kls, LIST_head(l1), LIST_diff_p_kls(kls, LIST_tail(l1), l2));
        } else {
            return LIST_diff_p_kls(kls, LIST_tail(l1), l2);
        }
    }
}
#endif // LIST_DECLS_ONLY

// Cleanup
// These need to be undef'ed so they can be redefined the
// next time you need to instantiate this template.
#undef LIST_T
#undef LIST_PREFIX
#undef LIST_NAME
#undef LIST_LINKAGE
#undef LIST_I_SUFFIX
#undef LIST_ITEM_NAME
#undef LIST_CMP_DEFAULT_FN
#undef LIST_nullList
#undef LIST_isEmpty
#undef LIST_head
#undef LIST_tail
#undef LIST_cons_gl
#undef LIST_cons_kls
#undef LIST_free_gl
#undef LIST_cmp
#undef LIST_CMP_FN
#undef LIST_member_fn
#undef LIST_member
#undef LIST_member_p
#undef LIST_length
#undef LIST_append_gl
#undef LIST_append_kls
#undef LIST_reverse_gl
#undef LIST_reverse_kls
#undef LIST_copy_gl
#undef LIST_copy_kls
#undef LIST_remove_gl_fn
#undef LIST_remove_gl
#undef LIST_remove_p_gl
#undef LIST_remove_kls_fn
#undef LIST_remove_kls
#undef LIST_remove_p_kls
#undef LIST_intersect_gl_fn
#undef LIST_intersect_gl
#undef LIST_intersect_p_gl
#undef LIST_intersect_kls_fn
#undef LIST_intersect_kls
#undef LIST_intersect_p_kls
#undef LIST_diff_gl_fn
#undef LIST_diff_gl
#undef LIST_diff_p_gl
#undef LIST_diff_kls_fn
#undef LIST_diff_kls
#undef LIST_diff_p_kls
#ifdef LIST_DECLS_ONLY
#undef LIST_DECLS_ONLY
#endif // LIST_HEADER_H
