#include <stdio.h>
#include <locale.h>
#include "../src/koliseo.h"
#include "amboso.h"


int main(void) {
  KOLISEO_DEBUG = 1;
  KOLISEO_AUTOSET_REGIONS = 1;
  kls_print_title();
  printf("\n\nDemo for Koliseo, using API lvl [%i], version %s \n", int_koliseo_version(), string_koliseo_version());
  printf("Supporting Amboso API version %s\n\n", getAmbosoVersion());
  printf("KOLISEO_DEBUG is [%i]\n\n", KOLISEO_DEBUG);
  printf("KOLISEO_AUTOSET_REGIONS is [%i]\n\n", KOLISEO_AUTOSET_REGIONS);
  //Reset debug log file
  if (KOLISEO_DEBUG == 1) {
	  KOLISEO_DEBUG_FP = fopen("./static/debug_log.txt","w");
	  if (KOLISEO_DEBUG_FP == NULL) {
		fprintf(stderr,"[KLS]    Failed to open debug logfile.\n");
		exit(EXIT_FAILURE);
	  }
	  kls_log("KLS-DEMO","New demo run.");
  }
  if (KOLISEO_DEBUG == 1) {
	  fclose(KOLISEO_DEBUG_FP);
  }
  if (KOLISEO_DEBUG == 1) {
	  KOLISEO_DEBUG_FP = fopen("./static/debug_log.txt","a");
	  if (KOLISEO_DEBUG_FP == NULL) {
		fprintf(stderr,"[KLS]    Failed to open debug logfile.\n");
		exit(EXIT_FAILURE);
	  }
  }

  printf("[Init Koliseo] [size: %i]\n",KLS_DEFAULT_SIZE);
  Koliseo* kls = kls_new(KLS_DEFAULT_SIZE);

  #ifndef MINGW32_BUILD
  printf("[Current position in Koliseo] [pos: %li]\n",kls_get_pos(kls));
  #else
  printf("[Current position in Koliseo] [pos: %lli]\n",kls_get_pos(kls));
  #endif

  print_dbg_kls(kls);

  #ifndef MINGW32_BUILD
  printf("[Show reversed Region list for Koliseo] [pos: %li]\n",kls_get_pos(kls));
  #else
  printf("[Show reversed Region list for Koliseo] [pos: %lli]\n",kls_get_pos(kls));
  #endif

  KLS_ECHOLIST(kls_reverse(kls->regs));

  Koliseo_Temp temp_kls = kls_temp_start(kls);

  #ifndef MINGW32_BUILD
  printf("[Started Koliseo_Temp] [pos: %li]\n",kls_get_pos(temp_kls.kls));
  #else
  printf("[Started Koliseo_Temp] [pos: %lli]\n",kls_get_pos(temp_kls.kls));
  #endif

  int minusone = -1;
  int* p = &minusone;
  int* p2 = &minusone;
  int* p3 = &minusone;
  printf("\n*p is [%i] before KLS_PUSH\n",*p);
  printf("\n*p2 is [%i] before KLS_PUSH_T\n",*p2);
  printf("\n*p3 is [%i] before KLS_PUSH_T\n",*p3);

  #ifndef MINGW32_BUILD
  printf("[KLS_PUSH for a int to Koliseo] [size: %li]\n",sizeof(int));
  #else
  printf("[KLS_PUSH for a int to Koliseo] [size: %lli]\n",sizeof(int));
  #endif

  printf("[This handles the Koliseo directly while we have an open Koliseo_Temp.]\n");
  p = (int*) KLS_PUSH(kls, int, 1);

  #ifndef MINGW32_BUILD
  printf("[KLS_PUSH_T_NAMED for a int to Koliseo_Temp] [size: %li]\n",sizeof(int));
  #else
  printf("[KLS_PUSH_T_NAMED for a int to Koliseo_Temp] [size: %lli]\n",sizeof(int));
  #endif

  p2 = (int*) KLS_PUSH_T_NAMED(temp_kls, int, 1,"int", "Another int");
  #ifndef MINGW32_BUILD
  printf("[KLS_PUSH_T for a int to Koliseo_Temp] [size: %li]\n",sizeof(int));
  #else
  printf("[KLS_PUSH_T for a int to Koliseo_Temp] [size: %lli]\n",sizeof(int));
  #endif
  p3 = (int*) KLS_PUSH_T(temp_kls, int, 1);

  #ifndef MINGW32_BUILD
  printf("[Current position in Koliseo] [pos: %li]\n",kls_get_pos(kls));
  #else
  printf("[Current position in Koliseo] [pos: %lli]\n",kls_get_pos(kls));
  #endif

  #ifndef MINGW32_BUILD
  printf("[Current position in Koliseo_Temp] [pos: %li]\n",temp_kls.offset);
  #else
  printf("[Current position in Koliseo_Temp] [pos: %lli]\n",temp_kls.offset);
  #endif

  print_dbg_kls(kls);

  *p = 1;
  printf("\n*p is [%i] after KLS_PUSH\n",*p);
  *p2 = 2;
  printf("\n*p2 is [%i] after KLS_PUSH\n",*p2);
  *p2 = 3;
  printf("\n*p3 is [%i] after KLS_PUSH\n",*p3);

  #ifndef MINGW32_BUILD
  printf("[Show reversed Region list for Koliseo] [pos: %li]\n",kls_get_pos(kls));
  #else
  printf("[Show reversed Region list for Koliseo] [pos: %lli]\n",kls_get_pos(kls));
  #endif

  KLS_ECHOLIST(kls_reverse(kls->regs));
  printf("[%i] List size\n",kls_length(kls->regs));

  printf("[Usage report for Koliseo]\n");
  kls_usageReport(kls);

  WINDOW* win = NULL;
  /* Initialize curses */
  setlocale(LC_ALL, "");
  initscr();
  clear();
  refresh();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  win = newwin(20, 60, 1, 2);
  keypad(win, TRUE);
  wclear(win);
  wrefresh(win);
  kls_show_toWin(kls,win);
  refresh();
  kls_showList_toWin(kls,win);
  delwin(win);
  endwin();

  int* z = &minusone;
  printf("\n*z is [%i] before KLS_POP\n",*z);

  #ifndef MINGW32_BUILD
  printf("[KLS_POP a int from Koliseo] [size: %li]\n",sizeof(int));
  #else
  printf("[KLS_POP a int from Koliseo] [size: %lli]\n",sizeof(int));
  #endif

  z = KLS_POP(kls, int, 1);


  printf("\n*z is [%i] after KLS_POP\n",*z);

  #ifndef MINGW32_BUILD
  printf("[Current position in Koliseo] [pos: %li]\n",kls_get_pos(kls));
  #else
  printf("[Current position in Koliseo] [pos: %lli]\n",kls_get_pos(kls));
  #endif

  print_dbg_kls(kls);

  kls_temp_end(temp_kls);
  printf("[Ended Koliseo_Temp]\n");

  print_dbg_kls(kls);

  printf("[Clear Koliseo]\n");
  kls_clear(kls);
  print_dbg_kls(kls);

  printf("[Free Koliseo]\n");
  kls_free(kls);

  printf("[End of demo]\n");
  printf("[End of demo for Koliseo, API lvl [%i] v%s ]\n", int_koliseo_version(), string_koliseo_version());
  if (KOLISEO_DEBUG == 1) {
	  fclose(KOLISEO_DEBUG_FP);
  }

  return 0;
}
