#include "../../src/koliseo.h"

int main(void) {
    Koliseo* k = kls_new(KLS_DEFAULT_SIZE);

    int* p = KLS_PUSH_ARR(k,int,-1);
    assert(p == NULL); // Count was < 0
    kls_free(k);
    return 0;
}
