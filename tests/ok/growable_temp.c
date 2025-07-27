// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only

typedef struct Example {
    int val;
} Example;

#include "../../src/koliseo.h"

int main(void)
{
    //Init the arena
    Koliseo* kls = kls_new(500);
    kls->conf.kls_growable = 1;
    Koliseo_Temp* kls_t = kls_temp_start(kls);

    //Use the arena (see demo for Koliseo_Temp usage)
    Example* e = NULL;
    for(int i = 0; i < 500; i++) {
        e = KLS_PUSH_T(kls_t,Example);
    }
    e->val = 42;

    kls_temp_end(kls_t);

    //Free the arena
    kls_free(kls);
    printf("Done test {\"%s\"}.\n",__FILE__);
    return 0;
}
