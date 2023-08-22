#ifndef KOLISEO_H
#define KOLISEO_H
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#define KLS_MAJOR 0 /**< Represents current major release.*/
#define KLS_MINOR 1 /**< Represents current minor release.*/
#define KLS_PATCH 5 /**< Represents current patch release.*/

/**
 * Global variable for debug flag.
 */
extern int KOLISEO_DEBUG;

/**
 * Global variable for debug file pointer.
 */
extern FILE* KOLISEO_DEBUG_FP;

static const char KOLISEO_API_VERSION_STRING[] = "0.1.6"; /**< Represents current version with MAJOR.MINOR.PATCH format.*/

const char* string_koliseo_version(void);

void kls_log(const char* tag, const char* msg);

#define KLS_DEFAULT_SIZE (16*1024) /**< Represents a simple default size for demo purposes.*/

#ifndef KLS_DEFAULT_ALIGNMENT
#define KLS_DEFAULT_ALIGNMENT (2*sizeof(void *)) /**< Represents a default alignment value. Not used.*/
#endif

/**
 * Represents the initialised arena allocator struct.
 * @see kls_new()
 * @see kls_clear()
 * @see kls_free()
 * @see KLS_PUSH()
 * @see KLS_POP()
 */
typedef struct Koliseo {
	char* data; /**< Points to data field.*/
	ptrdiff_t size; /**< Size of data field.*/
	ptrdiff_t offset; /**< Current position of memory pointer.*/
	ptrdiff_t prev_offset; /**< Previous position of memory pointer.*/
} Koliseo;

/**
 * Represents a savestate for a Koliseo.
 * @see kls_temp_start()
 * @see kls_temp_end()
 * @see KLS_PUSH()
 * @see KLS_POP()
 */
typedef struct Koliseo_Temp {
	Koliseo* kls; /**< Reference to the actual Koliseo we're saving.*/
	ptrdiff_t offset; /**< Current position of memory pointer.*/
	ptrdiff_t prev_offset; /**< Previous position of memory pointer.*/
} Koliseo_Temp;

ptrdiff_t kls_get_pos(Koliseo* kls);

Koliseo* kls_new(ptrdiff_t size);

void* kls_push(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void* kls_push_zero(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void* kls_pop(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);

#define KLS_PUSH(kls, type, count) (type*)kls_push_zero(kls, sizeof(type), _Alignof(type), count)
#define KLS_POP(kls, type, count) (type*)kls_pop(kls, sizeof(type), _Alignof(type), count)

#define KLS_PUSH_ARRAY(kls, type, count) (type*)kls_push_zero(kls, sizeof(type)*(count), _Alignof(type), count)
#define KLS_PUSH_STRUCT(kls, type) (type*)KLS_PUSH_ARRAY((kls), (type))

void kls_clear(Koliseo* kls);
void kls_free(Koliseo* kls);
void print_kls_2file(FILE* fp, Koliseo* kls);
void print_dbg_kls(Koliseo* kls);

Koliseo_Temp kls_temp_start(Koliseo* kls);
void kls_temp_end(Koliseo_Temp tmp_kls);

#define KLS_PUSH_T(kls_temp, type, count) (type*)KLS_PUSH(kls_temp.kls, type, count)
#define KLS_POP_T(kls_temp, type, count) (type*)KLS_POP(kls_temp.kls, type, count)

#endif
