#include "../../src/koliseo.h"

int main(void) {

    KLS_Conf kls_config = {
        .kls_autoset_regions = 1,
        .kls_reglist_alloc_backend = KLS_REGLIST_ALLOC_KLS_BASIC,
        .kls_reglist_kls_size = KLS_DEFAULT_SIZE,
        .kls_autoset_temp_regions = 1,
        .kls_collect_stats = 1,
    };

    Koliseo* kls = kls_new_conf(KLS_DEFAULT_SIZE,kls_config);
    kls->conf.kls_verbose_lvl = 1;

    while(true) {
        int* dummy = KLS_PUSH(kls,int,1);
    }
    kls_free(kls);
    return 0;
}
