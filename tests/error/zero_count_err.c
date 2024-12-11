#include "../../src/koliseo.h"

int main(void) {
    KLS_Conf conf = KLS_DEFAULT_CONF;
    //Missing this line causes the wanted error
    //conf.kls_allow_zerocount_push = 1;

    Koliseo* k = kls_new_conf(KLS_DEFAULT_SIZE, conf);

    double* p = KLS_PUSH(k, double);

    ptrdiff_t count = 0;
    int* p2 = KLS_PUSH_ARR(k,int,count);
    assert(p2 == NULL); // Count was == 0, we get a NULL pointer here.
    kls_free(k);
    return 0;
}
