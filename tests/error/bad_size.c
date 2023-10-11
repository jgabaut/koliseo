#include "../../src/koliseo.h"

int main(void) {
    KOLISEO_DEBUG = 1;
    KOLISEO_DEBUG_FP = fopen("./static/debug_log.txt","w");
    if (!KOLISEO_DEBUG_FP) {
        fprintf(stderr,"Error at %s(), file {%s}: failed opening KOLISEO_DEBUG_FP.\n", __func__, __FILE__);
        return 1;
    }
    Koliseo* k = kls_new(-1);

    int* p = KLS_PUSH(k,int,1);

    kls_free(k);
    printf("Unreachable?\n");
    return 0;
}
