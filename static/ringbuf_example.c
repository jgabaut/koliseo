#include <assert.h>
#include "koliseo.h"
#define RINGBUF_T int
#include "ringbuf.h"
int main(int argc, char** argv)
{
    Koliseo* kls = kls_new_dbg(KLS_DEFAULT_SIZE);
    ringbuf_int* rb = ringbuf_int_init(kls, 4);
    ringbuf_int_push(rb, 1);
    ringbuf_int_push(rb, 2);
    ringbuf_int_push(rb, 3);
    ringbuf_int_push(rb, 4);
    // Ringbuf is now full
    for (size_t i = 0; i < 4; i++) {
        bool get_res = false;
        int* val = ringbuf_int_getelem_by_index(rb, i, &get_res);
        if (!get_res) {
            fprintf(stderr, "%s():    failed getelem_by_index()\n", __func__);
            kls_free(kls);
            return -1;
        }
#ifndef _WIN32
        printf("Pos %li: val (%i)\n", i, *val);
#else
        printf("Pos %lli: val (%i)\n", i, *val);
#endif
    }
    printf("Overriding one:\n");
    ringbuf_int_push(rb, 5);
    for (size_t i = ringbuf_int_get_head(rb); i < ringbuf_int_get_capacity(rb); i++) {
        bool get_res = false;
        int* val = ringbuf_int_getelem_by_index(rb, i, &get_res);
        if (!get_res) {
            fprintf(stderr, "%s():    failed getelem_by_index()\n", __func__);
            kls_free(kls);
            return -1;
        }
#ifndef _WIN32
        printf("Pos %li: val (%i)\n", i, *val);
#else
        printf("Pos %lli: val (%i)\n", i, *val);
#endif
    }
    for (size_t i = 0; i < ringbuf_int_get_head(rb); i++) {
        bool get_res = false;
        int* val = ringbuf_int_getelem_by_index(rb, i, &get_res);
        if (!get_res) {
            fprintf(stderr, "%s():    failed getelem_by_index()\n", __func__);
            kls_free(kls);
            return -1;
        }
#ifndef _WIN32
        printf("Pos %li: val (%i)\n", i, *val);
#else
        printf("Pos %lli: val (%i)\n", i, *val);
#endif
    }
    kls_free(kls);
    return 0;
}
