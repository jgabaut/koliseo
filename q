[1mdiff --git a/src/koliseo.c b/src/koliseo.c[m
[1mindex 412286e..4bc5be2 100644[m
[1m--- a/src/koliseo.c[m
[1m+++ b/src/koliseo.c[m
[36m@@ -3383,6 +3383,35 @@[m [mKstr kstr_trim(Kstr kstr)[m
     return kstr_trim_left(kstr_trim_right(kstr));[m
 }[m
 [m
[32m+[m[32mKstr kstr_token_kstr(Kstr* k, Kstr delim) {[m
[32m+[m
[32m+[m[32m    //Kstr to scroll k data, sized as the delimiter[m
[32m+[m[32m    Kstr win = kstr_new(k->data, delim.len);[m
[32m+[m
[32m+[m[32m    size_t i = 0;[m
[32m+[m
[32m+[m[32m    //Loop checking if k data can still be scrolled and if current window is equal to the delimiter[m
[32m+[m[32m    while (i + delim.len < k->len &&[m
[32m+[m[32m            !(kstr_eq(win, delim))) {[m
[32m+[m[32m        i++;[m
[32m+[m[32m        win.data++;[m
[32m+[m[32m    }[m
[32m+[m
[32m+[m[32m    //New Kstr just up to the delimiter position[m
[32m+[m[32m    Kstr res = kstr_new(k->data, i);[m
[32m+[m
[32m+[m[32m    //If we cleanly empty k, we increase result len so that it holds the delimiter in[m
[32m+[m[32m    if (i + delim.len == k->len) {[m
[32m+[m[32m        res.len += delim.len;[m
[32m+[m[32m    }[m
[32m+[m
[32m+[m[32m    //Advance k by the delimiter size, plus its starting position[m
[32m+[m[32m    k->data += i + delim.len;[m
[32m+[m[32m    k->len += i + delim.len;[m
[32m+[m
[32m+[m[32m    return res;[m
[32m+[m[32m}[m
[32m+[m
 static char * kls_read_file(Koliseo* kls, const char * f_name, Gulp_Res * err, size_t * f_size, ...)[m
 {[m
     if (!kls) {[m
[1mdiff --git a/src/koliseo.h b/src/koliseo.h[m
[1mindex f23c141..a29aed0 100644[m
[1m--- a/src/koliseo.h[m
[1m+++ b/src/koliseo.h[m
[36m@@ -474,6 +474,7 @@[m [mKstr kstr_cut_r(Kstr *k, size_t n);[m
 Kstr kstr_trim_left(Kstr kstr);[m
 Kstr kstr_trim_right(Kstr kstr);[m
 Kstr kstr_trim(Kstr kstr);[m
[32m+[m[32mKstr kstr_token_kstr(Kstr* k, Kstr delim);[m
 [m
 #define KSTR(c_lit) kstr_new(c_lit, sizeof(c_lit) - 1)[m
 #define KSTR_NULL kstr_new(NULL, 0)[m
[1mdiff --git a/tests/ok/kstr_test.c b/tests/ok/kstr_test.c[m
[1mindex c477394..6f2bdf2 100644[m
[1m--- a/tests/ok/kstr_test.c[m
[1m+++ b/tests/ok/kstr_test.c[m
[36m@@ -21,23 +21,34 @@[m [mint main(void) {[m
     const char* line = "  Hello, World  \t!\n42\n";[m
     *kls_str = kstr_from_c_lit(line);[m
     Kstr data = *kls_str;[m
[31m-    printf("Orig: {" Kstr_Fmt "}\n", Kstr_Arg(data));[m
[32m+[m[32m    printf("\n\nOrig: {" Kstr_Fmt "}\n", Kstr_Arg(data));[m
     Kstr trimmed = kstr_trim(data);[m
[31m-    printf("Trimmed: {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));[m
[31m-    printf("Orig after trim: " Kstr_Fmt "\n", Kstr_Arg(data));[m
[32m+[m[32m    printf("\n\nTrimmed: {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));[m
[32m+[m[32m    printf("Orig after trim: {" Kstr_Fmt "}\n", Kstr_Arg(data));[m
     Kstr cut_l = kstr_cut_l(&trimmed, 5);[m
[31m-    printf("Trimmed after cut_l : {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));[m
[32m+[m[32m    printf("\n\nTrimmed after cut_l : {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));[m
     printf("Cut_l : {" Kstr_Fmt "}\n", Kstr_Arg(cut_l));[m
     assert(kstr_eq(cut_l,KSTR("Hello")) && "kstr_eq() failed\n");[m
     Kstr cut_r = kstr_cut_r(&trimmed, 4);[m
[31m-    printf("Trimmed after cut_r : {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));[m
[32m+[m[32m    printf("\n\nTrimmed after cut_r : {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));[m
     printf("Cut_r : {" Kstr_Fmt "}\n", Kstr_Arg(cut_r));[m
     assert(kstr_eq(cut_r,KSTR("!\n42")) && "kstr_eq() failed\n");[m
[31m-    printf("Orig after cuts: " Kstr_Fmt "\n", Kstr_Arg(data));[m
[32m+[m[32m    printf("Orig after cuts: {" Kstr_Fmt "}\n", Kstr_Arg(data));[m
     Kstr retrimmed = kstr_trim(trimmed);[m
[31m-    printf("Retrimmed: {" Kstr_Fmt "}\n", Kstr_Arg(retrimmed));[m
[31m-    printf("Trimmed after 2nd trim: " Kstr_Fmt "\n", Kstr_Arg(trimmed));[m
[31m-    printf("Orig after retrim: " Kstr_Fmt "\n", Kstr_Arg(data));[m
[32m+[m[32m    printf("\n\nRetrimmed: {" Kstr_Fmt "}\n", Kstr_Arg(retrimmed));[m
[32m+[m[32m    printf("Trimmed after 2nd trim: {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));[m
[32m+[m[32m    printf("Orig after retrim: {" Kstr_Fmt "}\n", Kstr_Arg(data));[m
[32m+[m[32m    Kstr tokenised = kstr_token_kstr(&retrimmed,KSTR(", "));[m
[32m+[m[32m    printf("\n\nRetrimmed after token: {" Kstr_Fmt "}\n", Kstr_Arg(retrimmed));[m
[32m+[m[32m    printf("Trimmed after token: {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));[m
[32m+[m[32m    printf("Orig after token: {" Kstr_Fmt "}\n", Kstr_Arg(data));[m
[32m+[m[32m    printf("Token: {" Kstr_Fmt "}\n", Kstr_Arg(tokenised));[m
[32m+[m[32m    Kstr retokenised = kstr_token_kstr(&retrimmed,KSTR("!"));[m
[32m+[m[32m    printf("\n\nRetrimmed after retoken: {" Kstr_Fmt "}\n", Kstr_Arg(retrimmed));[m
[32m+[m[32m    printf("Trimmed after retoken: {" Kstr_Fmt "}\n", Kstr_Arg(trimmed));[m
[32m+[m[32m    printf("Orig after retoken: {" Kstr_Fmt "}\n", Kstr_Arg(data));[m
[32m+[m[32m    printf("Retoken: {" Kstr_Fmt "}\n", Kstr_Arg(retokenised));[m
[32m+[m[32m    assert(retokenised.len != retrimmed.len && "kstr_token_kstr() failed\n");[m
     kls_free(k);[m
     return res;[m
 }[m
