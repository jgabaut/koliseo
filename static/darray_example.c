#include <assert.h>
#include "koliseo.h"
#define DARRAY_T int
#include "darray.h"
#define DARRAY_T char*
#define DARRAY_NAME darray_str
#include "darray.h"

int longest_string(const char** a, const char** b) {
    int l_a = strlen(*a);
    int l_b = strlen(*b);
    return l_b - l_a;
}

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

    char* foobar = "foobar";
    darray_str_push(ds, foobar);

    for (int i = 0; i < ds->count; i++) {
        printf("{#%i: %s}\n", i, ds->items[i]);
    }

    printf("Sorting based on length (longest first):\n");

    darray_str_sort(ds, longest_string);

    for (int i = 0; i < ds->count; i++) {
        printf("{#%i: %s}\n", i, ds->items[i]);
    }

    Koliseo_Temp* t_kls = kls_temp_start(kls);

    darray_str* ds_t = darray_str_init_t(t_kls);

    darray_str_push_t(ds_t, "MEOW");

    for (int i = 0; i < ds_t->count; i++) {
        printf("{#%i: %s}\n", i, ds_t->items[i]);
    }

    kls_temp_end(t_kls);

    kls_free(kls);
    return 0;
}
