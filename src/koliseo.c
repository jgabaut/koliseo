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
		sprintf(msg,"Init of Region_List for kls.");
		kls_log("KLS",msg);
		Region* kls_header = (Region*) malloc(sizeof(Region));
		kls_header->begin_offset = 0;
		kls_header->end_offset = kls->offset;
		strcpy(kls_header->name,"KLS Header");
		strcpy(kls_header->desc,"Denotes Space occupied by the Koliseo header.");
		Region_List reglist = kls_emptyList();
		reglist = kls_cons(kls_header,reglist);
		kls->regs = reglist;
		if (kls->regs == NULL) {
		  fprintf(stderr,"[KLS] kls_new() failed to get a Region_List.\n");
		  abort();
		}
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
	Region* reg = (Region*) malloc(sizeof(Region));
	reg->begin_offset = kls->prev_offset;
	reg->end_offset = kls->offset;
	strcpy(reg->name,"KLS");
	strcpy(reg->desc,"KLS");
	Region_List reglist = kls_emptyList();
	reglist = kls_cons(reg,reglist);
	kls->regs = kls_append(reglist, kls->regs);

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
		free(l);
	}
	return;
}

void kls_showList(Region_List l) {
	char msg[1000];
	printf("[");
	while (!kls_empty(l))
	{
		printf("--BEGIN Region--\n\n");
		printf("Begin [%li] End [%li]\n",kls_head(l)->begin_offset,kls_head(l)->end_offset);
		printf("Name [%s] Desc [%s]",kls_head(l)->name,kls_head(l)->desc);
		printf("\n\n--END Region--");
		kls_log("KLS","--BEGIN Region--");
		sprintf(msg,"Begin [%li] End [%li]",kls_head(l)->begin_offset,kls_head(l)->end_offset);
		kls_log("KLS",msg);
		sprintf(msg,"Name [%s] Desc [%s]",kls_head(l)->name,kls_head(l)->desc);
		kls_log("KLS",msg);
		kls_log("KLS","--END Region--");
		kls_log("KLS",msg);
		//TODO
		//Print containing kls
		//print_kls_2file(stdout,kls_head(l));
		l = kls_tail(l);
		if (!kls_empty(l))
		{
			printf(",\n");
		}
	}
	printf("]\n");
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
int kls_lenght(Region_List l) {
	if (kls_empty(l))
	{
		return 0;
	}
	else
	{
		return 1 + kls_lenght(kls_tail(l));
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
Region_List kls_delet(element el, Region_List l) {
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
			return kls_cons(kls_head(l), kls_delet(el, kls_tail(l)));
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
  return (r1->begin_offset < r2->begin_offset);
}

bool kls_isEqual(Region* r1, Region* r2) {
  return (r1->begin_offset == r2->begin_offset);
}
