#include "../../src/koliseo.h"

int main(void) {
    Koliseo* k = kls_new(KLS_DEFAULT_SIZE);

    ptrdiff_t size = -1;
    ptrdiff_t align = _Alignof(int);
    ptrdiff_t count = 1;
    int* p = kls_push_zero(k, size, align, count);
    assert(p == NULL); // Count was < 0
    kls_free(k);
    return 0;
}
