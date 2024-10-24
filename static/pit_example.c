#define KLS_PIT_IMPLEMENTATION
#include "../templates/pit.h"

int main(void) {

    KLS_Pit pit = kls_new_pit(sizeof(Koliseo)*2);
    KLS_Pit* p = &pit;
    printf("Hi\n");

    int iters = 20000;
    int* voider = NULL;
    for (int i=0; i < iters; i++) {
        voider = KLS_PIT_PUSH(p, int);
        *voider = 42069;
    }
    printf("%d\n", *voider);
    kls_pit_free(p);
    return 0;
}
