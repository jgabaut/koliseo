// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
#include <koliseo.h>
#define LIST_T int
#define LIST_NAME IntList
#include "../templates/list.h"

int main(void) {
    printf("KLS API: v%s\n", string_koliseo_version());
    kls_dbg_features();

    Koliseo* kls = NULL;

    kls = kls_new(KLS_DEFAULT_SIZE);

    IntList il = IntList_nullList();

    int one = 1;
    int two = 2;
    int three = 3;

    il = IntList_cons_kls(kls, &one, il);
    il = IntList_cons_kls(kls, &two, il);
    il = IntList_cons_kls(kls, &three, il);

    IntList* il_p = &il;

    while (! IntList_isEmpty(*il_p)) {
        int* node_pt = IntList_head(*il_p);

        printf("Node: {%i}\n", (*node_pt));
        *il_p = IntList_tail(*il_p);
    }

    kls_free(kls);
    return 0;
}
