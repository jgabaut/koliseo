#include "../../src/koliseo.h"

int main(void) {
    size_t size = -1;
    Koliseo* k = kls_new(size);

    int* p = KLS_PUSH(k,int);

    kls_free(k);
    printf("Unreachable?\n");
    return 0;
}
