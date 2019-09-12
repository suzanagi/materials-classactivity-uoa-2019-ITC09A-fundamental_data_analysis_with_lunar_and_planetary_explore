#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SpiceUsr.h"

#define  STRLEN 100
#define NONE 100

int main(int argc, char *argv[]) {


  SpiceDouble             et;
  SpiceDouble             start_et;
  SpiceChar               utc[STRLEN];

  //  SpiceDouble itokawa_pos_hayabusa_hp[3];
  SpiceDouble hayabusa_pos_hayabusa_hp[3];
  SpiceDouble itokawa_lt, hayabusa_lt;

  //Define start time
  SpiceChar   time[STRLEN]="2005-09-10T03:00:00";
  
  int i = 0;

  if (argc < 2) {
    printf("Usage: %s kernel ...\n",argv[0]);
    exit(-1);
  }

  //Read kernels
  while (argc > 1) {
    furnsh_c (argv[1]);
    fprintf(stderr,"%s is loaded.\n",argv[1]);
    --argc;
    ++argv;
  }
  
  printf("kernels loaded successfully\n");

  //Convert UTC time string to et
  str2et_c ( time, &start_et );
  
  //Output index text
  printf ("#hayabusa_pos_X\tY\tZ\n");


  erract_c("SET", STRLEN, "RETURN");
  errdev_c("SET", STRLEN, "NULL");

  printf("error handling process operated\n");

  //Compute from the start time for 5 days with 1 hour interval
  for(et=start_et;et< start_et+24*3600*73+3600*2;et=et+3600){
    
    //Convert et to UTC time string
    et2utc_c ( et, "ISOC", 0, STRLEN, utc );
    printf("%s\t",utc);

    spkpos_c ( "HAYABUSA", et, "HAYABUSA_HP", "NONE", "ITOKAWA", hayabusa_pos_hayabusa_hp, &hayabusa_lt);
    if(failed_c()) {

      hayabusa_pos_hayabusa_hp[0] = NONE;
      hayabusa_pos_hayabusa_hp[1] = NONE;
      hayabusa_pos_hayabusa_hp[2] = NONE;

      reset_c();
    }

    for(i = 0; i < 3; i++)
      if(hayabusa_pos_hayabusa_hp[i] != NONE) printf("%9.4f\t", hayabusa_pos_hayabusa_hp[i]);
    printf("\n");
    
  }
  return 0;
}
