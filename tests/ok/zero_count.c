#include "../../src/koliseo.h"

int main(void) {
    KLS_Conf conf = KLS_DEFAULT_CONF;
    conf.allow_zerocount_push = 1;

    Koliseo* k = kls_new_conf(KLS_DEFAULT_SIZE, conf);

    double* p = KLS_PUSH(k, double);

    ptrdiff_t count = 0;
    int* p2 = KLS_PUSH_ARR(k,int,count);
    assert(p2 != NULL); // Count was == 0, but we still get an aligned pointer into the Koliseo.
    int* p3 = KLS_PUSH(k, int);
    ptrdiff_t diff_to_zero = (char*) p2 - (char*) p;
    ptrdiff_t diff_after_zero = p3 - p2;
    printf("Diff to zero: %td, diff after zero: %td\n", diff_to_zero, diff_after_zero);
    kls_free(k);
    return 0;
}
