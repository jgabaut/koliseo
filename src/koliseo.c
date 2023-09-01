#include "koliseo.h"
//Default settings for global vars.
int KOLISEO_DEBUG = 0;
int KOLISEO_AUTOSET_REGIONS = 1;
FILE* KOLISEO_DEBUG_FP = NULL;

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
		kls_log("KLS","Allocated (%s) for new KLS.",h_size);
		Koliseo* kls = p;
		kls->data = p;
		kls->size = size;
		kls->offset = sizeof(*kls);
		kls->prev_offset = kls->offset;
		if (KOLISEO_AUTOSET_REGIONS == 1) {
			kls_log("KLS","Init of Region_List for kls.");
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
	kls_log("KLS","Popped (%li) for KLS.",size);
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
	char h_size[200];
	kls_formatSize(size,h_size,sizeof(h_size));
	//sprintf(msg,"Pushed size (%li) for KLS.",size);
	//kls_log("KLS",msg);
	kls_log("KLS","Pushed size (%s) for KLS.",h_size);
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
	if (KOLISEO_AUTOSET_REGIONS == 1) {
		Region* reg = (Region*) malloc(sizeof(Region));
		reg->begin_offset = kls->prev_offset;
		reg->end_offset = kls->offset;
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
	kls_log("KLS","Pushed zeroes, size (%s) for KLS.",h_size);
	if (KOLISEO_DEBUG == 1) {
		print_kls_2file(KOLISEO_DEBUG_FP,kls);
	}
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
	strcpy(reg->name,name);
	strcpy(reg->desc,desc);
	Region_List reglist = kls_emptyList();
	reglist = kls_cons(reg,reglist);
	kls->regs = kls_append(reglist, kls->regs);

	char h_size[200];
	kls_formatSize(size,h_size,sizeof(h_size));
	//sprintf(msg,"Pushed zeroes, size (%li) for KLS.",size);
	//kls_log("KLS",msg);
	kls_log("KLS","Pushed zeroes, size (%s) for KLS.",h_size);
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
		fprintf(fp,"\n[KLS] Size: { %li }\n", kls->size);
		char human_size[200];
		char curr_size[200];
		kls_formatSize(kls->size,human_size,sizeof(human_size));
		fprintf(fp,"[KLS] Human: { %s }\n", human_size);
		kls_formatSize(kls->offset,curr_size,sizeof(curr_size));
		fprintf(fp,"[KLS] Used (Human): { %s }\n", curr_size);
		fprintf(fp,"[KLS] Offset: { %li }\n", kls->offset);
		fprintf(fp,"[KLS] Prev_Offset: { %li }\n\n", kls->prev_offset);
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
		kls_log("ERROR","kls_show_toWin():  passed WINDOW was null.");
		abort();
	}
	if (kls == NULL) {
		kls_log("ERROR","kls_show_toWin():  passed Koliseo was null.");
		abort();
	}
	wclear(win);
	box(win,0,0);
	wrefresh(win);
	int y = 2;
	int x = 2;
	mvwprintw(win, y++, x, "Koliseo data:");
	mvwprintw(win, y++, x, "Size: { %li }", kls->size);
	char h_size[200];
	kls_formatSize(kls->size,h_size,sizeof(h_size));
	mvwprintw(win, y++, x, "Human size: { %s }", h_size);
	char curr_size[200];
	kls_formatSize(kls->offset,curr_size,sizeof(curr_size));
	mvwprintw(win, y++, x, "Used (Human): { %s }\n", curr_size);
	mvwprintw(win, y++, x, "Offset: { %li }", kls->offset);
	mvwprintw(win, y++, x, "Prev_Offset: { %li }", kls->prev_offset);
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
 * Displays a slideshow of Region_List from passed Koliseo, to the passed WINDOW pointer.
 * @param kls The Koliseo at hand.
 * @param win The Window at hand.
 */
void kls_showList_toWin(Koliseo* kls, WINDOW* win) {
	if (win == NULL) {
		kls_log("ERROR","kls_show_toWin():  passed WINDOW was null.");
		abort();
	}
	if (kls == NULL) {
		kls_log("ERROR","kls_show_toWin():  passed Koliseo was null.");
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
		mvwprintw(win, y++, x, "Offsets: { %li } -> { %li }", e->begin_offset, e->end_offset);
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
	kls_log("KLS","Cleared offsets for KLS.");
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
	kls_log("KLS","Freed KLS.");
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
	kls_log("KLS","Prepared new Temp KLS.");
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
	kls_log("KLS","Ended Temp KLS.");
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
		kls_log("KLS","Freeing Region_List->value");
		free(l->value);
		kls_log("KLS","Freeing Region_List");
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
		fprintf(fp,"{ %li } -> { %li }",kls_head(l)->begin_offset,kls_head(l)->end_offset);
		kls_log("KLS-Region","    Region {");
		kls_log("KLS-Region","{ %s }, { %s }",kls_head(l)->name,kls_head(l)->desc);
		char h_size[200];
		ptrdiff_t r_size = kls_head(l)->end_offset - kls_head(l)->begin_offset;
		kls_formatSize(r_size,h_size,sizeof(h_size));
		kls_log("KLS-Region","{ %s }",h_size);
		kls_log("KLS-Region","{ %li } -> { %li }",kls_head(l)->begin_offset,kls_head(l)->end_offset);
		kls_log("KLS-Region","    }");

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
