// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only

typedef struct Example {
    int val;
} Example;

#include "../../src/koliseo.h"

int main(void)
{
    //Init the arena
    Koliseo* kls = kls_new(KLS_DEFAULT_SIZE);

    //Use the arena (see demo for Koliseo_Temp usage)
    Example* e = KLS_PUSH(kls,Example);
    e->val = 42;

    //Free the arena
    kls_free(kls);
    printf("Done test {\"%s\"}.\n",__FILE__);
    return 0;
}
