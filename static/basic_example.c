typedef struct Example {
    int val;
} Example;

#include "../src/koliseo.h"

int main(void)
{
    //Init the arena
    Koliseo* kls = kls_new(KLS_DEFAULT_SIZE);

    //Use the arena (see demo for Koliseo_Temp usage)
    Example* e = KLS_PUSH(kls,Example,1);
    e->val = 42;

    //Show contents to stdout
    print_dbg_kls(kls);

    //Free the arena
    kls_free(kls);
    return 0;
}
