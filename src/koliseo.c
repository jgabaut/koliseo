#include "koliseo.h"
//Default settings for global vars.
int KOLISEO_DEBUG = 0;
int KOLISEO_AUTOSET_REGIONS = 1;
int KOLISEO_AUTOSET_TEMP_REGIONS = 0;
FILE* KOLISEO_DEBUG_FP = NULL;

/**
 * Defines titlescreen.
 */
char* kls_title[KLS_TITLEROWS+1] = {
	"                               .',,,.                                                               ",
	"                        ...''',,;;:cl;.                                                             ",
	"                  ..''''''....      .co,                                                            ",
	"              .'',,,'..               'ol.                                                          ",
	"          ..''''..                     .co.                                                         ",
	"        ..'..                            cl.                                                        ",
	"      ..'...               .          .  .:'...       .       .      ..      .....     ...          ",
	"     ....      .      ..:c,'.   .oc .ll.  :dddxo.    ld.     'x:  .cxddx;   :kxodo,  .lddxx;        ",
	"    .,..     .cl,..   .cxd:..   ;Ol;do.  :Ol. c0c   .kx.     lO;  ;0d..::  .xk'     .xx' 'kx.       ",
	"    .,..     'dOl.     .''......lKOkc.  .kx.  ;0l   ;0l     .xk.  'xk;     'Ok;'.   l0;  .xk.       ",
	"    ''.;,    .,;;............. .xKKO'   ;0c   c0:   lO;     'Od.   .lko.   :0koo:. .xk.  'Od.       ",
	"   ',.'c;.  .......            'Oo:ko.  c0;  .xk.  .xx.     :0c  .   ;Od. .oO,     'Od.  c0:        ",
	"   ,;  .......        .::.     :O; lO;  :0l..okc.  'Od...  .oO' .ld'.:Od. 'kk,...  .kk,.:kd.        ",
	"   ;xc,...  .   'd:   .:d;     ;l. .l:  .:dool''c. 'ddolc. .cc.  'ldooc.  'dxoll:.  'odoo:.         ",
	"  .lx;. .  ,d;  .ll.   .''.                     ;l:'                                                ",
	"  :o.   ;,  cc    ..     .                       ,cc:::c,                                           ",
	" .c;    ..  .'           ....',,;;;;;;;;;;,,,,,,,,,,;;;ox;........'cddoollcc:;,,'..                 ",
	"  ,:         ...',:clodxkO0KKXXXXXXKKK000000KKKKXXXXKKKXXXKKKKKKKKKXNNNNNNNNNNNXXKOxoc;'.           ",
	"  .:. ..';cldkOKXXXXK0Okxxdolc::;,''.','......';clc::cloONNNNNNNNNNNNNNNNNNNNNNNNNNNNNNK0xl;.       ",
	"  :xxxOKKK0kxoddl;,';od;.   'cl,    .ckko.    ,d00Ol.   .xXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNKkl;.   ",
	".lKX0xoc,';;. .ll.   ;xO;   .oX0,    ,ONXl    ,0NNNK;    .lKNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNX0o' ",
	".xk;.  .  .;.  .:,    .dc    'OK;    .dNNo.   ;0NNNXc      ;kXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNk.",
	" :c        ..   ..    .l,    .k0,    .xXXo    ;0XNNXc       .l0NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNO'",
	" :;                   .'.    .;:.    .,cl;....,loddo;..       'dKNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN0,",
	".c,        ....',;;:cclllooddddddddddodxxxxxxxdddddddolllccccc:cxXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNK;",
	".l:',:cloxxkkkOkkxxdollcc::;,,'''.............................',cONNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXc",
	":00OOxdoc:;,'...                     ..        ..         .     .oNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXl",
	":o,..',. ..   ';.   .co'     ;d,    'oOk;    .o00kd;    .oOkd,   :KNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXl",
	":;   ,o'  ..  .l:    ;0o    .dXo.   'xNNk.   'ONNNNO'   :KNNNd.   cKNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXo",
	"c;    ..  ..   ';    'ko.   'ONo.   .kNNO'   ,0NNNNK;   :KNNXd.    :0NNNNNNNNNNNNNNNNNNNNNNNNNNNNNXo",
	"l,             ..    'ko    'OXl    ,0NNO'   'ONNNNK;   ;KNNXl.     ;ONNNNNNNNNNNNNNNNNNNNNNNNNNNNXl",
	"cc.............';'...;do'...:k0c....:0XX0:...'xXXXX0c...:0XK0l.......c0NNNNNNNNNNNNNNNNNNNNXXXXXXKO,",
	".;:;;;;;;:::::::::cc:::::::::c:;;;;,;ccc:;,,,,:cccc:;;;;;ccc:;,,,,,;;;clloooooooooooooooooollcc:;,. "
};

void kls_print_title_2file(FILE* fp) {
	if (fp == NULL) {
		fprintf(stderr,"[KLS] kls_print_title_2file():  Passed file pointer was NULL.\n");
		abort();
	}
  	for (int i = 0; i < KLS_TITLEROWS; i++) {
		fprintf(fp,"%s\n",kls_title[i]);
  	}
}

void kls_print_title(void) {
	kls_print_title_2file(stdout);
}

/**
 * Returns the constant string representing current version for Koliseo.
 * @return A constant string in MAJOR-MINOR-PATCH format for current Koliseo version.
 */
const char* string_koliseo_version(void) {
	return KOLISEO_API_VERSION_STRING;
}

/**
 * Returns the constant int representing current version for Koliseo.
 * @return A constant int in numeric format for current Koliseo version.
 */
const int int_koliseo_version(void) {
	return KOLISEO_API_VERSION_INT;
}


/**
 * Returns the current offset (position of pointer bumper) for the passed Koliseo.
 * @param kls The Koliseo at hand.
 * @return A ptrdiff_t value for current position.
 */
ptrdiff_t kls_get_pos(Koliseo* kls) {
	return kls->offset;
}

/**
 * When KOLISEO_DEBUG is 1, logs a message to the defined KOLISEO_DEBUG_FP.
 * @param tag Tag for a message.
 * @param format The message format string.
 */
void kls_log(const char* tag, const char* format, ...) {
	if (KOLISEO_DEBUG == 1) {
		va_list args;
		va_start(args, format);
		if (KOLISEO_DEBUG_FP == NULL) {
			fprintf(stderr,"[KLS]    kls_log(): Failed opening KOLISEO_DEBUG_FP to print logs.\n");
		} else {
			time_t now = time(0);
			struct tm *mytime = localtime(&now);
			char timeheader[500];
			if ( strftime(timeheader, sizeof timeheader, "%X", mytime) )
			{
				fprintf(KOLISEO_DEBUG_FP,"[%-10.10s] [%s] [", tag, timeheader);
				vfprintf(KOLISEO_DEBUG_FP, format, args);
				fprintf(KOLISEO_DEBUG_FP,"]\n");
			}
		}
		va_end(args);
	}
}


/**
 * Takes a ptrdiff_t size and allocates the backing memory for a Koliseo. Sets the fields with appropriate values if memory allocation was successful, goes to abort() otherwise.
 * @param size The size for Koliseo data field.
 * @return A pointer to the initialised Koliseo struct.
 * @see Koliseo
 * @see Koliseo_Temp
 * @see kls_temp_start()
 * @see kls_temp_end()
 */
Koliseo* kls_new(ptrdiff_t size) {
	assert(size >= (ptrdiff_t)sizeof(Koliseo));
	void *p = malloc(size);
	if (p) {
		//sprintf(msg,"Allocated (%li) for new KLS.",size);
		//kls_log("KLS",msg);
		char h_size[200];
		kls_formatSize(size,h_size,sizeof(h_size));
		#ifdef KLS_DEBUG_CORE
		kls_log("KLS","API Level { %i } ->  Allocated (%s) for new KLS.",int_koliseo_version(),h_size);
		#endif
		Koliseo* kls = p;
		kls->data = p;
		kls->size = size;
		kls->offset = sizeof(*kls);
		kls->prev_offset = kls->offset;
		kls->has_temp = 0;
		if (KOLISEO_AUTOSET_REGIONS == 1) {
			#ifdef KLS_DEBUG_CORE
			kls_log("KLS","Init of Region_List for kls.");
			#endif
			Region* kls_header = (Region*) malloc(sizeof(Region));
			kls_header->begin_offset = 0;
			kls_header->end_offset = kls->offset;
			kls_header->type = KLS_Header;
			strcpy(kls_header->name,"KLS Header");
			strcpy(kls_header->desc,"Denotes Space occupied by the Koliseo header.");
			Region_List reglist = kls_emptyList();
			reglist = kls_cons(kls_header,reglist);
			kls->regs = reglist;
			if (kls->regs == NULL) {
			  fprintf(stderr,"[KLS] kls_new() failed to get a Region_List.\n");
			  abort();
			}
		}
	} else {
		fprintf(stderr,"[KLS] Failed kls_new() call.\n");
		abort();
	}
	#ifdef KLS_DEBUG_CORE
	if (KOLISEO_DEBUG == 1) {
		print_kls_2file(KOLISEO_DEBUG_FP,p);
	}
	#endif
	return p;
}

/**
 * Takes a Koliseo pointer, and ptrdiff_t values for size, align and count. Tries popping the specified amount of memory from the Koliseo data field, marking it as free (as far as Koliseo is concerned), or goes to abort() if the operation fails.
 * @param kls The Koliseo at hand.
 * @param size The size for data to pop.
 * @param align The alignment for data to pop.
 * @param count The multiplicative quantity to scale data size to pop for.
 * @return A void pointer to the start of memory just popped from the Koliseo.
 */
void* kls_pop(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count) {
	ptrdiff_t padding = -kls->offset & (align -1);
	if (count > PTRDIFF_MAX/size || (kls->size + kls->offset) < (size*count)) {
		fprintf(stderr,"[KLS] Failed kls_pop() call.\n");
		abort();
	}
	char* p = kls->data + kls->offset - padding - size*count;
	kls->prev_offset = kls->offset;
	kls->offset -= padding + size*count;
	#ifdef KLS_DEBUG_CORE
	kls_log("KLS","API Level { %i } -> Popped (%li) for KLS.", int_koliseo_version(), size);
	if (KOLISEO_DEBUG == 1) {
		print_kls_2file(KOLISEO_DEBUG_FP,kls);
	}
	#endif
	return p;
}

/**
 * Takes a Koliseo_Temp, and ptrdiff_t values for size, align and count. Tries popping the specified amount of memory from the Koliseo data field, marking it as free (as far as Koliseo is concerned), or goes to abort() if the operation fails.
 * @param t_kls The Koliseo_Temp at hand.
 * @param size The size for data to pop.
 * @param align The alignment for data to pop.
 * @param count The multiplicative quantity to scale data size to pop for.
 * @return A void pointer to the start of memory just popped from the referred Koliseo.
 */
void* kls_temp_pop(Koliseo_Temp t_kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count) {
	Koliseo* kls = t_kls.kls;
	if (kls == NULL) {
		fprintf(stderr,"[ERROR] [%s()]: Referred Koliseo was NULL.\n",__func__);
		#ifdef KLS_DEBUG_CORE
		kls_log("ERROR","[%s()]: Referred Koliseo was NULL.\n",__func__);
		#endif
		abort();
	}
	ptrdiff_t padding = -kls->offset & (align -1);
	if (count > PTRDIFF_MAX/size || (kls->size + kls->offset) < (size*count)) {
		fprintf(stderr,"[KLS] Failed kls_pop() call.\n");
		abort();
	}
	char* p = kls->data + kls->offset - padding - size*count;
	kls->prev_offset = kls->offset;
	kls->offset -= padding + size*count;
	#ifdef KLS_DEBUG_CORE
	kls_log("KLS","API Level { %i } -> Popped (%li) for Temp_KLS.", int_koliseo_version(), size);
	if (KOLISEO_DEBUG == 1) {
		print_kls_2file(KOLISEO_DEBUG_FP,kls);
	}
	#endif
	return p;
}

/**
 * Takes a Koliseo pointer, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the Koliseo data field, or goes to abort() if the operation fails.
 * Notably, it does NOT zero the memory region.
 * @param kls The Koliseo at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @return A void pointer to the start of memory just pushed to the Koliseo.
 */
void* kls_push(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count) {
	ptrdiff_t available = kls->size - kls->offset;
	ptrdiff_t padding = -kls->offset & (align -1);
	if (count > PTRDIFF_MAX/size || available-padding < size*count) {
		if (count > PTRDIFF_MAX/size) {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n", count, PTRDIFF_MAX/size);
			#else
			fprintf(stderr, "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n", count, PTRDIFF_MAX/size);
			#endif
		} else {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n", size*count, available-padding);
			#else
			fprintf(stderr, "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n", size*count, available-padding);
			#endif
		}
		fprintf(stderr,"[KLS] Failed kls_push() call.\n");
		abort();
	}
	char* p = kls->data + kls->offset + padding;
	kls->prev_offset = kls->offset;
	kls->offset += padding + size*count;
	char h_size[200];
	kls_formatSize(size,h_size,sizeof(h_size));
	//sprintf(msg,"Pushed size (%li) for KLS.",size);
	//kls_log("KLS",msg);
	#ifdef KLS_DEBUG_CORE
	kls_log("KLS","API Level { %i } -> Pushed size (%s) for KLS.", int_koliseo_version(), h_size);
	if (KOLISEO_DEBUG == 1) {
		print_kls_2file(KOLISEO_DEBUG_FP,kls);
	}
	#endif
	return p;
}

/**
 * Takes a Koliseo pointer, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the Koliseo data field, or goes to abort() if the operation fails.
 * Notably, it zeroes the memory region.
 * @param kls The Koliseo at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @return A void pointer to the start of memory just pushed to the Koliseo.
 */
void* kls_push_zero(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count) {
	ptrdiff_t available = kls->size - kls->offset;
	ptrdiff_t padding = -kls->offset & (align -1);
	if (count > PTRDIFF_MAX/size || (available - padding) < (size*count)) {
		if (count > PTRDIFF_MAX/size) {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n", count, PTRDIFF_MAX/size);
			#else
			fprintf(stderr, "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n", count, PTRDIFF_MAX/size);
			#endif
		} else {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n", size*count, available-padding);
			#else
			fprintf(stderr, "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n", size*count, available-padding);
			#endif
		}
		fprintf(stderr,"[KLS] Failed kls_push_zero() call.\n");
		abort();
		//return 0;
	}
	char* p = kls->data + kls->offset + padding;
	//Zero new area
	memset(p, 0, size*count);
	kls->prev_offset = kls->offset;
	kls->offset += padding + size*count;
	char h_size[200];
	kls_formatSize(size,h_size,sizeof(h_size));
	//sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
	//kls_log("KLS",msg);
	#ifdef KLS_DEBUG_CORE
	kls_log("KLS","API Level { %i } -> Pushed zeroes, size (%s) for KLS.",h_size);
	if (KOLISEO_DEBUG == 1) {
		print_kls_2file(KOLISEO_DEBUG_FP,kls);
	}
	#endif
	return p;
}

/**
 * Takes a Koliseo pointer, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the Koliseo data field, or goes to abort() if the operation fails.
 * Notably, it zeroes the memory region.
 * @param kls The Koliseo at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @return A void pointer to the start of memory just pushed to the Koliseo.
 */
void* kls_push_zero_AR(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count) {
	ptrdiff_t available = kls->size - kls->offset;
	ptrdiff_t padding = -kls->offset & (align -1);
	if (count > PTRDIFF_MAX/size || (available - padding) < (size*count)) {
		if (count > PTRDIFF_MAX/size) {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n", count, PTRDIFF_MAX/size);
			#else
			fprintf(stderr, "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n", count, PTRDIFF_MAX/size);
			#endif
		} else {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n", size*count, available-padding);
			#else
			fprintf(stderr, "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n", size*count, available-padding);
			#endif
		}
		fprintf(stderr,"[KLS] Failed kls_push_zero() call.\n");
		abort();
		//return 0;
	}
	char* p = kls->data + kls->offset + padding;
	//Zero new area
	memset(p, 0, size*count);
	kls->prev_offset = kls->offset;
	kls->offset += padding + size*count;
	if (KOLISEO_AUTOSET_REGIONS == 1) {
		Region* reg = (Region*) malloc(sizeof(Region));
		reg->begin_offset = kls->prev_offset;
		reg->end_offset = kls->offset;
		reg->type = None;
		strcpy(reg->name, KOLISEO_DEFAULT_REGION_NAME);
		strcpy(reg->desc,KOLISEO_DEFAULT_REGION_DESC);
		Region_List reglist = kls_emptyList();
		reglist = kls_cons(reg,reglist);
		kls->regs = kls_append(reglist, kls->regs);
	}

	char h_size[200];
	kls_formatSize(size,h_size,sizeof(h_size));
	//sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
	//kls_log("KLS",msg);
	#ifdef KLS_DEBUG_CORE
	kls_log("KLS","API Level { %i } -> Pushed zeroes, size (%s) for KLS.",h_size);
	if (KOLISEO_DEBUG == 1) {
		print_kls_2file(KOLISEO_DEBUG_FP,kls);
	}
	#endif
	return p;
}
/**
 * Takes a Koliseo_Temp, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the referred Koliseo data field, or goes to abort() if the operation fails.
 * Notably, it zeroes the memory region.
 * @param t_kls The Koliseo_Temp at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @return A void pointer to the start of memory just pushed to the referred Koliseo.
 */
void* kls_temp_push_zero_AR(Koliseo_Temp t_kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count) {
	Koliseo* kls = t_kls.kls;
	if (kls == NULL) {
		fprintf(stderr,"[ERROR] [%s()]: Referred Koliseo was NULL.\n",__func__);
		#ifdef KLS_DEBUG_CORE
		kls_log("ERROR","[%s()]: Referred Koliseo was NULL.\n",__func__);
		#endif
		abort();
	}
	ptrdiff_t available = kls->size - kls->offset;
	ptrdiff_t padding = -kls->offset & (align -1);
	if (count > PTRDIFF_MAX/size || (available - padding) < (size*count)) {
		if (count > PTRDIFF_MAX/size) {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n", count, PTRDIFF_MAX/size);
			#else
			fprintf(stderr, "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n", count, PTRDIFF_MAX/size);
			#endif
		} else {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n", size*count, available-padding);
			#else
			fprintf(stderr, "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n", size*count, available-padding);
			#endif
		}
		fprintf(stderr,"[KLS] Failed kls_push_zero() call.\n");
		abort();
		//return 0;
	}
	char* p = kls->data + kls->offset + padding;
	//Zero new area
	memset(p, 0, size*count);
	kls->prev_offset = kls->offset;
	kls->offset += padding + size*count;
	if (KOLISEO_AUTOSET_TEMP_REGIONS == 1) {
		Region* reg = (Region*) malloc(sizeof(Region));
		reg->begin_offset = kls->prev_offset;
		reg->end_offset = kls->offset;
		reg->type = None;
		strcpy(reg->name, KOLISEO_DEFAULT_REGION_NAME);
		strcpy(reg->desc,KOLISEO_DEFAULT_REGION_DESC);
		Region_List reglist = kls_emptyList();
		reglist = kls_cons(reg,reglist);
		t_kls.t_regs = kls_append(reglist, t_kls.t_regs);
	}

	char h_size[200];
	kls_formatSize(size,h_size,sizeof(h_size));
	//sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
	//kls_log("KLS",msg);
	#ifdef KLS_DEBUG_CORE
	kls_log("KLS","API Level { %i } -> Pushed zeroes, size (%s) for Temp_KLS.",h_size);
	if (KOLISEO_DEBUG == 1) {
		print_kls_2file(KOLISEO_DEBUG_FP,kls);
	}
	#endif
	return p;
}

/**
 * Takes a Koliseo pointer, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the Koliseo data field, or goes to abort() if the operation fails.
 * Uses the passed name and desc fields to initialise the allocated Region fields.
 * Notably, it zeroes the memory region.
 * @param kls The Koliseo at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @return A void pointer to the start of memory just pushed to the Koliseo.
 */
void* kls_push_zero_named(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, char* name, char* desc) {
	ptrdiff_t available = kls->size - kls->offset;
	ptrdiff_t padding = -kls->offset & (align -1);
	if (count > PTRDIFF_MAX/size || (available - padding) < (size*count)) {
		if (count > PTRDIFF_MAX/size) {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n", count, PTRDIFF_MAX/size);
			#else
			fprintf(stderr, "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n", count, PTRDIFF_MAX/size);
			#endif
		} else {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n", size*count, available-padding);
			#else
			fprintf(stderr, "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n", size*count, available-padding);
			#endif
		}
		fprintf(stderr,"[KLS] Failed kls_push_zero() call.\n");
		abort();
		//return 0;
	}
	char* p = kls->data + kls->offset + padding;
	//Zero new area
	memset(p, 0, size*count);
	kls->prev_offset = kls->offset;
	kls->offset += padding + size*count;
	if (KOLISEO_AUTOSET_REGIONS == 1) {
		Region* reg = (Region*) malloc(sizeof(Region));
		reg->begin_offset = kls->prev_offset;
		reg->end_offset = kls->offset;
		reg->type = None;
		strcpy(reg->name,name);
		strcpy(reg->desc,desc);
		Region_List reglist = kls_emptyList();
		reglist = kls_cons(reg,reglist);
		kls->regs = kls_append(reglist, kls->regs);

		char h_size[200];
		kls_formatSize(size,h_size,sizeof(h_size));
		//sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
		//kls_log("KLS",msg);
		#ifdef KLS_DEBUG_CORE
		kls_log("KLS","API Level { %i } -> Pushed zeroes, size (%s) for KLS.", int_koliseo_version(), h_size);
		if (KOLISEO_DEBUG == 1) {
			print_kls_2file(KOLISEO_DEBUG_FP,kls);
		}
		#endif
	}
	return p;
}

/**
 * Takes a Koliseo_Temp, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the referred Koliseo data field, or goes to abort() if the operation fails.
 * Uses the passed name and desc fields to initialise the allocated Region fields.
 * Notably, it zeroes the memory region.
 * @param t_kls The Koliseo_Temp at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @return A void pointer to the start of memory just pushed to the Koliseo.
 */
void* kls_temp_push_zero_named(Koliseo_Temp t_kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, char* name, char* desc) {
	Koliseo* kls = t_kls.kls;
	if (kls == NULL) {
		fprintf(stderr,"[ERROR] [%s()]: Referred Koliseo was NULL.\n",__func__);
		#ifdef KLS_DEBUG_CORE
		kls_log("ERROR","[%s()]: Referred Koliseo was NULL.\n",__func__);
		#endif
		abort();
	}

	ptrdiff_t available = kls->size - kls->offset;
	ptrdiff_t padding = -kls->offset & (align -1);
	if (count > PTRDIFF_MAX/size || (available - padding) < (size*count)) {
		if (count > PTRDIFF_MAX/size) {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n", count, PTRDIFF_MAX/size);
			#else
			fprintf(stderr, "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n", count, PTRDIFF_MAX/size);
			#endif
		} else {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n", size*count, available-padding);
			#else
			fprintf(stderr, "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n", size*count, available-padding);
			#endif
		}
		fprintf(stderr,"[KLS] Failed kls_push_zero() call.\n");
		abort();
		//return 0;
	}
	char* p = kls->data + kls->offset + padding;
	//Zero new area
	memset(p, 0, size*count);
	kls->prev_offset = kls->offset;
	kls->offset += padding + size*count;
	if (KOLISEO_AUTOSET_TEMP_REGIONS == 1) {
		Region* reg = (Region*) malloc(sizeof(Region));
		reg->begin_offset = kls->prev_offset;
		reg->end_offset = kls->offset;
		reg->type = None;
		strcpy(reg->name,name);
		strcpy(reg->desc,desc);
		Region_List reglist = kls_emptyList();
		reglist = kls_cons(reg,reglist);
		t_kls.t_regs = kls_append(reglist, t_kls.t_regs);

		char h_size[200];
		kls_formatSize(size,h_size,sizeof(h_size));
		//sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
		//kls_log("KLS",msg);
		#ifdef KLS_DEBUG_CORE
		kls_log("KLS","API Level { %i } -> Pushed zeroes, size (%s) for Temp_KLS.", int_koliseo_version(), h_size);
		if (KOLISEO_DEBUG == 1) {
			print_kls_2file(KOLISEO_DEBUG_FP,kls);
		}
		#endif
	}
	return p;
}

/**
 * Takes a Koliseo pointer, a Region_Type index, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the Koliseo data field, or goes to abort() if the operation fails.
 * Uses the passed name and desc fields to initialise the allocated Region fields.
 * Notably, it zeroes the memory region.
 * @param kls The Koliseo at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @param type The type index for pushed Region.
 * @return A void pointer to the start of memory just pushed to the referred Koliseo.
 */
void* kls_push_zero_typed(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, int type, char* name, char* desc) {
	ptrdiff_t available = kls->size - kls->offset;
	ptrdiff_t padding = -kls->offset & (align -1);
	if (count > PTRDIFF_MAX/size || (available - padding) < (size*count)) {
		if (count > PTRDIFF_MAX/size) {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n", count, PTRDIFF_MAX/size);
			#else
			fprintf(stderr, "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n", count, PTRDIFF_MAX/size);
			#endif
		} else {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n", size*count, available-padding);
			#else
			fprintf(stderr, "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n", size*count, available-padding);
			#endif
		}
		fprintf(stderr,"[KLS] Failed kls_push_zero() call.\n");
		abort();
		//return 0;
	}
	char* p = kls->data + kls->offset + padding;
	//Zero new area
	memset(p, 0, size*count);
	kls->prev_offset = kls->offset;
	kls->offset += padding + size*count;
	if (KOLISEO_AUTOSET_REGIONS == 1) {
		Region* reg = (Region*) malloc(sizeof(Region));
		reg->begin_offset = kls->prev_offset;
		reg->end_offset = kls->offset;
		reg->type = type;
		strcpy(reg->name,name);
		strcpy(reg->desc,desc);
		Region_List reglist = kls_emptyList();
		reglist = kls_cons(reg,reglist);
		kls->regs = kls_append(reglist, kls->regs);

		char h_size[200];
		kls_formatSize(size,h_size,sizeof(h_size));
		//sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
		//kls_log("KLS",msg);
		#ifdef KLS_DEBUG_CORE
		kls_log("KLS","API Level { %i } -> Pushed zeroes, size (%s) for KLS.", int_koliseo_version(), h_size);
		if (KOLISEO_DEBUG == 1) {
			print_kls_2file(KOLISEO_DEBUG_FP,kls);
		}
		#endif
	}
	return p;
}

/**
 * Takes a Koliseo_Temp, a Region_Type index, and ptrdiff_t values for size, align and count. Tries pushing the specified amount of memory to the referred Koliseo data field, or goes to abort() if the operation fails.
 * Uses the passed name and desc fields to initialise the allocated Region fields.
 * Notably, it zeroes the memory region.
 * @param t_kls The Koliseo_Temp at hand.
 * @param size The size for data to push.
 * @param align The alignment for data to push.
 * @param count The multiplicative quantity to scale data size to push for.
 * @param type The type index for pushed Region.
 * @return A void pointer to the start of memory just pushed to the referred Koliseo.
 */
void* kls_temp_push_zero_typed(Koliseo_Temp t_kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, int type, char* name, char* desc) {
	Koliseo* kls = t_kls.kls;
	if (kls == NULL) {
		fprintf(stderr,"[ERROR] [%s()]: Referred Koliseo was NULL.\n",__func__);
		#ifdef KLS_DEBUG_CORE
		kls_log("ERROR","[%s()]: Referred Koliseo was NULL.\n",__func__);
		#endif
		abort();
	}
	ptrdiff_t available = kls->size - kls->offset;
	ptrdiff_t padding = -kls->offset & (align -1);
	if (count > PTRDIFF_MAX/size || (available - padding) < (size*count)) {
		if (count > PTRDIFF_MAX/size) {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n", count, PTRDIFF_MAX/size);
			#else
			fprintf(stderr, "[KLS]  count [%lli] was bigger than PTRDIFF_MAX/size [%lli].\n", count, PTRDIFF_MAX/size);
			#endif
		} else {
			#ifndef MINGW32_BUILD
			fprintf(stderr, "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n", size*count, available-padding);
			#else
			fprintf(stderr, "[KLS]  Out of memory. size*count [%lli] was bigger than available-padding [%lli].\n", size*count, available-padding);
			#endif
		}
		fprintf(stderr,"[KLS] Failed kls_push_zero() call.\n");
		abort();
		//return 0;
	}
	char* p = kls->data + kls->offset + padding;
	//Zero new area
	memset(p, 0, size*count);
	kls->prev_offset = kls->offset;
	kls->offset += padding + size*count;
	if (KOLISEO_AUTOSET_TEMP_REGIONS == 1) {
		Region* reg = (Region*) malloc(sizeof(Region));
		reg->begin_offset = kls->prev_offset;
		reg->end_offset = kls->offset;
		reg->type = type;
		strcpy(reg->name,name);
		strcpy(reg->desc,desc);
		Region_List reglist = kls_emptyList();
		reglist = kls_cons(reg,reglist);
		t_kls.t_regs = kls_append(reglist, t_kls.t_regs);

		char h_size[200];
		kls_formatSize(size,h_size,sizeof(h_size));
		//sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
		//kls_log("KLS",msg);
		#ifdef KLS_DEBUG_CORE
		kls_log("KLS","API Level { %i } -> Pushed zeroes, size (%s) for Temp_KLS.", int_koliseo_version(), h_size);
		if (KOLISEO_DEBUG == 1) {
			print_kls_2file(KOLISEO_DEBUG_FP,kls);
		}
		#endif
	}
	return p;
}

/**
 * Prints header fields from the passed Koliseo pointer, to the passed FILE pointer.
 * @param kls The Koliseo at hand.
 */
void print_kls_2file(FILE* fp, Koliseo* kls) {
	if (fp == NULL) {
		fprintf(stderr,"print_kls_2file():  fp was NULL.\n");
		exit(EXIT_FAILURE);
	}
	if (kls == NULL) {
		fprintf(fp,"[KLS] kls was NULL.");
	} else {
		fprintf(fp,"\n[KLS] API Level: { %i }\n", int_koliseo_version());
		#ifndef MINGW32_BUILD
		fprintf(fp,"\n[KLS] Size: { %li }\n", kls->size);
		#else
		fprintf(fp,"\n[KLS] Size: { %lli }\n", kls->size);
		#endif
		char human_size[200];
		char curr_size[200];
		kls_formatSize(kls->size,human_size,sizeof(human_size));
		fprintf(fp,"[KLS] Human: { %s }\n", human_size);
		kls_formatSize(kls->offset,curr_size,sizeof(curr_size));
		fprintf(fp,"[KLS] Used (Human): { %s }\n", curr_size);
		#ifndef MINGW32_BUILD
		fprintf(fp,"[KLS] Offset: { %li }\n", kls->offset);
		#else
		fprintf(fp,"[KLS] Offset: { %lli }\n", kls->offset);
		#endif
		#ifndef MINGW32_BUILD
		fprintf(fp,"[KLS] Prev_Offset: { %li }\n\n", kls->prev_offset);
		#else
		fprintf(fp,"[KLS] Prev_Offset: { %lli }\n\n", kls->prev_offset);
		#endif
	}
}

/**
 * Prints header fields from the passed Koliseo pointer, to stderr.
 * @param kls The Koliseo at hand.
 */
void print_dbg_kls(Koliseo* kls) {
  print_kls_2file(stderr,kls);
}

/**
 * Prints header fields from the passed Koliseo_Temp pointer, to the passed FILE pointer.
 * @param t_kls The Koliseo_Temp at hand.
 */
void print_temp_kls_2file(FILE* fp, Koliseo_Temp* t_kls) {
	if (fp == NULL) {
		fprintf(stderr,"print_temp_kls_2file():  fp was NULL.\n");
		exit(EXIT_FAILURE);
	}
	if (t_kls == NULL) {
		fprintf(fp,"[KLS_T] t_kls was NULL.");
	} else if (t_kls->kls == NULL) {
		fprintf(fp,"[KLS_T] [%s()]: Referred Koliseo was NULL.\n",__func__);
	} else {
		Koliseo* kls = t_kls->kls;
		fprintf(fp,"\n[KLS_T] API Level: { %i }\n", int_koliseo_version());
		#ifndef MINGW32_BUILD
		fprintf(fp,"\n[KLS_T] Temp Size: { %li }\n", kls->size - t_kls->offset);
		fprintf(fp,"\n[KLS_T] Refer Size: { %li }\n", kls->size);
		#else
		fprintf(fp,"\n[KLS_T] Temp Size: { %lli }\n", kls->size - t_kls->offset);
		fprintf(fp,"\n[KLS_T] Refer Size: { %lli }\n", kls->size);
		#endif
		char human_size[200];
		char curr_size[200];
		kls_formatSize(kls->size - t_kls->offset,human_size,sizeof(human_size));
		fprintf(fp,"[KLS_T] Temp Size Human: { %s }\n", human_size);
		kls_formatSize(kls->size,human_size,sizeof(human_size));
		fprintf(fp,"[KLS_T] Refer Size Human: { %s }\n", human_size);
		kls_formatSize(kls->offset,curr_size,sizeof(curr_size));
		fprintf(fp,"[KLS_T] Inner Used (Human): { %s }\n", curr_size);
		kls_formatSize(t_kls->offset,curr_size,sizeof(curr_size));
		fprintf(fp,"[KLS_T] Temp Used (Human): { %s }\n", curr_size);
		#ifndef MINGW32_BUILD
		fprintf(fp,"[KLS_T] Inner Offset: { %li }\n", kls->offset);
		fprintf(fp,"[KLS_T] Temp Offset: { %li }\n", t_kls->offset);
		#else
		fprintf(fp,"[KLS_T] Inner Offset: { %lli }\n", kls->offset);
		fprintf(fp,"[KLS_T] Temp Offset: { %lli }\n", t_kls->offset);
		#endif
		#ifndef MINGW32_BUILD
		fprintf(fp,"[KLS_T] Inner Prev_Offset: { %li }\n", kls->prev_offset);
		fprintf(fp,"[KLS_T] Temp Prev_Offset: { %li }\n\n", t_kls->prev_offset);
		#else
		fprintf(fp,"[KLS_T] Inner Prev_Offset: { %lli }\n", kls->prev_offset);
		fprintf(fp,"[KLS_T] Temp Prev_Offset: { %lli }\n\n", t_kls->prev_offset);
		#endif
	}
}

/**
 * Prints header fields from the passed Koliseo_Temp pointer, to stderr.
 * @param t_kls The Koliseo_Temp at hand.
 */
void print_dbg_temp_kls(Koliseo_Temp* t_kls) {
  print_temp_kls_2file(stderr,t_kls);
}

/**
 * Converts a ptrdiff_t size to human-readable SI units (modulo 1000).
 * Fills outputBuffer with the converted string.
 * @param size The size at hand.
 * @param outputBuffer The output buffer.
 * @param bufferSize The output buffer size.
 */
void kls_formatSize(ptrdiff_t size, char* outputBuffer, size_t bufferSize) {
    const char* units[] = {"bytes", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    const int numUnits = sizeof(units) / sizeof(units[0]);

    int unitIndex = 0;
    double sizeValue = (double)size;

    while (sizeValue >= 1000 && unitIndex < numUnits - 1) {
        sizeValue /= 1000;
        unitIndex++;
    }

    snprintf(outputBuffer, bufferSize, "%.2f %s", sizeValue, units[unitIndex]);
}

#ifdef KOLISEO_HAS_CURSES
/**
 * Prints fields and eventually Region_List from the passed Koliseo pointer, to the passed WINDOW pointer.
 * @param kls The Koliseo at hand.
 * @param win The Window at hand.
 */
void kls_show_toWin(Koliseo* kls, WINDOW* win) {
	if (win == NULL) {
		#ifdef KLS_DEBUG_CORE
		kls_log("ERROR","kls_show_toWin():  passed WINDOW was null.");
		#else
		fprintf(stderr,"kls_show_toWin(): passed WINDOW was null.");
		#endif
		abort();
	}
	if (kls == NULL) {
		#ifdef KLS_DEBUG_CORE
		kls_log("ERROR","kls_show_toWin():  passed Koliseo was null.");
		#else
		fprintf(stderr,"kls_show_toWin(): passed Koliseo was null.");
		#endif
		abort();
	}
	wclear(win);
	box(win,0,0);
	wrefresh(win);
	int y = 2;
	int x = 2;
	mvwprintw(win, y++, x, "Koliseo data:");
	mvwprintw(win, y++, x, "API Level: { %i }", int_koliseo_version());
	#ifndef MINGW32_BUILD
	mvwprintw(win, y++, x, "Size: { %li }", kls->size);
	#else
	mvwprintw(win, y++, x, "Size: { %lli }", kls->size);
	#endif
	char h_size[200];
	kls_formatSize(kls->size,h_size,sizeof(h_size));
	mvwprintw(win, y++, x, "Human size: { %s }", h_size);
	char curr_size[200];
	kls_formatSize(kls->offset,curr_size,sizeof(curr_size));
	mvwprintw(win, y++, x, "Used (Human): { %s }\n", curr_size);
	#ifndef MINGW32_BUILD
	mvwprintw(win, y++, x, "Offset: { %li }", kls->offset);
	#else
	mvwprintw(win, y++, x, "Offset: { %lli }", kls->offset);
	#endif
	#ifndef MINGW32_BUILD
	mvwprintw(win, y++, x, "Prev_Offset: { %li }", kls->prev_offset);
	#else
	mvwprintw(win, y++, x, "Prev_Offset: { %lli }", kls->prev_offset);
	#endif
	mvwprintw(win, y++, x, "Region_List len: { %i }", kls_length(kls->regs));
	mvwprintw(win, y++, x, "Current usage: { %.3f%% }", (kls->offset * 100.0 ) / kls->size );
	mvwprintw(win, y++, x, "%s","");
	mvwprintw(win, y++, x, "q or Enter to quit.");
	/*
	Region_List rl = kls_copy(kls->regs);
	while (!kls_empty(rl)) {
	  mvwprintw(win, y, x, "Prev_Offset: [%i]",kls->prev_offset);
	}
	*/
	wrefresh(win);
	int ch = '?';
	int quit = -1;
	do {
		quit = 0;
		ch = wgetch(win);
		switch (ch) {
			case 10: case 'q': {
				quit = 1;
			}
			break;
			default: {
				quit = 0;
			}
			break;
		}
	} while (!quit);
}

/**
 * Takes a Koliseo_Temp pointer and prints fields and eventually Region_List from the referred Koliseo pointer, to the passed WINDOW pointer.
 * @param t_kls The Koliseo_Temp at hand.
 * @param win The Window at hand.
 */
void kls_temp_show_toWin(Koliseo_Temp* t_kls, WINDOW* win) {
	if (win == NULL) {
		#ifdef KLS_DEBUG_CORE
		kls_log("ERROR","kls_temp_show_toWin():  passed WINDOW was null.");
		#else
		fprintf(stderr,"kls_temp_show_toWin(): passed WINDOW was null.");
		#endif
		abort();
	}
	if (t_kls == NULL) {
		#ifdef KLS_DEBUG_CORE
		kls_log("ERROR","kls_temp_show_toWin():  passed Koliseo_Temp was null.");
		#else
		fprintf(stderr,"kls_temp_show_toWin(): passed Koliseo_Temp was null.");
		#endif
		abort();
	}
	Koliseo* kls = t_kls->kls;
	if (kls == NULL) {
		#ifdef KLS_DEBUG_CORE
		kls_log("ERROR","kls_temp_show_toWin():  referred Koliseo was null.");
		#else
		fprintf(stderr,"kls_temp_show_toWin(): referred Koliseo was null.");
		#endif
		abort();
	}
	wclear(win);
	box(win,0,0);
	wrefresh(win);
	int y = 2;
	int x = 2;
	mvwprintw(win, y++, x, "Koliseo_Temp data:");
	mvwprintw(win, y++, x, "API Level: { %i }", int_koliseo_version());
	#ifndef MINGW32_BUILD
	mvwprintw(win, y++, x, "Temp Size: { %li }", kls->size - t_kls->offset);
	mvwprintw(win, y++, x, "Refer Size: { %li }", kls->size);
	#else
	mvwprintw(win, y++, x, "Temp Size: { %lli }", kls->size - t_kls->offset);
	mvwprintw(win, y++, x, "Refer Size: { %lli }", kls->size);
	#endif
	char h_size[200];
	char curr_size[200];
	kls_formatSize(kls->size - t_kls->offset,h_size,sizeof(h_size));
	mvwprintw(win, y++, x, "Temp Human size: { %s }", h_size);
	kls_formatSize(kls->size,h_size,sizeof(h_size));
	mvwprintw(win, y++, x, "Inner Human size: { %s }", h_size);
	kls_formatSize(kls->offset,curr_size,sizeof(curr_size));
	mvwprintw(win, y++, x, "Inner Used (Human): { %s }\n", curr_size);
	kls_formatSize(t_kls->offset,curr_size,sizeof(curr_size));
	mvwprintw(win, y++, x, "Temp Used (Human): { %s }\n", curr_size);
	#ifndef MINGW32_BUILD
	mvwprintw(win, y++, x, "Inner Offset: { %li }", kls->offset);
	mvwprintw(win, y++, x, "Temp Offset: { %li }", t_kls->offset);
	#else
	mvwprintw(win, y++, x, "Inner Offset: { %lli }", kls->offset);
	mvwprintw(win, y++, x, "Temp Offset: { %lli }", t_kls->offset);
	#endif
	#ifndef MINGW32_BUILD
	mvwprintw(win, y++, x, "Inner Prev_Offset: { %li }", kls->prev_offset);
	mvwprintw(win, y++, x, "Temp Prev_Offset: { %li }", t_kls->prev_offset);
	#else
	mvwprintw(win, y++, x, "Inner Prev_Offset: { %lli }", kls->prev_offset);
	mvwprintw(win, y++, x, "Temp Prev_Offset: { %lli }", t_kls->prev_offset);
	#endif
	mvwprintw(win, y++, x, "Refer Region_List len: { %i }", kls_length(kls->regs));
	mvwprintw(win, y++, x, "Temp Region_List len: { %i }", kls_length(t_kls->t_regs));
	mvwprintw(win, y++, x, "Current inner usage: { %.3f%% }", (kls->offset * 100.0 ) / kls->size );
	mvwprintw(win, y++, x, "Current refer usage: { %.3f%% }", (t_kls->offset * 100.0 ) / kls->size );
	mvwprintw(win, y++, x, "%s","");
	mvwprintw(win, y++, x, "q or Enter to quit.");
	/*
	Region_List rl = kls_copy(kls->regs);
	while (!kls_empty(rl)) {
	  mvwprintw(win, y, x, "Prev_Offset: [%i]",kls->prev_offset);
	}
	*/
	wrefresh(win);
	int ch = '?';
	int quit = -1;
	do {
		quit = 0;
		ch = wgetch(win);
		switch (ch) {
			case 10: case 'q': {
				quit = 1;
			}
			break;
			default: {
				quit = 0;
			}
			break;
		}
	} while (!quit);
}

/**
 * Displays a slideshow of Region_List from passed Koliseo, to the passed WINDOW pointer.
 * @param kls The Koliseo at hand.
 * @param win The Window at hand.
 */
void kls_showList_toWin(Koliseo* kls, WINDOW* win) {
	if (win == NULL) {
		#ifdef KLS_DEBUG_CORE
		kls_log("ERROR","kls_showList_toWin():  passed WINDOW was null.");
		#else
		fprintf(stderr,"kls_showList_toWin(): passed WINDOW was null.");
		#endif
		abort();
	}
	if (kls == NULL) {
		#ifdef KLS_DEBUG_CORE
		kls_log("ERROR","kls_showList_toWin():  passed Koliseo was null.");
		#else
		fprintf(stderr,"kls_showList_toWin(): passed Koliseo was null.");
		#endif
		abort();
	}
	wclear(win);
	box(win,0,0);
	wrefresh(win);
	int y = 2;
	int x = 2;
	int quit = 0;
	mvwprintw(win, y++, x, "Region_List data:");
	Region_List rl = kls_copy(kls->regs);
	do {
		wclear(win);
		y = 3;
		element e = kls_head(rl);
		mvwprintw(win, y++, x, "Name: { %s }", e->name);
		mvwprintw(win, y++, x, "Desc: { %s }", e->desc);
		#ifndef MINGW32_BUILD
		mvwprintw(win, y++, x, "Offsets: { %li } -> { %li }", e->begin_offset, e->end_offset);
		#else
		mvwprintw(win, y++, x, "Offsets: { %lli } -> { %lli }", e->begin_offset, e->end_offset);
		#endif
		mvwprintw(win, y++, x, "Region_List len: { %i }", kls_length(kls->regs));
		mvwprintw(win, y++, x, "Current usage: { %.3f%% }", kls_usageShare(e,kls));
		char h_size[200];
		ptrdiff_t reg_size = e->end_offset - e->begin_offset;
		kls_formatSize(reg_size,h_size,sizeof(h_size));
		mvwprintw(win, y++, x, "Human size: { %s }", h_size);
		mvwprintw(win, y++, x, "%s","");
		mvwprintw(win, y++, x, "q to quit, Right arrow to go forward.");
		/*
		Region_List rl = kls_copy(kls->regs);
		while (!kls_empty(rl)) {
		  mvwprintw(win, y, x, "Prev_Offset: [%i]",kls->prev_offset);
		}
		*/
		box(win,0,0);
		wrefresh(win);
		int ch = '?';
		int picked = -1;
		do {
			picked = 0;
			ch = wgetch(win);
			switch (ch) {
				case KEY_RIGHT: {
					rl = kls_tail(rl);
					picked = 1;
				}
				break;
				case 'q': {
					quit = 1;
					picked = 1;
				}
				break;
				default: {
					picked = 0;
				}
				break;
			}
		} while (!quit && !picked);
	} while (!quit && !kls_empty(rl));
}

/**
 * Displays a slideshow of Region_List from passed Koliseo_Temp, to the passed WINDOW pointer.
 * @param t_kls The Koliseo_Temp at hand.
 * @param win The Window at hand.
 */
void kls_temp_showList_toWin(Koliseo_Temp* t_kls, WINDOW* win) {
	if (win == NULL) {
		#ifdef KLS_DEBUG_CORE
		kls_log("ERROR","kls_temp_showList_toWin():  passed WINDOW was null.");
		#else
		fprintf(stderr,"kls_temp_showList_toWin(): passed WINDOW was null.");
		#endif
		abort();
	}
	if (t_kls == NULL) {
		#ifdef KLS_DEBUG_CORE
		kls_log("ERROR","kls_temp_showList_toWin():  passed Koliseo_Temp was null.");
		#else
		fprintf(stderr,"kls_temp_showList_toWin(): passed Koliseo_Temp was null.");
		#endif
		abort();
	}
	wclear(win);
	box(win,0,0);
	wrefresh(win);
	int y = 2;
	int x = 2;
	int quit = 0;
	mvwprintw(win, y++, x, "Region_List data:");
	Region_List rl = kls_copy(t_kls->t_regs);
	do {
		wclear(win);
		y = 3;
		element e = kls_head(rl);
		mvwprintw(win, y++, x, "Name: { %s }", e->name);
		mvwprintw(win, y++, x, "Desc: { %s }", e->desc);
		#ifndef MINGW32_BUILD
		mvwprintw(win, y++, x, "Offsets: { %li } -> { %li }", e->begin_offset, e->end_offset);
		#else
		mvwprintw(win, y++, x, "Offsets: { %lli } -> { %lli }", e->begin_offset, e->end_offset);
		#endif
		mvwprintw(win, y++, x, "Region_List len: { %i }", kls_length(t_kls->t_regs));
		//mvwprintw(win, y++, x, "Current usage: { %.3f%% }", kls_usageShare(e,kls));
		char h_size[200];
		ptrdiff_t reg_size = e->end_offset - e->begin_offset;
		kls_formatSize(reg_size,h_size,sizeof(h_size));
		mvwprintw(win, y++, x, "Human size: { %s }", h_size);
		mvwprintw(win, y++, x, "%s","");
		mvwprintw(win, y++, x, "q to quit, Right arrow to go forward.");
		/*
		Region_List rl = kls_copy(kls->regs);
		while (!kls_empty(rl)) {
		  mvwprintw(win, y, x, "Prev_Offset: [%i]",kls->prev_offset);
		}
		*/
		box(win,0,0);
		wrefresh(win);
		int ch = '?';
		int picked = -1;
		do {
			picked = 0;
			ch = wgetch(win);
			switch (ch) {
				case KEY_RIGHT: {
					rl = kls_tail(rl);
					picked = 1;
				}
				break;
				case 'q': {
					quit = 1;
					picked = 1;
				}
				break;
				default: {
					picked = 0;
				}
				break;
			}
		} while (!quit && !picked);
	} while (!quit && !kls_empty(rl));
}
#endif

/**
 * Resets the offset field for the passed Koliseo pointer.
 * Notably, it sets the prev_offset field to the previous offset, thus remembering where last allocation was before the clear.
 * @param kls The Koliseo at hand.
 */
void kls_clear(Koliseo* kls) {
	//Reset pointer
	kls->prev_offset = kls->offset;
	kls->offset = sizeof(*kls);
	#ifdef KLS_DEBUG_CORE
	kls_log("KLS","API Level { %i } -> Cleared offsets for KLS.", int_koliseo_version());
	#endif
}

/**
 * Calls kls_clear() on the passed Koliseo pointer and the frees the actual Koliseo.
 * @param kls The Koliseo at hand.
 * @see kls_clear()
 */
void kls_free(Koliseo* kls) {
	kls_clear(kls);
	kls_freeList(kls->regs);
	free(kls);
	#ifdef KLS_DEBUG_CORE
	kls_log("KLS","API Level { %i } -> Freed KLS.", int_koliseo_version());
	#endif
}

/**
 * Starts a new savestate for the passed Koliseo pointer, by initialising a Koliseo_temp and returning it.
 * Notably, you should not use the original while using the copy.
 * @param kls The Koliseo at hand.
 * @return A Koliseo_Temp struct.
 * @see Koliseo_Temp
 */
Koliseo_Temp kls_temp_start(Koliseo* kls) {
	assert(kls->has_temp == 0); //TODO handle this more gracefully
	Koliseo_Temp tmp;
	tmp.kls = kls;
	tmp.prev_offset = kls->prev_offset;
	tmp.offset = kls->offset;
	kls->has_temp = 1;
	if (KOLISEO_AUTOSET_TEMP_REGIONS == 1) {
		#ifdef KLS_DEBUG_CORE
		kls_log("KLS","Init of Region_List for temp kls.");
		#endif
		Region* temp_kls_header = (Region*) malloc(sizeof(Region));
		temp_kls_header->begin_offset = tmp.prev_offset;
		temp_kls_header->end_offset = tmp.offset;
		temp_kls_header->type = Temp_KLS_Header;
		strcpy(temp_kls_header->name,"Temp KLS Header");
		strcpy(temp_kls_header->desc,"Denotes last region before starting the Koliseo_Temp.");
		Region_List reglist = kls_emptyList();
		reglist = kls_cons(temp_kls_header,reglist);
		tmp.t_regs = reglist;
		if (tmp.t_regs == NULL) {
		  fprintf(stderr,"[KLS] kls_temp_start() failed to get a Region_List.\n");
		  abort();
		}
	}
	#ifdef KLS_DEBUG_CORE
	kls_log("KLS","Prepared new Temp KLS.");
	#endif
	return tmp;
}

/**
 * Ends a new savestate for the passed Koliseo pointer, by initialising a Koliseo_temp and returning it.
 * Notably, you should not use the original while using the copy.
 * @param kls The Koliseo at hand.
 */
void kls_temp_end(Koliseo_Temp tmp_kls) {
	tmp_kls.kls->prev_offset = tmp_kls.prev_offset;
	tmp_kls.kls->offset = tmp_kls.offset;
	if (KOLISEO_AUTOSET_TEMP_REGIONS == 1) {
		kls_freeList(tmp_kls.t_regs);
	}
	#ifdef KLS_DEBUG_CORE
	kls_log("KLS","Ended Temp KLS.");
	#endif
	tmp_kls.kls->has_temp = 0;
}


Region_List kls_emptyList(void)
{
	return NULL;
}
bool kls_empty(Region_List l) {
	if (l==NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}
element kls_head(Region_List l) {
	if (kls_empty(l))
	{
		abort();
	}
	else
	{
		return l->value;
	}
}
Region_List kls_tail(Region_List l) {
	if (kls_empty(l))
	{
		abort();
	}
	else
	{
		return l->next;
	}
}
Region_List kls_cons(element e, Region_List l) {
	if (e == NULL) {
	  kls_log("KLS","kls_cons():  element was NULL");
	}
	Region_List t;
	t = (Region_List)malloc(sizeof(region_list_item));
	t->value = e;
	t->next = l;
	return t;
}

void kls_freeList(Region_List l) {
	if (kls_empty(l))
	{
		return;
	}
	else
	{
		kls_freeList(kls_tail(l));
		#ifdef KLS_DEBUG_CORE
		kls_log("KLS","Freeing Region_List->value");
		#endif
		free(l->value);
		#ifdef KLS_DEBUG_CORE
		kls_log("KLS","Freeing Region_List");
		#endif
		free(l);
	}
	return;
}

void kls_showList_toFile(Region_List l, FILE* fp) {
	if (fp == NULL) {
		fprintf(stderr,"[KLS]  kls_showList_toFile():  passed file was NULL.\n");
		abort();
	}
	fprintf(fp,"{");
	while (!kls_empty(l))
	{
		fprintf(fp,"\n{ %s }, { %s }    ",kls_head(l)->name,kls_head(l)->desc);
		#ifndef MINGW32_BUILD
		fprintf(fp,"{ %li } -> { %li }",kls_head(l)->begin_offset,kls_head(l)->end_offset);
		#else
		fprintf(fp,"{ %lli } -> { %lli }",kls_head(l)->begin_offset,kls_head(l)->end_offset);
		#endif
		#ifdef KLS_DEBUG_CORE
		kls_log("KLS-Region","    Region {");
		kls_log("KLS-Region","{ %s }, { %s }",kls_head(l)->name,kls_head(l)->desc);
		char h_size[200];
		ptrdiff_t r_size = kls_head(l)->end_offset - kls_head(l)->begin_offset;
		kls_formatSize(r_size,h_size,sizeof(h_size));
		kls_log("KLS-Region","{ %s }",h_size);
		kls_log("KLS-Region","{ %li } -> { %li }",kls_head(l)->begin_offset,kls_head(l)->end_offset);
		kls_log("KLS-Region","    }");
		#endif

		l = kls_tail(l);
		if (!kls_empty(l))
		{
			fprintf(fp,",\n");
		}
	}
	fprintf(fp,"\n}\n");
}

void kls_showList(Region_List l) {
	kls_showList_toFile(l,stdout);
}

bool kls_member(element el, Region_List l) {
	if (kls_empty(l))
	{
		return false;
	}
	else
	{
		if (el == kls_head(l))
		{
			return true;
		}
		else
		{
			return kls_member(el, kls_tail(l));
		}
	}
}
int kls_length(Region_List l) {
	if (kls_empty(l))
	{
		return 0;
	}
	else
	{
		return 1 + kls_length(kls_tail(l));
	}
}
Region_List kls_append(Region_List l1, Region_List l2) {
	if (kls_empty(l1))
	{
		return l2;
	}
	else
	{
		return kls_cons(kls_head(l1), kls_append(kls_tail(l1), l2));
	}
}
Region_List kls_reverse(Region_List l) {
	if (kls_empty(l))
	{
		return kls_emptyList();
	}
	else
	{
		return kls_append(kls_reverse(kls_tail(l)), kls_cons(kls_head(l), kls_emptyList()));
	}
}
Region_List kls_copy(Region_List l) {
	if (kls_empty(l))
	{
		return l;
	}
	else
	{
		return kls_cons(kls_head(l), kls_copy(kls_tail(l)));
	}
}
Region_List kls_delete(element el, Region_List l) {
	if (kls_empty(l))
	{
		return kls_emptyList();
	}
	else
	{
		if (el == kls_head(l))
		{
			return kls_tail(l);
		}
		else
		{
			return kls_cons(kls_head(l), kls_delete(el, kls_tail(l)));
		}
	}
}

Region_List kls_insord(element el, Region_List l) {
	if (kls_empty(l))
	{
		return kls_cons(el, l);
	}
	else
	{
		//Insert element according to its begin_offset
		if (el->begin_offset <= kls_head(l)->begin_offset)
		{
			return kls_cons(el, l);
		}
		else
		{
			return kls_cons(kls_head(l),kls_insord(el, kls_tail(l)));
		}
	}
}

Region_List kls_insord_p(element el, Region_List l) {
	Region_List pprec, patt = l, paux;
	bool found = false;
	pprec = NULL;

	while (patt != NULL && !found)
	{
		if (el < patt->value)
		{
			found = true;
		}
		else
		{
			pprec = patt; patt = patt->next;
		}
	}
	paux = (Region_List) malloc(sizeof(region_list_item));
	paux->value = el;
	paux->next = patt;
	if (patt == l)
	{
		return paux;
	}
	else
	{
		pprec->next = paux;
		return l;
	}
}
Region_List kls_mergeList(Region_List l1, Region_List l2) {
	if (kls_empty(l1))
	{
		return l2;
	}
	else
	{
		if (kls_empty(l2))
		{
			return l1;
		}
		else
		{
			if (kls_isLess(kls_head(l1), kls_head(l2)))
			{
				return kls_cons(kls_head(l1), kls_mergeList(kls_tail(l1), l2));
			}
			else
			{
				if (kls_isEqual(kls_head(l1), kls_head(l2)))
				{
					return kls_cons(kls_head(l1), kls_mergeList(kls_tail(l1), kls_tail(l2)));
				}
				else
				{
					return kls_cons(kls_head(l2), kls_mergeList(l1, kls_tail(l2)));
				}
			}
		}
	}
}
Region_List kls_intersect(Region_List l1, Region_List l2) {
	if (kls_empty(l1) || kls_empty(l2))
	{
		return kls_emptyList();
	}

	if (kls_member(kls_head(l1), l2) && !kls_member(kls_head(l1), kls_tail(l1)))
	{
		return kls_cons(kls_head(l1), kls_intersect(kls_tail(l1), l2));
	}

	else
	{
		return kls_intersect(kls_tail(l1), l2);
	}
}
Region_List kls_diff(Region_List l1, Region_List l2) {
	if (kls_empty(l1) || kls_empty(l2))
	{
		return l1;
	}

	else
	{
		if (!kls_member(kls_head(l1), l2) && !kls_member(kls_head(l1), kls_tail(l1)))
		{
			return kls_cons(kls_head(l1), kls_diff(kls_tail(l1), l2));
		}
		else
		{
			return kls_diff(kls_tail(l1), l2);
		}
	}
}

bool kls_isLess(Region* r1, Region* r2) {
  //Compare regions by their effective size
  ptrdiff_t s1 = r1->end_offset - r1->begin_offset;
  ptrdiff_t s2 = r2->end_offset - r2->begin_offset;
  return (s1 < s2);
}

bool kls_isEqual(Region* r1, Region* r2) {
  //Compare regions by their effective size
  ptrdiff_t s1 = r1->end_offset - r1->begin_offset;
  ptrdiff_t s2 = r2->end_offset - r2->begin_offset;
  return (s1 == s2);
}

double kls_usageShare(Region* r, Koliseo* kls) {
	if (r == NULL) {
		kls_log("ERROR","kls_usageShare():  passed Region was NULL");
		return -1;
	}
	if (kls == NULL) {
		kls_log("ERROR","kls_usageShare():  passed Koliseo was NULL");
		return -1;
	}
	ptrdiff_t r_size = r->end_offset - r->begin_offset;
	double res = (r_size * 100.0) / kls->size;
	return res;
}

void kls_usageReport_toFile(Koliseo* kls, FILE* fp) {
	if (fp == NULL) {
		kls_log("ERROR","kls_usageReport_toFile():  passed file was NULL");
		return;
	}
	if (kls == NULL) {
		kls_log("ERROR","kls_usageReport_toFile():  passed Koliseo was NULL");
		return;
	}
	Region_List rl = kls_copy(kls->regs);
	int i = 0;
	while(!kls_empty(rl)) {
		fprintf(fp,"Usage for region (%i) [%s]:  [%.3f%%]\n", i, rl->value->name, kls_usageShare(rl->value,kls));
		rl = kls_tail(rl);
		i++;
	}
}

void kls_usageReport(Koliseo* kls) {
	kls_usageReport_toFile(kls,stdout);
}

ptrdiff_t kls_type_usage(int type, Koliseo* kls) {
	Region_List rl = kls_copy(kls->regs);

	ptrdiff_t res = 0;

	while (!kls_empty(rl)) {
		element h = kls_head(rl);
		if (h->type == type) {
			res += (h->end_offset - h->begin_offset);
		}
		rl = kls_tail(rl);
	}

	return res;
}
