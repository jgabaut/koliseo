// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
#include "../../src/koliseo.h"

int main(void) {
    Koliseo* k = kls_new(1024*1024*1024);

    int* p = KLS_PUSH(k, p);

    kls_free(k);
    printf("That's a big Koliseo.\n");
    return 0;
}
