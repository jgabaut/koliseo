#include "../../src/koliseo.h"

int main(void) {

    Koliseo* kls = kls_new_traced_AR_KLS(KLS_DEFAULT_SIZE,"./static/debug_log.txt",KLS_DEFAULT_SIZE);

    while(true) {
        int* dummy = KLS_PUSH(kls,int,1);
    }
    kls_free(kls);
    return 0;
}
