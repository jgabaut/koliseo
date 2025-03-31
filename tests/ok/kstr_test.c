// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
#include "../../src/koliseo.h"
#define KLS_GULP_IMPLEMENTATION
#include "../../src/kls_gulp.h"

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
    Kstr * kls_str = KLS_PUSH_NAMED(k,Kstr, "Kstr*", "Kstr* for test");
    const char* line = "  Hello, World  test !  \n42\n";
    *kls_str = kstr_from_c_lit(line);
    Kstr data = *kls_str;
    printf("\n\nOrig: {" Kstr_Fmt "}\n", Kstr_Arg(data));
    Kstr trimmed = kstr_trim(data);
    printf("\n\nTrimmed: {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));
    printf("Orig after trim: {" Kstr_Fmt "}\n", Kstr_Arg(data));
    Kstr cut_l = kstr_cut_l(&trimmed, 5);
    printf("\n\nTrimmed after cut_l : {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));
    printf("Cut_l : {" Kstr_Fmt "}\n", Kstr_Arg(cut_l));
    assert(kstr_eq(cut_l,KSTR("Hello")) && "kstr_eq() failed\n");
    Kstr cut_r = kstr_cut_r(&trimmed, 4);
    printf("\n\nTrimmed after cut_r : {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));
    printf("Cut_r : {" Kstr_Fmt "}\n", Kstr_Arg(cut_r));
    assert(kstr_eq(cut_r,KSTR(" \n42")) && "kstr_eq() failed\n");
    printf("Orig after cuts: {" Kstr_Fmt "}\n", Kstr_Arg(data));
    Kstr retrimmed = kstr_trim(trimmed);
    printf("\n\nRetrimmed: {" Kstr_Fmt "}\n", Kstr_Arg(retrimmed));
    printf("Orig after retrim: {" Kstr_Fmt "}\n", Kstr_Arg(data));
    Kstr tokenised = kstr_token_kstr(&retrimmed,KSTR(", "));
    printf("\n\nRetrimmed after token: {" Kstr_Fmt "}\n", Kstr_Arg(retrimmed));
    printf("Orig after token: {" Kstr_Fmt "}\n", Kstr_Arg(data));
    printf("Token: {" Kstr_Fmt "}\n", Kstr_Arg(tokenised));
    Kstr retokenised = KSTR_NULL;
    bool token_res = kstr_try_token(&retrimmed,'!',&retokenised);
    printf("Res: %s\n", ( token_res ? "TRUE" : "FALSE"));
    printf("\n\nRetrimmed after retoken: {" Kstr_Fmt "}\n", Kstr_Arg(retrimmed));
    printf("Orig after retoken: {" Kstr_Fmt "}\n", Kstr_Arg(data));
    printf("Retoken: {" Kstr_Fmt "}\n", Kstr_Arg(retokenised));
    printf("Retoken len: %zu\n", retokenised.len);
    printf("Retrim len: %zu\n", retrimmed.len);
    kls_free(k);
    return res;
}
