// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only

#include <stdio.h>
#include <locale.h>
#include "../src/koliseo.h"
#include "amboso.h"

void usage(char *progname)
{
    fprintf(stderr, "Usage:  %s [-a]\n\n", progname);
    fprintf(stderr, "  [-a]  Makes the demo not interactive.\n");
}

int main(int argc, char **argv)
{

    int is_interactive = 1;

    if (argc > 2) {
	fprintf(stderr, "Too many arguments.\n");
	usage(argv[0]);
	exit(EXIT_FAILURE);
    } else if (argc == 2) {
	if ((strcmp(argv[1], "-a")) == 0) {
	    is_interactive = 0;
	} else {
	    fprintf(stderr, "Invalid argument: [%s].\n", argv[1]);
	    usage(argv[0]);
	    exit(EXIT_FAILURE);
	}
    }

    kls_print_title();
    printf("\n\nDemo for Koliseo, using API lvl [%i], version %s \n",
	   int_koliseo_version(), string_koliseo_version());
    printf("Supporting Amboso API version %s\n\n", getAmbosoVersion());

    KLS_Conf kls_config = {
#ifdef KOLISEO_HAS_REGION
	.kls_autoset_regions = 1,
	//.kls_reglist_alloc_backend = KLS_REGLIST_ALLOC_LIBC,
	.kls_reglist_alloc_backend = KLS_REGLIST_ALLOC_KLS_BASIC,
	.kls_reglist_kls_size = KLS_DEFAULT_SIZE,
	.kls_autoset_temp_regions = 1,
#endif //KOLISEO_HAS_REGION
	.kls_collect_stats = 1,
	.kls_log_filepath = "./static/debug_log.txt",
	.kls_verbose_lvl = 1,
    };
    printf("[Init Koliseo] [size: %i]\n", KLS_DEFAULT_SIZE);
    Koliseo *kls = kls_new_conf(KLS_DEFAULT_SIZE, kls_config);

    // TODO: add macro/function to wrap the string dupe use
    char* demo_str = KLS_PUSH_STR(kls, "\nKoliseo demo\n");
    strcpy(demo_str, "\nKoliseo demo\n");

    printf("%s\n", demo_str);

#ifdef KOLISEO_HAS_REGION
#ifndef _WIN32
    printf("kls size: (%li) kls_region size: (%li)\n", kls->size,
	   sizeof(KLS_Region));
    printf
	("Max KLS_Region on this size, when reglist alloc backend is KLS_BASIC: %i\n",
	 kls_get_maxRegions_KLS_BASIC(kls));
#else
    printf("kls size: (%lli) kls_region size: (%lli)\n", kls->size,
	   sizeof(KLS_Region));
    printf
	("Max KLS_Region on this size, when reglist alloc backend is KLS_BASIC: %i\n",
	 kls_get_maxRegions_KLS_BASIC(kls));
#endif
#endif // KOLISEO_HAS_REGION

    printf(KLS_Conf_Fmt "\n", KLS_Conf_Arg(kls->conf));

#ifndef WINDOWS_BUILD
    printf("[Current position in Koliseo] [pos: %li]\n", kls_get_pos(kls));
#else
    printf("[Current position in Koliseo] [pos: %lli]\n", kls_get_pos(kls));
#endif

    print_dbg_kls(kls);


#ifdef KOLISEO_HAS_REGION
#ifndef WINDOWS_BUILD
    printf("[Show Region list for Koliseo] [pos: %li]\n", kls_get_pos(kls));
#else
    printf("[Show Region list for Koliseo] [pos: %lli]\n", kls_get_pos(kls));
#endif

    KLS_ECHOLIST(kls->regs);
#endif // KOLISEO_HAS_REGION


    Koliseo_Temp *temp_kls = kls_temp_start(kls);
    //temp_kls->conf.kls_autoset_regions = 1; TODO why does this crash?

#ifndef WINDOWS_BUILD
    printf("[Started Koliseo_Temp] [pos: %li]\n", kls_get_pos(temp_kls->kls));
#else
    printf("[Started Koliseo_Temp] [pos: %lli]\n", kls_get_pos(temp_kls->kls));
#endif

#ifdef KOLISEO_HAS_REGION
    // TODO: still declare KLS_Temp_Conf
    printf(KLS_Temp_Conf_Fmt "\n", KLS_Temp_Conf_Arg(temp_kls->conf));
#endif

    int minusone = -1;
    int *p = &minusone;
    int *p2 = &minusone;
    int *p3 = &minusone;
    printf("\n*p is [%i] before KLS_PUSH\n", *p);
    printf("\n*p2 is [%i] before KLS_PUSH_T\n", *p2);
    printf("\n*p3 is [%i] before KLS_PUSH_T\n", *p3);

#ifndef WINDOWS_BUILD
    printf("[KLS_PUSH for a int to Koliseo] [size: %li]\n", sizeof(int));
#else
    printf("[KLS_PUSH for a int to Koliseo] [size: %lli]\n", sizeof(int));
#endif

    printf
	("[This handles the Koliseo directly while we have an open Koliseo_Temp.]\n");
    p = (int *)KLS_PUSH(kls, int);

#ifdef KOLISEO_HAS_REGION
#ifndef WINDOWS_BUILD
    printf("[KLS_PUSH_T_NAMED for a int to Koliseo_Temp] [size: %li]\n",
	   sizeof(int));
#else
    printf("[KLS_PUSH_T_NAMED for a int to Koliseo_Temp] [size: %lli]\n",
	   sizeof(int));
#endif

    p2 = (int *)KLS_PUSH_T_NAMED(temp_kls, int, "int", "Another int");
#endif // KOLISEO_HAS_REGION
#ifndef WINDOWS_BUILD
    printf("[KLS_PUSH_T for a int to Koliseo_Temp] [size: %li]\n", sizeof(int));
#else
    printf("[KLS_PUSH_T for a int to Koliseo_Temp] [size: %lli]\n",
	   sizeof(int));
#endif
    p3 = (int *)KLS_PUSH_T(temp_kls, int);

#ifndef WINDOWS_BUILD
    printf("[Current position in Koliseo] [pos: %li]\n", kls_get_pos(kls));
#else
    printf("[Current position in Koliseo] [pos: %lli]\n", kls_get_pos(kls));
#endif

#ifndef WINDOWS_BUILD
    printf("[Current position in Koliseo_Temp] [pos: %li]\n", temp_kls->offset);
#else
    printf("[Current position in Koliseo_Temp] [pos: %lli]\n",
	   temp_kls->offset);
#endif

    print_dbg_kls(kls);

    *p = 1;
    printf("\n*p is [%i] after KLS_PUSH\n", *p);
    *p2 = 2;
    printf("\n*p2 is [%i] after KLS_PUSH\n", *p2);
    *p2 = 3;
    printf("\n*p3 is [%i] after KLS_PUSH\n", *p3);

#ifdef KOLISEO_HAS_REGION
#ifndef WINDOWS_BUILD
    printf("[Show Region list for Koliseo] [pos: %li]\n", kls_get_pos(kls));
#else
    printf("[Show Region list for Koliseo] [pos: %lli]\n", kls_get_pos(kls));
#endif

    KLS_ECHOLIST(kls->regs);
    printf("[%i] List size\n", kls_length(kls->regs));

    KLS_ECHOLIST(temp_kls->t_regs);
    printf("[%i] Temp List size\n", kls_length(temp_kls->t_regs));

    printf("[Usage report for Koliseo]\n");
    kls_usageReport(kls);
#endif // KOLISEO_HAS_REGION

#ifdef KOLISEO_HAS_CURSES
    if (is_interactive == 1) {
	WINDOW *win = NULL;
	/* Initialize curses */
	setlocale(LC_ALL, "");
	initscr();
	clear();
	refresh();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	win = newwin(22, 60, 1, 2);
	keypad(win, TRUE);
	wclear(win);
	wrefresh(win);
	kls_show_toWin(kls, win);
	kls_temp_show_toWin(temp_kls, win);
	refresh();
#ifdef KOLISEO_HAS_REGION
	kls_showList_toWin(kls, win);
	kls_temp_showList_toWin(temp_kls, win);
#endif // KOLISEO_HAS_REGION
	delwin(win);
	endwin();
    }
#endif

    int *z = &minusone;
    printf("\n*z is [%i] before KLS_POP\n", *z);

#ifndef WINDOWS_BUILD
    printf("[KLS_POP a int from Koliseo] [size: %li]\n", sizeof(int));
#else
    printf("[KLS_POP a int from Koliseo] [size: %lli]\n", sizeof(int));
#endif

    z = KLS_POP(kls, int);

    printf("\n*z is [%i] after KLS_POP\n", *z);

#ifndef WINDOWS_BUILD
    printf("[Current position in Koliseo] [pos: %li]\n", kls_get_pos(kls));
#else
    printf("[Current position in Koliseo] [pos: %lli]\n", kls_get_pos(kls));
#endif

    print_dbg_kls(kls);
    print_dbg_temp_kls(temp_kls);

    //We may forget to end the Koliseo_Temp...
    //kls_temp_end(temp_kls);
    //printf("[Ended Koliseo_Temp]\n");

    print_dbg_kls(kls);

    printf(KLSFmt "\n", KLS_Arg(kls));
    printf(KLS_Stats_Fmt "\n", KLS_Stats_Arg(kls->stats));

    printf("[Koliseo Info]\n");
    print_dbg_kls(kls);
#ifdef KOLISEO_HAS_REGION
    print_dbg_kls(kls->reglist_kls);
    KLS_ECHOLIST(kls->regs);
    printf("[Koliseo_Temp Info]\n");
    KLS_ECHOLIST(temp_kls->t_regs);
    print_dbg_kls(temp_kls->reglist_kls);
    printf("[%i] List size for Koliseo\n", kls_length(kls->regs));
    printf("[%i] List size for Koliseo_Temp\n", kls_length(temp_kls->t_regs));
#endif // KOLISEO_HAS_REGION
    printf("[Clear Koliseo]\n");
    kls_clear(kls);
    print_dbg_kls(kls);

    //This should also clean up an eventual Koliseo_Temp
    printf("[Free Koliseo]\n");
    kls_free(kls);

    printf("[End of demo]\n");
    printf("[End of demo for Koliseo, API lvl [%i] v%s ]\n",
	   int_koliseo_version(), string_koliseo_version());

    printf("Press Enter to quit.\n");
    int sc_res = -1;
    if (is_interactive == 1) {
	sc_res = scanf("%*c");
    } else {
	sc_res = 0;
    }

    return sc_res;
}
