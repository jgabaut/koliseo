typedef struct Example {
    int val;
} Example;

#include "../src/koliseo.h"

int main(void)
{
    //Init the arena and provide an optional file path for internal kls logging.
    //If KLS_DEBUG_CORE is not defined for the library, a warning will be displayed and no logging is done.
    Koliseo* kls = kls_new_traced(KLS_DEFAULT_SIZE,"./prova.txt");

    //Use the arena (see demo for Koliseo_Temp usage)
    Example* e = KLS_PUSH(kls,Example,1);
    e->val = 42;

    //Show contents to stdout
    print_dbg_kls(kls);

    //Free the arena
    kls_free(kls);
    return 0;
}
