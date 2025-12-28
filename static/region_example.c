// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
#include "kls_region.h"

int main(void) {
    printf("KLS API: v%s\n", string_koliseo_version());
    kls_dbg_features();

    Koliseo* kls = kls_new(KLS_DEFAULT_SIZE);

    printf("kls->extension_handlers.on_push_handler == {%p}\n", kls->hooks[KLS_AUTOREGION_EXT_SLOT].on_push_handler);
    printf("KLS_DEFAULT_EXTENSION_HANDLERS.on_push_handler == {%p}\n", KLS_DEFAULT_HOOKS.on_push_handler);
    int* foo = KLS_PUSH(kls, int);

    *foo = 42;

    int* bar = KLS_PUSH(kls, int);
    printf("foo: {%i}\n", *foo);

    KLS_Autoregion_Extension_Data* data_pt = (KLS_Autoregion_Extension_Data*) kls->extension_data[KLS_AUTOREGION_EXT_SLOT];

    printf("Region list length: {%i}\n", kls_rl_length(data_pt->regs));
    kls_rl_showList(data_pt->regs);

    kls_free(kls);
    return 0;
}
