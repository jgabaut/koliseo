// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
/*
    Copyright (C) 2023-2025  jgabaut

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef KLS_GULP_H_
#define KLS_GULP_H_
#ifndef KOLISEO_H_
#include "koliseo.h"
#endif // KOLISEO_H_

#include "ctype.h" // Needed for isspace()...

typedef struct Kstr {
    const char* data;
    size_t len;
} Kstr;

Kstr kstr_new(const char* str, size_t len);
Kstr kstr_from_c_lit(const char* c_lit);
bool kstr_eq(Kstr left, Kstr right);
bool kstr_eq_ignorecase(Kstr left, Kstr right);
Kstr kstr_cut_l(Kstr *k, size_t n);
Kstr kstr_cut_r(Kstr *k, size_t n);
Kstr kstr_trim_left(Kstr kstr);
Kstr kstr_trim_right(Kstr kstr);
Kstr kstr_trim(Kstr kstr);
bool kstr_indexof(Kstr k, char c, int* idx);
Kstr kstr_token(Kstr* k, char delim);
bool kstr_try_token(Kstr* k, char delim, Kstr* part);
Kstr kstr_token_kstr(Kstr* k, Kstr delim);

#define KSTR(c_lit) kstr_new(c_lit, sizeof(c_lit) - 1)
#define KSTR_NULL kstr_new(NULL, 0)

/**
 * Format macro for a Kstr.
 */
#define Kstr_Fmt "%.*s"
/**
 * Format matching macro for a Kstr.
 */
#define Kstr_Arg(kstr) (int) (kstr.len), (kstr.data)

/**
 * Defines a one GB size as decimal integer representation.
 * @see GULP_MAX_FILE_SIZE
 * @see GULP_FILE_TOO_LARGE
 * @see kls_gulp_file_sized()
 * @see KLS_GULP_FILE()
 */
#define ONEGB_DEC_INT 1073741824

/**
 * Defines max size allowed for a file passed to KLS_GULP_FILE().
 * Use try_kls_gulp_file() if you want a different upper limit.
 */
#define GULP_MAX_FILE_SIZE ONEGB_DEC_INT

/**
 * Defines possible results for kls_gulp_file_sized().
 * @see kls_gulp_file_sized().
 */
typedef enum Gulp_Res {
    GULP_FILE_OK=0,
    GULP_FILE_NOT_EXIST,
    GULP_FILE_TOO_LARGE,
    GULP_FILE_READ_ERROR,
    GULP_FILE_CONTAINS_NULLCHAR,
    GULP_FILE_KLS_NULL,
    TOT_GULP_RES
} Gulp_Res;

/**
 * Format macro for a Gulp_Res.
 */
#define Gulp_Res_Fmt "%s"
/**
 * Format matching macro for a Gulp_Res.
 */
#define Gulp_Res_Arg(gr) (string_from_Gulp_Res((gr)))

/**
 * String array for representations of Gulp_Res.
 * @see string_from_Gulp_Res()
 */
extern const char* gulp_res_names[TOT_GULP_RES+1];
const char* string_from_Gulp_Res(Gulp_Res g);

//static char * kls_read_file(Koliseo* kls, const char * f_name, Gulp_Res * err, size_t * f_size, ...);
char * kls_gulp_file_sized(Koliseo* kls, const char * filepath, Gulp_Res * err, size_t max_size);
char * try_kls_gulp_file(Koliseo* kls, const char * filepath, size_t max_size);
#define KLS_GULP_FILE(kls, filepath) try_kls_gulp_file((kls),(filepath), GULP_MAX_FILE_SIZE)
//Kstr * kls_read_file_to_kstr(Koliseo* kls, const char * f_name, Gulp_Res * err, size_t * f_size, ...);
Kstr * kls_gulp_file_sized_to_kstr(Koliseo* kls, const char * filepath, Gulp_Res * err, size_t max_size, bool allow_nullchar);
Kstr * try_kls_gulp_file_to_kstr(Koliseo* kls, const char * filepath, size_t max_size, bool allow_nullchar);
#define KLS_GULP_FILE_KSTR(kls, filepath) try_kls_gulp_file_to_kstr((kls),(filepath), GULP_MAX_FILE_SIZE, false)

#endif // KLS_GULP_H_

#ifdef KLS_GULP_IMPLEMENTATION

/**
 * Contains the constant string representation of Gulp_Res values.
 * @see Gulp_Res
 */
const char* gulp_res_names[TOT_GULP_RES+1] = {
    [GULP_FILE_OK] = "Success",
    [GULP_FILE_NOT_EXIST] = "File does not exist",
    [GULP_FILE_TOO_LARGE] = "File is too large",
    [GULP_FILE_READ_ERROR] = "File could not be read",
    [GULP_FILE_CONTAINS_NULLCHAR] = "File contains nullchar",
    [GULP_FILE_KLS_NULL] = "Koliseo was NULL",
    [TOT_GULP_RES] = "Total of Gulp_Res values",
};

/**
 * Return a constant string for the passed Gulp_Res.
 * @see gulp_res_names
 * @see Gulp_Res
 * @param g The Gulp_Res to get a string for.
 * @return A constant string representation of passed Gulp_Res.
 */
const char* string_from_Gulp_Res(Gulp_Res g)
{
    assert(g >= 0 && g < TOT_GULP_RES && "Unexpected Gulp_Res value");
    return gulp_res_names[g];
}

/**
 * Returns a new Kstr with the passed args set.
 * @see Kstr
 * @param data The string pointer to set.
 * @param len The len to set.
 * @return The resulting Kstr.
 */
Kstr kstr_new(const char* data, size_t len)
{
    return (Kstr) {
        .data = data,
        .len = len,
    };
}

/**
 * Returns a new Kstr from the passed null-terminated string.
 * @see Kstr
 * @param c_lit The cstring pointer to set.
 * @return The resulting Kstr.
 */
Kstr kstr_from_c_lit(const char* c_lit)
{
    return kstr_new(c_lit, strlen(c_lit));
}

/**
 * Checks if the two passed Kstr have exactly equal data.
 * @see Kstr
 * @param left The first Kstr to compare.
 * @param right The second Kstr to compare.
 * @return A bool result for the comparation.
 */
bool kstr_eq(Kstr left, Kstr right)
{
    if (left.len != right.len) {
        return false;
    }

    for (size_t i=0; i < left.len; i++) {
        if (left.data[i] != right.data[i]) return false;
    }
    return true;
}

/**
 * Checks if the two passed Kstr have equal data, ignoring case.
 * @see Kstr
 * @param left The first Kstr to compare.
 * @param right The second Kstr to compare.
 * @return A bool result for the comparation.
 */
bool kstr_eq_ignorecase(Kstr left, Kstr right)
{
    if (left.len != right.len) {
        return false;
    }

    char l, r;
    for (size_t i=0; i < left.len; i++) {
        l = 'A' <= left.data[i] && 'Z' >= left.data[i]
            ? left.data[i] + 32
            : left.data[i];
        r = 'A' <= right.data[i] && 'Z' >= right.data[i]
            ? right.data[i] + 32
            : right.data[i];
        if (l != r) return false;
    }
    return true;
}

/**
 * Cuts the passed Kstr by up to n chars, from the left. Returns cut portion as a new Kstr.
 * @see Kstr
 * @param k The Kstr to cut.
 * @param n How many chars to cut.
 * @return The cut part as a new Kstr.
 */
Kstr kstr_cut_l(Kstr *k, size_t n)
{
    if (n > k->len) {
        n = k->len;
    }
    Kstr res = kstr_new(k->data, n);
    k->data += n;
    k->len -= n;

    return res;
}

/**
 * Cuts the passed Kstr by up to n chars, from the right. Returns cut portion as a new Kstr.
 * @see Kstr
 * @param k The Kstr to cut.
 * @param n How many chars to cut.
 * @return The cut part as a new Kstr.
 */
Kstr kstr_cut_r(Kstr *k, size_t n)
{
    if (n > k->len) {
        n = k->len;
    }
    Kstr res = kstr_new(k->data + k->len - n, n);
    k->len -= n;

    return res;
}

/**
 * Returns a new Kstr after removing heading spaces from the passed one.
 * @see Kstr
 * @param kstr The Kstr to trim.
 * @return The resulting Kstr.
 */
Kstr kstr_trim_left(Kstr kstr)
{
    size_t i = 0;
    while ( i < kstr.len && isspace(kstr.data[i])) {
        i++;
    }
    return kstr_new(kstr.data + i, kstr.len - i);
}

/**
 * Returns a new Kstr after removing trailing spaces from the passed one.
 * @see Kstr
 * @param kstr The Kstr to trim.
 * @return The resulting Kstr.
 */
Kstr kstr_trim_right(Kstr kstr)
{
    size_t i = 0;
    while ( i < kstr.len && isspace(kstr.data[kstr.len - i - 1])) {
        i++;
    }
    return kstr_new(kstr.data, kstr.len - i);
}

/**
 * Returns a new Kstr after removing heading and trailing spaces from the passed one.
 * @see Kstr
 * @see kstr_trim_l()
 * @see kstr_trim_r()
 * @param kstr The Kstr to trim.
 * @return The resulting Kstr.
 */
Kstr kstr_trim(Kstr kstr)
{
    return kstr_trim_left(kstr_trim_right(kstr));
}

/**
 * Checks if passed Kstr contains the passed char, and if so, sets the value pointed by idx to the first occurrence.
 * @see Kstr
 * @param k The Kstr to scan.
 * @param c The char to look for.
 * @param idx Pointer to the value to be set as index.
 * @return false if the passed Kstr doesn't contain the passed char, true otherwise.
 */
bool kstr_indexof(Kstr k, char c, int* idx)
{
    if (k.len == 0) {
        return false;
    } else {
        size_t i = 0;
        while (i < k.len) {
            if (k.data[i] == c) {
                *idx = i;
                return true;
            }

            i++;
        }
        return false;
    }
}

/**
 * Scans the first passed Kstr and if the passed char is present, the old Kstr is set to second pointer and the first one is cut at the first occurrence of it.
 * @see Kstr
 * @param k The Kstr to scan.
 * @param delim The char to look for.
 * @param part The Kstr to set to the original data, if the delimiter is found.
 * @return false if the passed Kstr doesn't contain the passed char, true otherwise.
 */
bool kstr_try_token(Kstr *k, char delim, Kstr* part)
{
    size_t i = 0;
    while (i < k->len && k->data[i] != delim) {
        i++;
    }

    Kstr res = kstr_new(k->data,i);

    if (i < k->len) {
        k->len -= i +1;
        k->data += i +1;
        if (part) {
            *part = res;
        }
        return true;
    }

    return false;
}

/**
 * Scans the passed Kstr and cuts it up to the first occurrence of passed char, even if it is not present. Returns a new Kstr with the original data.
 * @see Kstr
 * @param k The Kstr to scan.
 * @param delim The char to look for.
 * @return A new Kstr with the original data.
 */
Kstr kstr_token(Kstr *k, char delim)
{
    size_t i = 0;
    while (i < k->len && k->data[i] != delim) {
        i++;
    }

    Kstr res = kstr_new(k->data,i);

    if (i < k->len) {
        k->len -= i +1;
        k->data += i +1;
    } else {
        k->len -= i;
        k->data += i;
    }

    return res;
}

Kstr kstr_token_kstr(Kstr* k, Kstr delim)
{

    //Kstr to scroll k data, sized as the delimiter
    Kstr win = kstr_new(k->data, delim.len);

    size_t i = 0;

    //Loop checking if k data can still be scrolled and if current window is equal to the delimiter
    while (i + delim.len < k->len &&
           !(kstr_eq(win, delim))) {
        i++;
        win.data++;
    }

    //New Kstr just up to the delimiter position
    Kstr res = kstr_new(k->data, i);

    //If we don't cleanly empty k, we increase result len so that it holds the remaining chars
    if (i + delim.len == k->len) {
        res.len += delim.len;
    }

    //Advance k by the delimiter size, plus its starting position
    k->data += i + delim.len;
    k->len += i + delim.len;

    return res;
}

static char * kls_read_file(Koliseo* kls, const char * f_name, Gulp_Res * err, size_t * f_size, ...)
{
    if (!kls) {
        *err = GULP_FILE_KLS_NULL;
        return NULL;
    }
    char * buffer;
    size_t length = 0;
    FILE * f = fopen(f_name, "rb");
    size_t read_length;

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);

        va_list args;
        va_start(args, f_size);
        size_t max_size = va_arg(args, size_t);
        if (length > max_size) {
            *err = GULP_FILE_TOO_LARGE;

            return NULL;
        }
        va_end(args);

        buffer = KLS_PUSH_ARR_NAMED(kls,char,length + 1,"char*","Buffer for file gulp");

        if (buffer == NULL) {
            assert(0 && "KLS_PUSH_NAMED() failed\n");
        }

        if (length) {
            read_length = fread(buffer, 1, length, f);

            if (length != read_length) {
                *err = GULP_FILE_READ_ERROR;
                return NULL;
            }
        }

        fclose(f);

        *err = GULP_FILE_OK;
        buffer[length] = '\0';
        *f_size = length;
    } else {
        *err = GULP_FILE_NOT_EXIST;

        return NULL;
    }

    if (strlen(buffer) == length) {
        return buffer;
    } else {
        *err = GULP_FILE_CONTAINS_NULLCHAR;
        return buffer;
    }
}

/**
 * Tries mapping the passed file on the Koliseo.
 * Sets the passed Gulp_Res to the result of the operation, .
 * @param kls The Koliseo to push to.
 * @param filepath Path to the file to gulp.
 * @param err Pointer to the Gulp_Res variable to store result.
 * @param max_size Max size allowed for the read file.
 * @see KLS_GULP_FILE()
 */
char * kls_gulp_file_sized(Koliseo* kls, const char * filepath, Gulp_Res * err, size_t max_size)
{
    static_assert(TOT_GULP_RES == 6, "Number of Gulp_Res changed");
    size_t f_size;
    char * data = NULL;
    data = kls_read_file(kls, filepath, err, &f_size, max_size);
    if (*err != GULP_FILE_OK) {
        switch (*err) {
        case GULP_FILE_NOT_EXIST:
        case GULP_FILE_TOO_LARGE:
        case GULP_FILE_READ_ERROR:
        case GULP_FILE_CONTAINS_NULLCHAR:
        case GULP_FILE_KLS_NULL: {
            fprintf(stderr,"[ERROR]    %s():  {" Gulp_Res_Fmt "}.\n",__func__, Gulp_Res_Arg(*err));
        }
        break;
        default: {
            fprintf(stderr,"[ERROR]    %s():  Unexpected error {%i}.\n",__func__, *err);
        }
        break;
        }
        if (*err != GULP_FILE_CONTAINS_NULLCHAR) return NULL;
    } else {
        assert(strlen(data) == f_size && "data len should be equal to f_size here!");
        if (!data) {
            assert(0 && "kls_read_file() failed\n");
        }
        //printf("%s\n\n",data);
        //printf("SIZE: {%i}\n",f_size);
    }
    return data;
}

/**
 * Tries mapping the passed file on the Koliseo.
 * @param kls The Koliseo to push to.
 * @param filepath Path to the file to gulp.
 * @param max_size Max size allowed for the read file.
 * @see KLS_GULP_FILE()
 * @return A pointer to the string with file contents.
 */
char * try_kls_gulp_file(Koliseo* kls, const char * filepath, size_t max_size)
{
    Gulp_Res err = -1;

    char* res = kls_gulp_file_sized(kls, filepath, &err, max_size);

    if (err != GULP_FILE_OK && err != GULP_FILE_CONTAINS_NULLCHAR) {
        fprintf(stderr, "%s():  kls_gulp_file_sized() failed with err {%s}.\n",__func__,string_from_Gulp_Res(err));
    }

    return res;
}

static Kstr * kls_read_file_to_kstr(Koliseo* kls, const char * f_name, Gulp_Res * err, size_t * f_size, ...)
{
    if (!kls) {
        *err = GULP_FILE_KLS_NULL;
        return NULL;
    }
    char * buffer = NULL;
    size_t length = 0;
    FILE * f = fopen(f_name, "rb");
    size_t read_length;
    bool allow_nullchar = false;

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);

        va_list args;
        va_start(args, f_size);
        size_t max_size = va_arg(args, size_t);
        if (length > max_size) {
            *err = GULP_FILE_TOO_LARGE;

            return NULL;
        }
        bool allow_nulls = va_arg(args, int);
        allow_nullchar = allow_nulls;
        va_end(args);

        buffer = KLS_PUSH_ARR_NAMED(kls,char,length + 1,"char*","Buffer for file gulp");

        if (buffer == NULL) {
            assert(0 && "KLS_PUSH_NAMED() failed\n");
        }

        if (length) {
            read_length = fread(buffer, 1, length, f);

            if (length != read_length) {
                *err = GULP_FILE_READ_ERROR;
                return NULL;
            }
        }

        fclose(f);

        *err = GULP_FILE_OK;
        buffer[length] = '\0';
        *f_size = length;
    } else {
        *err = GULP_FILE_NOT_EXIST;

        return NULL;
    }

    if (strlen(buffer) == length) {
    } else {
        *err = GULP_FILE_CONTAINS_NULLCHAR;
        if (!allow_nullchar) {
            return NULL;
        }
    }
    Kstr * res = KLS_PUSH_NAMED(kls,Kstr,"Kstr","Kstr for file gulp");
    if (res == NULL) {
        assert(0 && "KLS_PUSH_NAMED() failed\n");
    }
    res->data = buffer;
    if (*err == GULP_FILE_CONTAINS_NULLCHAR) {
        res->len = length;
    } else {
        res->len = strlen(buffer);
    }
    return res;
}

/**
 * Tries mapping the passed file on the Koliseo.
 * Sets the passed Gulp_Res to the result of the operation.
 * @param kls The Koliseo to push to.
 * @param filepath Path to the file to gulp.
 * @param err Pointer to the Gulp_Res variable to store result.
 * @param max_size Max size allowed for the read file.
 * @param allow_nullchar Bool to avoid returning NULL for a binary file.
 * @see KLS_GULP_FILE()
 * @return A Kstr for the passed filepath contents.
 */
Kstr * kls_gulp_file_sized_to_kstr(Koliseo* kls, const char * filepath, Gulp_Res * err, size_t max_size, bool allow_nullchar)
{
    static_assert(TOT_GULP_RES == 6, "Number of Gulp_Res changed");
    size_t f_size;
    Kstr * data = NULL;
    data = kls_read_file_to_kstr(kls, filepath, err, &f_size, max_size, allow_nullchar);
    if (*err != GULP_FILE_OK) {
        switch (*err) {
        case GULP_FILE_NOT_EXIST:
        case GULP_FILE_TOO_LARGE:
        case GULP_FILE_READ_ERROR:
        case GULP_FILE_CONTAINS_NULLCHAR:
        case GULP_FILE_KLS_NULL: {
            fprintf(stderr,"[ERROR]    %s():  {" Gulp_Res_Fmt "}.\n",__func__, Gulp_Res_Arg(*err));
        }
        break;
        default: {
            fprintf(stderr,"[ERROR]    %s():  Unexpected error {%i}.\n",__func__, *err);
        }
        break;
        }
        return data;
    } else {
        assert(data->len == f_size && "data len should be equal to f_size here!");
        if (!data) {
            assert(0 && "kls_read_file_to_kstr() failed\n");
        }
        //printf("%s\n\n",data->data);
        //printf("SIZE: {%i}\n",f_size);
    }
    return data;
}

/**
 * Tries mapping the passed file on the Koliseo.
 * @param kls The Koliseo to push to.
 * @param filepath Path to the file to gulp.
 * @param max_size Max size allowed for the read file.
 * @param allow_nullchar Boolean to avoid returning NULL for a binary file.
 * @see KLS_GULP_FILE()
 * @return A pointer to the Kstr with file contents.
 */
Kstr * try_kls_gulp_file_to_kstr(Koliseo* kls, const char * filepath, size_t max_size, bool allow_nullchar)
{
    Gulp_Res err = -1;

    Kstr * res = NULL;
    res = kls_gulp_file_sized_to_kstr(kls, filepath, &err, max_size, allow_nullchar);

    if (err != GULP_FILE_OK) {
        fprintf(stderr, "%s():  kls_gulp_file_sized_to_kstr() failed with err {%s}.\n",__func__,string_from_Gulp_Res(err));
    }

    return res;
}

#endif // KLS_GULP_IMPLEMENTATION
