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
#define KLS_PATCH 9 /**< Represents current patch release.*/

/**
 * Global variable for debug flag.
 */
extern int KOLISEO_DEBUG;
/**
 * Global variable for auto-setting of Regions flag.
 */
extern int KOLISEO_AUTOSET_REGIONS;

/**
 * Global variable for debug file pointer.
 */
extern FILE* KOLISEO_DEBUG_FP;

static const char KOLISEO_API_VERSION_STRING[] = "0.1.9"; /**< Represents current version with MAJOR.MINOR.PATCH format.*/

const char* string_koliseo_version(void);

void kls_log(const char* tag, const char* msg);

#define KLS_DEFAULT_SIZE (16*1024) /**< Represents a simple default size for demo purposes.*/

#ifndef KLS_DEFAULT_ALIGNMENT
#define KLS_DEFAULT_ALIGNMENT (2*sizeof(void *)) /**< Represents a default alignment value. Not used.*/
#endif

/**
 * Represents an allocated Region in a Koliseo.
 * @see KLS_PUSH()
 * @see KLS_PUSH_NAMED()
 */
typedef struct Region {
	ptrdiff_t begin_offset; /**< Starting offset of memory region.*/
	ptrdiff_t end_offset; /**< Ending offset of memory region.*/
	ptrdiff_t size; /**< Size of memory for the Region.*/
	char name[255]; /**< Name field for the Region.*/
	char desc[255]; /**< Description field for the Region.*/
} Region;

static const char KOLISEO_DEFAULT_REGION_NAME[] = "No Name"; /**< Represents default Region name, used for kls_push_zero().*/
static const char KOLISEO_DEFAULT_REGION_DESC[] = "No Desc"; /**< Represents default Region desc, used for kls_push_zero().*/

#ifndef KOLISEO_LIST_H
#define KOLISEO_LIST_H
#include "stdbool.h"

typedef Region* element;

typedef struct list_region
{
	element value;
	struct list_region *next;
} region_list_item;
typedef region_list_item *Region_List;

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
	Region_List regs; /**< List of allocated Regions*/
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

//void* kls_push(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void* kls_push_zero(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void* kls_push_zero_named(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, char* name, char* desc);
void* kls_pop(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);

#define KLS_PUSH(kls, type, count) (type*)kls_push_zero(kls, sizeof(type), _Alignof(type), count)
#define KLS_PUSH_NAMED(kls, type, count, name, desc) (type*)kls_push_zero_named(kls, sizeof(type), _Alignof(type), count, name, desc)
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
#define KLS_PUSH_T_NAMED(kls_temp, type, count, name, desc) (type*)KLS_PUSH_NAMED(kls_temp.kls, type, count, name, desc)
#define KLS_POP_T(kls_temp, type, count) (type*)KLS_POP(kls_temp.kls, type, count)

Region_List kls_emptyList(void);
#define KLS_GETLIST() kls_emptyList()
bool kls_empty(Region_List);
element kls_head(Region_List);
Region_List kls_tail(Region_List);
Region_List kls_cons(element, Region_List);

void kls_freeList(Region_List);
#define KLS_FREELIST(kls_list) kls_freeList(kls_list)
void kls_showList(Region_List);
void kls_showList_toFile(Region_List, FILE* fp);
#define KLS_ECHOLIST(kls_list) kls_showList(kls_list)
#define KLS_PRINTLIST(kls_list,file) kls_showList_toFile(kls_list,file)
bool kls_member(element, Region_List);
int kls_length(Region_List);
Region_List kls_append(Region_List, Region_List);
Region_List kls_reverse(Region_List);
Region_List kls_copy(Region_List);
Region_List kls_delet(element, Region_List);

Region_List kls_insord(element, Region_List);
#define KLS_PUSHLIST(reg,kls_list) kls_insord(reg,kls_list)
Region_List kls_insord_p(element, Region_List);
#define KLS_PUSHLIST_P(reg,kls_list) kls_insord_p(reg,kls_list)
Region_List kls_mergeList(Region_List, Region_List);
Region_List kls_intersect(Region_List, Region_List);
Region_List kls_diff(Region_List, Region_List);

#define KLS_DIFF(kls_list1,kls_list2) kls_diff(kls_list1,kls_list2)
bool kls_isLess(element, element);
bool kls_isEqual(element, element);
double kls_usageShare(element, Koliseo*);
void kls_usageReport_toFile(Koliseo*,FILE*);
void kls_usageReport(Koliseo*);

#endif

#endif
