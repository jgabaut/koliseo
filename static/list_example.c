// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
#include <koliseo.h>

bool my_cmp(const int* a, const int* b);
#define LIST_T int
#define LIST_NAME IntList
#define LIST_CMP_DEFAULT_FN &my_cmp
#include "../templates/list.h"


IntList_cmp *myfunc = &my_cmp;

bool my_cmp(const int* a, const int* b)
{
    printf("%s: %i == %i?\n", __func__, *a, *b);
    return (*a == *b);
}

bool my_cmp_bar(const int* a, const int* b);

bool my_cmp_bar(const int* a, const int* b)
{
    printf("%s: %i == %i?\n", __func__, *a, *b);
    return (*a == *b);
}


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

    int four = 3;
    IntList_cmp *myfunc_bar = &my_cmp_bar;
    if (IntList_member_fn(&four, il, myfunc_bar)) {
        printf("%i: MEMBER\n", four);
    } else {
        printf("%i: NOT MEMBER\n", four);
    }

    int five = 1;
    //Using default comparator
    if (IntList_member(&five, il)) {
        printf("%i: MEMBER\n", five);
    } else {
        printf("%i: NOT MEMBER\n", five);
    }


    while (! IntList_isEmpty(*il_p)) {
        int* node_pt = IntList_head(*il_p);

        printf("Node: {%i}\n", (*node_pt));
        *il_p = IntList_tail(*il_p);
    }

    kls_free(kls);
    return 0;
}
