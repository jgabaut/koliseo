#include "../../src/koliseo.h"

int main(void) {
    Koliseo* k = kls_new(-1);

    int* p = KLS_PUSH(k, p);

    kls_free(k);
    printf("Unreachable?\n");
    return 0;
}
