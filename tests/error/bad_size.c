#include "../../src/koliseo.h"

int main(void) {
    KOLISEO_DEBUG = 0; //Important. On crashes like this, as of 0.2.6, the KOLISEO_DEBUG_FP won't be closed properly.
    Koliseo* k = kls_new(-1);

    int* p = KLS_PUSH(k,int,1);

    kls_free(k);
    printf("Unreachable?\n");
    return 0;
}
