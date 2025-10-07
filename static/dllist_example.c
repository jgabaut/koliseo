// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
#include <koliseo.h>

#define LIST_T int
#define LIST_NAME IntList
#include "../templates/dllist.h"


int main(void) {
    printf("KLS API: v%s\n", string_koliseo_version());
    kls_dbg_features();

    Koliseo* kls = NULL;

    kls = kls_new(KLS_DEFAULT_SIZE);

    IntList* il = IntList_newList_kls(kls);
    int two = 2;
    int_item* node = IntList_newNode_kls(kls, &two);
    int four = 4;
    int_item* node_2 = IntList_newNode_kls(kls, &four);
    int zero = 0;
    int_item* node_3 = IntList_newNode_kls(kls, &zero);
    IntList_insertBeginning(il, node_2);
    IntList_insertEnd(il, node);
    IntList_insertAfter(il, node, node_3);
    int_item* iter = il->firstNode;

    while(iter) {
        printf("{%i}\n", *(iter->data));
        iter = iter->next;
    }

    int_item* n = IntList_getNodeAt(il, 1);

    printf("Node->data at #1: {%i}\n", *(n->data));

    IntList* il_2 = IntList_newList_gl();
    int_item* gl_node = IntList_newNode_gl(&two);
    int_item* gl_node_2 = IntList_newNode_gl(&four);
    int_item* gl_node_3 = IntList_newNode_gl(&zero);

    IntList_insertEnd(il_2, gl_node);
    IntList_insertBefore(il_2, gl_node, gl_node_2);
    IntList_insertEnd(il_2, gl_node_3);
    IntList_removeNode_gl(il_2, gl_node_3);

    iter = il_2->firstNode;

    while(iter) {
        printf("{%i}\n", *(iter->data));
        iter = iter->next;
    }

    IntList_free(il_2);

    kls_free(kls);
    return 0;
}
