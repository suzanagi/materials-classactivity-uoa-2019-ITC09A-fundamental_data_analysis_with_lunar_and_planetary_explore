#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SpiceUsr.h"

#define  STRLEN 100

int main(int argc, char *argv[]) {

  SpiceDouble             et;
  SpiceDouble             start_et;
  SpiceChar               utc[STRLEN];
  
  SpiceDouble hayabusa_pos_J2k[3];
  SpiceDouble itokawa_pos_J2k[3];
  SpiceDouble earth_pos_J2k[3];
  SpiceDouble lt_hayabusa, lt_itokawa, lt_earth;
  
  //Define start time
  SpiceChar   time[STRLEN]="2003-05-09T00:00:00";
  
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
  
  //Convert UTC time string to et
  str2et_c ( time, &start_et );
  
  //Output index text
  printf ("#hayabusa_pos_X\tY\tZ\titokawa_pos_X\tY\tZ\tearth_pos_X\tY\tZ\thayabusa_distance\n");

  //Compute from the start time for 5 days with 1 hour interval
  for(et=start_et;et< start_et+24*3600*857-3600;et=et+3600){
    
    //Convert et to UTC time string
    et2utc_c ( et, "ISOC", 0, STRLEN, utc );
    printf("%s\t",utc);

    
    spkpos_c ( "MUSES-C", et, "SUN_EARTH_FIXED", "NONE", "SUN", hayabusa_pos_J2k, &lt_hayabusa);
    spkpos_c ( "ITOKAWA", et, "SUN_EARTH_FIXED", "NONE", "SUN", itokawa_pos_J2k, &lt_itokawa);
    spkpos_c ( "EARTH", et, "SUN_EARTH_FIXED", "NONE", "SUN", earth_pos_J2k, &lt_earth);

    printf ( "%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\n",
	     hayabusa_pos_J2k[0], hayabusa_pos_J2k[1], hayabusa_pos_J2k[2],
	     itokawa_pos_J2k[0], itokawa_pos_J2k[1], itokawa_pos_J2k[2],
	     earth_pos_J2k[0], earth_pos_J2k[1], earth_pos_J2k[2],
	     lt_hayabusa);
  }
  return 0;
}
