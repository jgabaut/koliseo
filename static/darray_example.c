#include <assert.h>
#include "koliseo.h"
#define DARRAY_T int
#include "darray.h"
#define DARRAY_T char*
#define DARRAY_NAME darray_str
#include "darray.h"


int main(int argc, char** argv)
{
    Koliseo* kls = kls_new(KLS_DEFAULT_SIZE);
    darray_int* darray = darray_int_init(kls);

    darray_int_push(darray, 1);
    assert(darray->capacity == 4);

    for (int i = 0; i < 10; i++) {
        darray_int_push(darray, 42);
    }
    assert(darray->capacity = 16);
    assert(darray->count = 11);

    for (int i = 0; i < darray->count; i++) {
        printf("{#%i: %i}\n", i, darray->items[i]);
    }

    darray_str* ds = darray_str_init(kls);
    char* foo = KLS_PUSH_STR(kls, "FOO");
    memcpy(foo, "FOO", strlen("FOO"));
    darray_str_push(ds, foo);

    char* bar = "bar";
    darray_str_push(ds, bar);

    for (int i = 0; i < ds->count; i++) {
        printf("{#%i: %s}\n", i, ds->items[i]);
    }


    kls_free(kls);
    return 0;
}
