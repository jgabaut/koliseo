// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
#include "../../src/koliseo.h"

int main(void) {
    Koliseo* k = kls_new(1024*1024*1024);

    size_t huge = (size_t)PTRDIFF_MAX + 1;
    char buf[2000] = {0};
    kls_formatSize(PTRDIFF_MAX, buf, 2000);

    printf("Biggest single allocation is {%s} bytes.\n", buf);

    kls_formatSize(SIZE_MAX, buf, 2000);
    printf("SIZE_MAX is {%s} bytes.\n", buf);

    kls_push_zero_ext(
        k,
        (ptrdiff_t)huge,
        1,
        1
    );
    kls_free(k);
    return 0;
}
