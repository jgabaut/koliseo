// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
#include "../../src/koliseo.h"

int main(void) {
    const char* filepath = "./LICENSE";

    int res = 0;

    Koliseo * k = kls_new(KLS_DEFAULT_SIZE*4);
    Kstr kstr = KSTR("Hello");
    Kstr cstr = kstr_from_c_lit("Hello");
    Kstr lstr = kstr_new("hello",5);
    printf(Kstr_Fmt "\n", Kstr_Arg(kstr));
    printf(Kstr_Fmt "\n", Kstr_Arg(cstr));
    printf(Kstr_Fmt "\n", Kstr_Arg(lstr));
    assert(kstr_eq(kstr,cstr) && "kstr_eq() failed.\n");
    assert(kstr_eq_ignorecase(kstr,lstr) && "kstr_eq_ignorecase() failed.\n");
    assert(!kstr_eq(cstr,lstr) && "kstr_eq() failed.\n");
    Kstr * kls_str = KLS_PUSH_NAMED(k,Kstr, 1, "Kstr*", "Kstr* for test");
    const char* line = "  Hello, World  \t!\n42\n";
    *kls_str = kstr_from_c_lit(line);
    Kstr data = *kls_str;
    printf("Orig: {" Kstr_Fmt "}\n", Kstr_Arg(data));
    Kstr trimmed = kstr_trim(data);
    printf("Trimmed: {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));
    printf("Orig after trim: " Kstr_Fmt "\n", Kstr_Arg(data));
    Kstr cut_l = kstr_cut_l(&trimmed, 5);
    printf("Trimmed after cut_l : {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));
    printf("Cut_l : {" Kstr_Fmt "}\n", Kstr_Arg(cut_l));
    assert(kstr_eq(cut_l,KSTR("Hello")) && "kstr_eq() failed\n");
    Kstr cut_r = kstr_cut_r(&trimmed, 4);
    printf("Trimmed after cut_r : {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));
    printf("Cut_r : {" Kstr_Fmt "}\n", Kstr_Arg(cut_r));
    assert(kstr_eq(cut_r,KSTR("!\n42")) && "kstr_eq() failed\n");
    printf("Orig after cuts: " Kstr_Fmt "\n", Kstr_Arg(data));
    Kstr retrimmed = kstr_trim(trimmed);
    printf("Retrimmed: {" Kstr_Fmt "}\n", Kstr_Arg(retrimmed));
    printf("Trimmed after 2nd trim: " Kstr_Fmt "\n", Kstr_Arg(trimmed));
    printf("Orig after retrim: " Kstr_Fmt "\n", Kstr_Arg(data));
    kls_free(k);
    return res;
}
