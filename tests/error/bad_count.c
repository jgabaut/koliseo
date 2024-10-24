#include "../../src/koliseo.h"

int main(void) {
    Koliseo* k = kls_new(KLS_DEFAULT_SIZE);

    int* p = KLS_PUSH_ARR(k,int,0);
    kls_free(k);
    printf("Unreachable?\n");
    return 0;
}
