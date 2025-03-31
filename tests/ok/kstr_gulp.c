// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
#include "../../src/koliseo.h"
#define KLS_GULP_IMPLEMENTATION
#include "../../src/kls_gulp.h"

int main(void) {
    const char* filepath = "./LICENSE";

    Koliseo* k = kls_new(KLS_DEFAULT_SIZE*4);
    Kstr * gulped = NULL;
    gulped = KLS_GULP_FILE_KSTR(k, filepath);
    if (gulped != NULL) {
        printf("[Gulped file as Kstr]\n");
        printf("File: {\n\"%s\"\n}\n", gulped->data);
    } else {
        fprintf(stderr, "%s():  KLS_GULP_FILE_KSTR() failed.\n",__func__);
    }
    kls_free(k);
    return 0;
}
