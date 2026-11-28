#include "../../src/koliseo.h"

int main(void) {
    size_t size = -1;
    Koliseo* k = kls_new(size);

    assert(!k);

    int* p = KLS_PUSH(k,int);

    if (!p) {
        return 1;
    }

    printf("Unreachable?\n");

    if (!kls_free(k)) {
        return 2;
    }
    return 0;
}
