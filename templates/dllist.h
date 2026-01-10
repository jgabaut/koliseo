#ifdef LIST_T //This ensures the library never causes any trouble if this macro was not defined.
// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
/*
    Copyright (C) 2025-2026  jgabaut

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

/*****************************************************************************\
| dllist.h                                                                    |
| This is a template for a doubly linked list, inspired by the dynamic array  |
| example in https://www.davidpriver.com/ctemplates.html#template-headers.    |
| Include this header multiple times to implement a simplistic linked list.   |
| Before inclusion define at least DLIST_T to the type the linked list can    |
| hold. See DLIST_NAME, DLIST_PREFIX and DLIST_LINKAGE for other              |
| customization points.                                                       |
|                                                                             |
| If you define DLIST_DECLS_ONLY, only the declarations                       |
| of the type and its function will be declared.                              |
\*****************************************************************************/

#ifndef DLLIST_HEADER_H
#define DLLIST_HEADER_H
// Inline functions, #defines and includes that will be
// needed for all instantiations can go up here.
#include <stdlib.h>

#define LIST_IMPL(word) LIST_COMB1(LIST_PREFIX,word)
#define LIST_COMB1(pre, word) LIST_COMB2(pre, word)
#define LIST_COMB2(pre, word) pre##word

#define DLLIST_HEADER_VERSION "0.1.0"

#endif // DLLIST_HEADER_H

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
    LIST_T* data;
    struct LIST_ITEM_NAME* next;
    struct LIST_ITEM_NAME* prev;
};

typedef struct LIST_NAME LIST_NAME;
struct LIST_NAME {
    LIST_ITEM_NAME* firstNode;
    LIST_ITEM_NAME* lastNode;
    int len;
};

#define LIST_newNode_gl LIST_IMPL(newNode_gl)
#define LIST_newNode_kls LIST_IMPL(newNode_kls)
#define LIST_freeNode LIST_IMPL(freeNode)
#define LIST_newList_gl LIST_IMPL(newList_gl)
#define LIST_newList_kls LIST_IMPL(newList_kls)
#define LIST_removeNode_gl LIST_IMPL(removeNode_gl)
#define LIST_removeNode_kls LIST_IMPL(removeNode_kls)
#define LIST_free LIST_IMPL(free)
#define LIST_insertAfter LIST_IMPL(insertAfter)
#define LIST_insertBefore LIST_IMPL(insertBefore)
#define LIST_insertBeginning LIST_IMPL(insertBeginning)
#define LIST_insertEnd LIST_IMPL(insertEnd)
#define LIST_getNodeAt LIST_IMPL(getNodeAt)

#ifdef LIST_DECLS_ONLY

LIST_LINKAGE
LIST_ITEM_NAME*
LIST_newNode_gl(LIST_T* data);

LIST_LINKAGE
LIST_ITEM_NAME*
LIST_newNode_kls(Koliseo* kls, LIST_T* data);

LIST_LINKAGE
void
LIST_freeNode(LIST_ITEM_NAME* node);

LIST_LINKAGE
LIST_NAME*
LIST_newList_gl(void);

LIST_LINKAGE
LIST_NAME*
LIST_newList_kls(Koliseo* kls);

LIST_LINKAGE
void
LIST_removeNode_gl(LIST_NAME* list, LIST_ITEM_NAME* node);

LIST_LINKAGE
void
LIST_removeNode_kls(LIST_NAME* list, LIST_ITEM_NAME* node);

LIST_LINKAGE
void
LIST_free(LIST_NAME* list);

LIST_LINKAGE
int
LIST_insertAfter(LIST_NAME* list, LIST_ITEM_NAME* node, LIST_ITEM_NAME* new_node);

LIST_LINKAGE
int
LIST_insertBefore(LIST_NAME* list, LIST_ITEM_NAME* node, LIST_ITEM_NAME* new_node);

LIST_LINKAGE
int
LIST_insertBeginning(LIST_NAME* list, LIST_ITEM_NAME* node);

LIST_LINKAGE
int
LIST_insertEnd(LIST_NAME* list, LIST_ITEM_NAME* node);

LIST_LINKAGE
LIST_ITEM_NAME*
LIST_getNodeAt(LIST_NAME* list, int pos);

#else

LIST_LINKAGE
LIST_ITEM_NAME*
LIST_newNode_gl(LIST_T* data)
{
    if (data == NULL) {
        return NULL;
    } else {
        LIST_ITEM_NAME* new_node = malloc(sizeof(LIST_ITEM_NAME));
        new_node->data = malloc(sizeof(data));
        *(new_node->data) = *data;
        return new_node;
    }
};

LIST_LINKAGE
LIST_ITEM_NAME*
LIST_newNode_kls(Koliseo* kls, LIST_T* data)
{
    if (data == NULL) {
        return NULL;
    } else {
        LIST_ITEM_NAME* new_node = KLS_PUSH(kls, LIST_ITEM_NAME);
        new_node->data = KLS_PUSH(kls, LIST_T);
        *(new_node->data) = *data;
        return new_node;
    }
}

LIST_LINKAGE
void
LIST_freeNode(LIST_ITEM_NAME* node)
{
    free(node->data);
    free(node);
}

LIST_LINKAGE
LIST_NAME*
LIST_newList_gl(void)
{
    LIST_NAME* list = malloc(sizeof(LIST_NAME));
    list->len = 0;
    return list;
}

LIST_LINKAGE
LIST_NAME*
LIST_newList_kls(Koliseo* kls)
{
    LIST_NAME* list = KLS_PUSH(kls, LIST_NAME);
    list->len = 0;
    return list;
}

LIST_LINKAGE
void
LIST_removeNode_gl(LIST_NAME* list, LIST_ITEM_NAME* node)
{
    if (node->prev == NULL) {
        list->firstNode = node->next;
    } else {
        node->prev->next = node->next;
    }
    if (node->next == NULL) {
        list->lastNode = node->prev;
    } else {
        node->next->prev = node->prev;
    }
    LIST_freeNode(node);
}

LIST_LINKAGE
void
LIST_removeNode_kls(LIST_NAME* list, LIST_ITEM_NAME* node)
{
    if (node->prev == NULL) {
        list->firstNode = node->next;
    } else {
        node->prev->next = node->next;
    }
    if (node->next == NULL) {
        list->lastNode = node->prev;
    } else {
        node->next->prev = node->prev;
    }
}

LIST_LINKAGE
void
LIST_free(LIST_NAME* list)
{
    LIST_ITEM_NAME* node = list->lastNode;
    while (node != NULL) {
        LIST_ITEM_NAME* prev = node->prev;
        LIST_removeNode_gl(list,node);
        node = prev;
    }
}

LIST_LINKAGE
int
LIST_insertAfter(LIST_NAME* list, LIST_ITEM_NAME* node, LIST_ITEM_NAME* new_node)
{
    if (node == NULL) {
        return -1;
    }
    if (new_node == NULL) {
        return -1;
    }
    new_node->prev = node;
    if (node->next == NULL) {
        new_node->next = NULL; //NOT ALWAYS NECESSARY
        list->lastNode = new_node;
    } else {
        new_node->next = node->next;
        node->next->prev = new_node;
    }
    node->next = new_node;
    list->len++;
    return 0;
}

LIST_LINKAGE
int
LIST_insertBefore(LIST_NAME* list, LIST_ITEM_NAME* node, LIST_ITEM_NAME* new_node)
{
    if (node == NULL) {
        return -1;
    }
    if (new_node == NULL) {
        return -1;
    }
    new_node->next = node;
    if (node->prev == NULL) {
        new_node->prev = NULL; //NOT ALWAYS NECESSARY
        list->firstNode = new_node;
    } else {
        new_node->prev = node->prev;
        node->next->prev = new_node;
    }
    node->prev = new_node;
    list->len++;
    return 0;
}

LIST_LINKAGE
int
LIST_insertBeginning(LIST_NAME* list, LIST_ITEM_NAME* node)
{
    if (node == NULL) {
        return -1;
    }
    if (list->firstNode == NULL) {
        list->firstNode = node;
        list->lastNode = node;
        list->len++;
        node->prev = NULL;
        node->next = NULL;
        return 0;
    } else {
        return LIST_insertBefore(list, list->firstNode, node);
    }
}

LIST_LINKAGE
int
LIST_insertEnd(LIST_NAME* list, LIST_ITEM_NAME* node)
{
    if (node == NULL) {
        return -1;
    }
    if (list->lastNode == NULL) {
        return LIST_insertBeginning(list,node);
    } else {
        return LIST_insertAfter(list, list->lastNode, node);
    }
}

LIST_LINKAGE
LIST_ITEM_NAME*
LIST_getNodeAt(LIST_NAME* list, int pos)
{
    if (list->len < pos) {
        return NULL;
    }
    LIST_ITEM_NAME* node = list->firstNode;

    for (int i=0; node != NULL && i < pos; i++) {
        node = node->next;
    }
    return node;
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
#undef LIST_newNode_gl
#undef LIST_newNode_kls
#undef LIST_freeNode
#undef LIST_newList_gl
#undef LIST_newList_kls
#undef LIST_removeNode_gl
#undef LIST_removeNode_kls
#undef LIST_free
#undef LIST_insertAfter
#undef LIST_insertBefore
#undef LIST_insertBeginning
#undef LIST_insertEnd
#undef LIST_getNodeAt
#ifdef LIST_DECLS_ONLY
#undef LIST_DECLS_ONLY
#endif // LIST_DECLS_ONLY
#endif // LIST_T
