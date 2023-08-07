#include "koliseo.h"

const char* string_koliseo_version(void) {
	return KOLISEO_API_VERSION_STRING;
}

ptrdiff_t kls_get_pos(Koliseo* kls) {
	return kls->offset;
}

Koliseo* kls_new(ptrdiff_t size) {
	assert(size >= (ptrdiff_t)sizeof(Koliseo));
	void *p = malloc(size);
	if (p) {
		Koliseo* kls = p;
		kls->data = p;
		kls->size = size;
		kls->offset = sizeof(*kls);
		kls->prev_offset = kls->offset;
	} else {
		fprintf(stderr,"[KLS] Failed kls_new() call.\n");
		abort();
	}
	return p;
}

void* kls_pop(Koliseo* kls, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count) {
	ptrdiff_t padding = -kls->offset & (align -1);
	if (count > PTRDIFF_MAX/size || (kls->size + kls->offset) < (size*count)) {
		fprintf(stderr,"[KLS] Failed kls_pop() call.\n");
		abort();
		//return 0;
	}
	char* p = kls->data + kls->offset - padding - size*count;
	kls->offset -= padding + size*count;
	return p;
}

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
		//return 0;
	}
	char* p = kls->data + kls->offset + padding;
	kls->offset += padding + size*count;
	return p;
}

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
	kls->offset += padding + size*count;
	return p;
}

void print_dbg_kls(Koliseo* kls) {
	if (kls == NULL) {
		fprintf(stderr,"[KLS] kls was NULL.");
	} else {
		printf("\n[KLS] Size: [%li]\n", kls->size);
		printf("[KLS] Offset: [%li]\n", kls->offset);
		printf("[KLS] Prev_Offset: [%li]\n\n", kls->prev_offset);
	}
}

void kls_clear(Koliseo* kls) {
	//Reset pointer
	kls->prev_offset = kls->offset;
	kls->offset = sizeof(*kls);
}

void kls_free(Koliseo* kls) {
	kls_clear(kls);
	free(kls);
}
