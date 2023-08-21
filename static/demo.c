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

  int minusone = -1;
  int* p = &minusone;
  printf("\n*p is [%i] before KLS_PUSH\n",*p);
  printf("[KLS_PUSH for a int to Koliseo] [size: %li]\n",sizeof(int));
  p = (int*) KLS_PUSH(kls, int, 1);
  printf("[Current position in Koliseo] [pos: %li]\n",kls_get_pos(kls));
  print_dbg_kls(kls);


  *p = 1;
  printf("\n*p is [%i] after KLS_PUSH\n",*p);

  int* z = &minusone;
  printf("\n*z is [%i] before KLS_POP\n",*z);

  printf("[KLS_POP a int from Koliseo] [size: %li]\n",sizeof(int));
  z = KLS_POP(kls, int, 1);


  printf("\n*z is [%i] after KLS_POP\n",*z);
  printf("[Current position in Koliseo] [pos: %li]\n",kls_get_pos(kls));

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
