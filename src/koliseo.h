#ifndef KOLISEO_H
#define KOLISEO_H
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
static const char KOLISEO_API_VERSION_STRING[] = "0.1.0";
const char* string_koliseo_version(void);

#define KLS_DEFAULT_SIZE (16*1024)

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif

typedef struct Koliseo {
	char* data; /**< Points to data field*/
	ptrdiff_t size; /**< Size of data field*/
	ptrdiff_t offset; /**< Current position of memory pointer*/
	ptrdiff_t prev_offset; /**< Previous position of memory pointer*/
} Koliseo;

ptrdiff_t kls_get_pos(Koliseo* kls);

Koliseo *kls_new(ptrdiff_t size);

void* kls_push(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void* kls_push_zero(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void* kls_pop(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);

#define KLS_PUSH(kls, type, count) (type*)kls_push_zero(kls, sizeof(type), _Alignof(type), count)
#define KLS_POP(kls, type, count) (type*)kls_pop(kls, sizeof(type), _Alignof(type), count)

#define KLS_PUSH_ARRAY(kls, type, count) (type*)kls_push_zero(kls, sizeof(type)*(count), _Alignof(type), count)
#define KLS_PUSH_STRUCT(kls, type) (type*)KLS_PUSH_ARRAY((kls), (type))

void kls_clear(Koliseo* kls);
void kls_free(Koliseo* kls);
void print_dbg_kls(Koliseo* kls);

#endif
