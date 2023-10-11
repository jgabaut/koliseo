#ifndef KOLISEO_H_
#define KOLISEO_H_
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <time.h>


#define KLS_MAJOR 0 /**< Represents current major release.*/
#define KLS_MINOR 2 /**< Represents current minor release.*/
#define KLS_PATCH 6 /**< Represents current patch release.*/

/**
 * Global variable for debug flag.
 */
extern int KOLISEO_DEBUG;

/**
 * Global variable for debug file pointer.
 * FIXME: ATM this file pointer is not correctly closed when an error causes an exit() call.
 */
extern FILE* KOLISEO_DEBUG_FP;

/**
 * Defines flags for Koliseo.
 */
typedef struct KLS_Conf {
    int kls_autoset_regions;
    int kls_autoset_temp_regions;
} KLS_Conf;

/**
 * Default KLS_Conf used by kls_new().
 * @see kls_new()
 * @see KLS_Conf
 */
extern KLS_Conf KLS_DEFAULT_CONF;

/**
 * Defines a format string for KLS_Conf.
 * @see KLS_Conf_Arg()
 */
#define KLS_Conf_Fmt "KLS_Conf {autoset_regions: %i, autoset_temp_regions: %i}"

/**
 * Defines a format macro for KLS_Conf args.
 * @see KLS_Conf_Fmt
 */
#define KLS_Conf_Arg(conf) (conf.kls_autoset_regions),(conf.kls_autoset_temp_regions)

/**
 * Defines flags for Koliseo_Temp.
 */
typedef struct KLS_Temp_Conf {
    int kls_autoset_regions;
} KLS_Temp_Conf;

/**
 * Defines a format string for KLS_Temp_Conf.
 * @see KLS_Temp_Conf_Arg()
 */
#define KLS_Temp_Conf_Fmt "KLS_Temp_Conf {autoset_regions: %i}"

/**
 * Defines a format macro for KLS_Conf args.
 * @see KLS_Temp_Conf_Fmt
 */
#define KLS_Temp_Conf_Arg(conf) (conf.kls_autoset_regions)

static const int KOLISEO_API_VERSION_INT = (KLS_MAJOR*1000000+KLS_MINOR*10000+KLS_PATCH*100); /**< Represents current version with numeric format.*/
static const char KOLISEO_API_VERSION_STRING[] = "0.2.6"; /**< Represents current version with MAJOR.MINOR.PATCH format.*/

const char* string_koliseo_version(void);

const int int_koliseo_version(void);

#define KLS_TITLEROWS 33 /**< Defines how many rows the title banner has.*/
extern char* kls_title[KLS_TITLEROWS+1];

void kls_print_title_2file(FILE* fp); /**< Prints the title banner to the passed FILE.*/
void kls_print_title(void);

void kls_log(const char* tag, const char* format, ...);

#define KLS_DEFAULT_SIZE (16*1024) /**< Represents a simple default size for demo purposes.*/

#ifndef KLS_DEFAULT_ALIGNMENT
#define KLS_DEFAULT_ALIGNMENT (2*sizeof(void *)) /**< Represents a default alignment value. Not used.*/
#endif

/**
 * Represents a type index for Regions.
 * @see KLS_PUSH_TYPED()
 */
typedef enum KLS_Region_Type {
	KLS_None=0,
	Temp_KLS_Header=1,
	KLS_Header=2,
} KLS_Region_Type;

/**
 * Defines max index for Koliseo's own Region_Type values.
 * @see Region_Type
 */
#define KLS_REGIONTYPE_MAX KLS_Header

/**
 * Represents an allocated memory region in a Koliseo.
 * @see KLS_PUSH()
 * @see KLS_PUSH_NAMED()
 */
typedef struct KLS_Region {
	ptrdiff_t begin_offset; /**< Starting offset of memory region.*/
	ptrdiff_t end_offset; /**< Ending offset of memory region.*/
	ptrdiff_t size; /**< Size of memory for the KLS_Region.*/
	char name[255]; /**< Name field for the KLS_Region.*/
	char desc[255]; /**< Description field for the KLS_Region.*/
	int type; /**< Used to identify which type the KLS_Region holds.*/
} KLS_Region;

static const char KOLISEO_DEFAULT_REGION_NAME[] = "No Name"; /**< Represents default Region name, used for kls_push_zero().*/
static const char KOLISEO_DEFAULT_REGION_DESC[] = "No Desc"; /**< Represents default Region desc, used for kls_push_zero().*/


#ifndef KOLISEO_LIST_H_
#define KOLISEO_LIST_H_
#include <stdbool.h>

typedef KLS_Region* KLS_list_element;

/**
 * Defines the node for a KLS_Region_List.
 * @see KLS_list_element
 */
typedef struct KLS_list_region
{
	KLS_list_element value; /**< The KLS_Region value.*/
	struct KLS_list_region *next; /**< Pointer to the next node int the list.*/
} KLS_region_list_item;

typedef KLS_region_list_item *KLS_Region_List;

struct Koliseo_Temp; //Forward declaration for Koliseo itself

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
	KLS_Region_List regs; /**< List of allocated Regions*/
	int has_temp; /**< When == 1, a Koliseo_Temp is currently active on this Koliseo.*/
    KLS_Conf conf; /**< Contains flags to change the Koliseo behaviour.*/
	struct Koliseo_Temp* t_kls; /**< Points to related active Kolieo_Temp, when has_temp == 1.*/
} Koliseo;

/**
 * Defines a format string for Koliseo.
 * @see KLSArg()
 */
#ifndef _WIN32
#define KLSFmt "KLS {begin: %p, curr: %p, size: %li, offset: %li, has_temp: %i}"
#else
#define KLSFmt "KLS {begin: %p, curr: %p, size: %lli, offset: %lli, has_temp: %i}"
#endif

/**
 * Defines a format macro for Koliseo args.
 * @see KLSFmt
 */
#define KLS_Arg(kls) (void*)(kls),(void*)((kls)+(kls->offset)),(kls->size),(kls->offset),(kls->has_temp)

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
	KLS_Region_List t_regs; /**< List of temporarily allocated Regions*/
    KLS_Temp_Conf conf; /**< Contains flags to change the Koliseo_Temp behaviour.*/
} Koliseo_Temp;

ptrdiff_t kls_get_pos(Koliseo* kls);

Koliseo* kls_new(ptrdiff_t size);
int kls_set_conf(Koliseo* kls, KLS_Conf conf);
Koliseo* kls_new_conf(ptrdiff_t size, KLS_Conf conf);

//void* kls_push(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void* kls_push_zero(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void* kls_push_zero_AR(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void* kls_push_zero_named(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, char* name, char* desc);
void* kls_push_zero_typed(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, int type, char* name, char* desc);
void* kls_pop(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);

#define KLS_PUSH(kls, type, count) (type*)kls_push_zero_AR(kls, sizeof(type), _Alignof(type), count)
#define KLS_PUSH_NAMED(kls, type, count, name, desc) (type*)kls_push_zero_named(kls, sizeof(type), _Alignof(type), count, name, desc)
#define KLS_PUSH_TYPED(kls, type, count, region_type, name, desc) (type*)kls_push_zero_typed(kls, sizeof(type), _Alignof(type), count, region_type, name, desc)
#define KLS_POP(kls, type, count) (type*)kls_pop(kls, sizeof(type), _Alignof(type), count)

#define KLS_PUSH_ARRAY(kls, type, count) (type*)kls_push_zero(kls, sizeof(type)*(count), _Alignof(type), count)
#define KLS_PUSH_STRUCT(kls, type) (type*)KLS_PUSH_ARRAY((kls), (type))

void kls_clear(Koliseo* kls);
void kls_free(Koliseo* kls);
void print_kls_2file(FILE* fp, Koliseo* kls);
void print_dbg_kls(Koliseo* kls);
void kls_formatSize(ptrdiff_t size, char* outputBuffer, size_t bufferSize);

#ifdef KOLISEO_HAS_CURSES

#ifndef KOLISEO_CURSES_H_
#define KOLISEO_CURSES_H_

#ifndef MINGW32_BUILD
#include "ncurses.h"
#else
#include <ncursesw/ncurses.h>
#endif //MINGW32_BUILD

void kls_show_toWin(Koliseo* kls, WINDOW* win);
void kls_showList_toWin(Koliseo* kls, WINDOW* win);
void kls_temp_show_toWin(Koliseo_Temp* t_kls, WINDOW* win);
void kls_temp_showList_toWin(Koliseo_Temp* t_kls, WINDOW* win);
#endif //KOLISEO_CURSES_H_

#endif //KOLISEO_HAS_CURSES

Koliseo_Temp* kls_temp_start(Koliseo* kls);
int kls_temp_set_conf(Koliseo_Temp* t_kls, KLS_Temp_Conf conf);
void kls_temp_end(Koliseo_Temp* tmp_kls);
void* kls_temp_push_zero_AR(Koliseo_Temp* t_kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void* kls_temp_push_zero_named(Koliseo_Temp* t_kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, char* name, char* desc);
void* kls_temp_push_zero_typed(Koliseo_Temp* t_kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, int type, char* name, char* desc);
void* kls_temp_pop(Koliseo_Temp* t_kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
void print_temp_kls_2file(FILE* fp, Koliseo_Temp* t_kls);
void print_dbg_temp_kls(Koliseo_Temp* t_kls);


#define KLS_PUSH_T(kls_temp, type, count) (type*)kls_temp_push_zero_AR(kls_temp, sizeof(type), _Alignof(type), count)
#define KLS_PUSH_T_NAMED(kls_temp, type, count, name, desc) (type*)kls_temp_push_zero_named(kls_temp, sizeof(type), _Alignof(type), count, name, desc)
#define KLS_PUSH_T_TYPED(kls_temp, type, count, region_type, name, desc) (type*)kls_temp_push_zero_typed(kls_temp, sizeof(type), _Alignof(type), count, region_type, name, desc)
#define KLS_POP_T(kls_temp, type, count) (type*)kls_temp_pop(kls_temp, sizeof(type), _Alignof(type), count)

KLS_Region_List kls_emptyList(void);
#define KLS_GETLIST() kls_emptyList()
bool kls_empty(KLS_Region_List);
KLS_list_element kls_head(KLS_Region_List);
KLS_Region_List kls_tail(KLS_Region_List);
KLS_Region_List kls_cons(KLS_list_element, KLS_Region_List);

void kls_freeList(KLS_Region_List);
#define KLS_FREELIST(kls_list) kls_freeList(kls_list)
void kls_showList(KLS_Region_List);
void kls_showList_toFile(KLS_Region_List, FILE* fp);
#define KLS_ECHOLIST(kls_list) kls_showList(kls_list)
#define KLS_PRINTLIST(kls_list,file) kls_showList_toFile(kls_list,file)
bool kls_member(KLS_list_element, KLS_Region_List);
int kls_length(KLS_Region_List);
KLS_Region_List kls_append(KLS_Region_List, KLS_Region_List);
KLS_Region_List kls_reverse(KLS_Region_List);
KLS_Region_List kls_copy(KLS_Region_List);
KLS_Region_List kls_delete(KLS_list_element, KLS_Region_List);

KLS_Region_List kls_insord(KLS_list_element, KLS_Region_List);
#define KLS_PUSHLIST(reg,kls_list) kls_insord(reg,kls_list)
KLS_Region_List kls_insord_p(KLS_list_element, KLS_Region_List);
#define KLS_PUSHLIST_P(reg,kls_list) kls_insord_p(reg,kls_list)
KLS_Region_List kls_mergeList(KLS_Region_List, KLS_Region_List);
KLS_Region_List kls_intersect(KLS_Region_List, KLS_Region_List);
KLS_Region_List kls_diff(KLS_Region_List, KLS_Region_List);

#define KLS_DIFF(kls_list1,kls_list2) kls_diff(kls_list1,kls_list2)
bool kls_isLess(KLS_list_element, KLS_list_element);
bool kls_isEqual(KLS_list_element, KLS_list_element);
double kls_usageShare(KLS_list_element, Koliseo*);
void kls_usageReport_toFile(Koliseo*,FILE*);
void kls_usageReport(Koliseo*);
ptrdiff_t kls_type_usage(int, Koliseo*);

#endif

#endif
