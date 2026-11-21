// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only

#include "../../src/koliseo.h"

int main(void)
{
    // Init the arena
    Koliseo* kls = kls_new(sizeof(Koliseo)+96*sizeof(char));
    kls->conf.growable = 1;

    // Allocate most of the arena first
    void *filler = KLS_PUSH_ARR(kls, char, 80);
    assert(filler != NULL);

    // This must be the last allocation
    int *values = KLS_PUSH_ARR(kls, int, 2);
    assert(values != NULL);

    values[0] = 42;
    values[1] = 84;

    // Request more space than remains in the original arena
    int *grown = kls_repush(kls, values, sizeof(int),
                            _Alignof(int), 2, 64);

    assert(grown != NULL);
    assert(grown[0] == 42);
    assert(grown[1] == 84);

    for (int i = 2; i < 64; ++i) {
        grown[i] = i * 10;
    }

    // Free the arena
    kls_free(kls);
    printf("Done test {\"%s\"}.\n",__FILE__);
    return 0;
}
