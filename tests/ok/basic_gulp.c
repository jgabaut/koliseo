// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
#include "../../src/koliseo.h"
#define KLS_GULP_IMPLEMENTATION
#include "../../src/gulp.h"

int main(void) {
    const char* filepath = "./LICENSE";

    Koliseo* k = kls_new(KLS_DEFAULT_SIZE*4);
    char * gulped = NULL;
    gulped = KLS_GULP_FILE(k, filepath);
    if (gulped != NULL) {
        printf("[Gulped file as C string]\n");
        //printf("File: {\n\"%s\"\n}\n", gulped);
    } else {
        fprintf(stderr, "%s():  KLS_GULP_FILE() failed.\n",__func__);
    }
    kls_free(k);
    return 0;
}
