#include "../../src/kls_region.h"

int main(void) {

    Koliseo* kls = kls_new_traced_AR_KLS(KLS_DEFAULT_SIZE,"./static/debug_log.txt",KLS_DEFAULT_SIZE);

    Koliseo_Temp* t_kls = kls_temp_start(kls);
    int iter=1;
    while(true) {
        int* dummy = KLS_PUSH_T(t_kls,int);
    }
    kls_free(kls);
    return 0;
}
