#include "../../src/kls_region.h"

int main(void) {

    Koliseo* kls = kls_new_alloc_ext(KLS_DEFAULT_SIZE, KLS_DEFAULT_ALLOCF, KLS_DEFAULT_FREEF, KLS_DEFAULT_HOOKS, KLS_DEFAULT_EXTENSION_DATA);

    int iter=1;
    while(true) {
        int* dummy = KLS_PUSH_TYPED(kls,int, KLS_None, "foo", "bar");
    }
    kls_free(kls);
    return 0;
}
