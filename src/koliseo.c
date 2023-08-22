#include "koliseo.h"
int KOLISEO_DEBUG = 0;
FILE* KOLISEO_DEBUG_FP = NULL;

/**
 * Returns the constant string representing current version for Koliseo.
 * @return A constant string in MAJOR-MINOR-PATCH format for current Koliseo version.
 */
const char* string_koliseo_version(void) {
	return KOLISEO_API_VERSION_STRING;
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
 * @param msg The actual message.
 */
void kls_log(const char* tag, const char* msg) {
	if (KOLISEO_DEBUG == 1) {
		if (KOLISEO_DEBUG_FP == NULL) {
			fprintf(stderr,"[KLS]    kls_log(): Failed opening KOLISEO_DEBUG_FP to print logs.\n");
		} else {
			time_t now = time(0);
			struct tm *mytime = localtime(&now);
			char timeheader[500];
			if ( strftime(timeheader, sizeof timeheader, "%X", mytime) )
			{
				fprintf(KOLISEO_DEBUG_FP,"[%-10.10s] [%s] [%s]\n", tag, timeheader, msg);
			}
		}
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
		char msg[500];
		sprintf(msg,"Allocated (%li) for new KLS.",size);
		kls_log("KLS",msg);
		Koliseo* kls = p;
		kls->data = p;
		kls->size = size;
		kls->offset = sizeof(*kls);
		kls->prev_offset = kls->offset;
	} else {
		fprintf(stderr,"[KLS] Failed kls_new() call.\n");
		abort();
	}
	if (KOLISEO_DEBUG == 1) {
		print_kls_2file(KOLISEO_DEBUG_FP,p);
	}
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
	char msg[500];
	sprintf(msg,"Popped (%li) for KLS.",size);
	kls_log("KLS",msg);
	if (KOLISEO_DEBUG == 1) {
		print_kls_2file(KOLISEO_DEBUG_FP,kls);
	}
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
			fprintf(stderr, "[KSL]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n", count, PTRDIFF_MAX/size);
		} else {
			fprintf(stderr, "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n", size*count, available-padding);
		}
		fprintf(stderr,"[KLS] Failed kls_push() call.\n");
		abort();
	}
	char* p = kls->data + kls->offset + padding;
	kls->prev_offset = kls->offset;
	kls->offset += padding + size*count;
	char msg[500];
	sprintf(msg,"Pushed (%li) for KLS.",size);
	kls_log("KLS",msg);
	if (KOLISEO_DEBUG == 1) {
		print_kls_2file(KOLISEO_DEBUG_FP,kls);
	}
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
			fprintf(stderr, "[KSL]  count [%li] was bigger than PTRDIFF_MAX/size [%li].\n", count, PTRDIFF_MAX/size);
		} else {
			fprintf(stderr, "[KLS]  Out of memory. size*count [%li] was bigger than available-padding [%li].\n", size*count, available-padding);
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
	char msg[500];
	sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
	kls_log("KLS",msg);
	if (KOLISEO_DEBUG == 1) {
		print_kls_2file(KOLISEO_DEBUG_FP,kls);
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
		fprintf(fp,"\n[KLS] Size: [%li]\n", kls->size);
		fprintf(fp,"[KLS] Offset: [%li]\n", kls->offset);
		fprintf(fp,"[KLS] Prev_Offset: [%li]\n\n", kls->prev_offset);
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
 * Resets the offset field for the passed Koliseo pointer.
 * Notably, it sets the prev_offset field to the previous offset, thus remembering where last allocation was before the clear.
 * @param kls The Koliseo at hand.
 */
void kls_clear(Koliseo* kls) {
	//Reset pointer
	kls->prev_offset = kls->offset;
	kls->offset = sizeof(*kls);
	char msg[500];
	sprintf(msg,"Cleared offsets for KLS.");
	kls_log("KLS",msg);
}

/**
 * Calls kls_clear() on the passed Koliseo pointer and the frees the actual Koliseo.
 * @param kls The Koliseo at hand.
 * @see kls_clear()
 */
void kls_free(Koliseo* kls) {
	kls_clear(kls);
	free(kls);
	char msg[500];
	sprintf(msg,"Freed KLS.");
	kls_log("KLS",msg);
}

/**
 * Starts a new savestate for the passed Koliseo pointer, by initialising a Koliseo_temp and returning it.
 * Notably, you should not use the original while using the copy.
 * @param kls The Koliseo at hand.
 * @return A Koliseo_Temp struct.
 * @see Koliseo_Temp
 */
Koliseo_Temp kls_temp_start(Koliseo* kls) {
	Koliseo_Temp tmp;
	tmp.kls = kls;
	tmp.prev_offset = kls->prev_offset;
	tmp.offset = kls->offset;
	char msg[500];
	sprintf(msg,"Prepared new Temp KLS.");
	kls_log("KLS",msg);
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
	char msg[500];
	sprintf(msg,"Ended Temp KLS.");
	kls_log("KLS",msg);
}
