#include <stdio.h>
#include "../src/koliseo.h"
#include "amboso.h"

int main(void) {
  KOLISEO_DEBUG = 1;
  printf("Demo for Koliseo, using API version %s\n", string_koliseo_version());
  printf("Supporting Amboso API version %s\n\n", getAmbosoVersion());
  printf("KOLISEO_DEBUG is [%i]\n\n", KOLISEO_DEBUG);
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

  printf("[Current position in Koliseo] [pos: %li]\n",kls_get_pos(kls));

  print_dbg_kls(kls);

  Koliseo_Temp temp_kls = kls_temp_start(kls);
  printf("[Started Koliseo_Temp] [pos: %li]\n",kls_get_pos(temp_kls.kls));

  int minusone = -1;
  int* p = &minusone;
  int* p2 = &minusone;
  printf("\n*p is [%i] before KLS_PUSH\n",*p);
  printf("\n*p2 is [%i] before KLS_PUSH_T\n",*p2);
  printf("[KLS_PUSH for a int to Koliseo] [size: %li]\n",sizeof(int));
  printf("[This handles the Koliseo directly while we have an open Koliseo_Temp.]\n");
  p = (int*) KLS_PUSH(kls, int, 1);
  printf("[KLS_PUSH_T for a int to Koliseo_Temp] [size: %li]\n",sizeof(int));
  p2 = (int*) KLS_PUSH_T(temp_kls, int, 1);
  printf("[Current position in Koliseo] [pos: %li]\n",kls_get_pos(kls));
  printf("[Current position in Koliseo_Temp] [pos: %li]\n",temp_kls.offset);
  print_dbg_kls(kls);

  *p = 1;
  printf("\n*p is [%i] after KLS_PUSH\n",*p);
  *p2 = 3;
  printf("\n*p2 is [%i] after KLS_PUSH\n",*p2);

  int* z = &minusone;
  printf("\n*z is [%i] before KLS_POP\n",*z);

  printf("[KLS_POP a int from Koliseo] [size: %li]\n",sizeof(int));
  z = KLS_POP(kls, int, 1);


  printf("\n*z is [%i] after KLS_POP\n",*z);
  printf("[Current position in Koliseo] [pos: %li]\n",kls_get_pos(kls));

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
  if (KOLISEO_DEBUG == 1) {
	  fclose(KOLISEO_DEBUG_FP);
  }

  return 0;
}
