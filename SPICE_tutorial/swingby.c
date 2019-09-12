#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SpiceUsr.h"

#define  STRLEN 100


int main(int argc, char *argv[]) {


  SpiceDouble             et;
  SpiceDouble             start_et;
  SpiceChar               utc[STRLEN];
  
  SpiceDouble             SC_pos_bd[3];
  SpiceDouble             lunar_pos_bd[3];
  SpiceDouble             SC_pos_J2k[3];
  SpiceDouble             lunar_pos_J2k[3];
  SpiceDouble             lt_SC, lt_lunar;
  
  
  //Define start time
  SpiceChar   time[STRLEN]="2004-05-17T00:00:00";
  
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
  printf ( "#SC_pos_bd_X\tY\tZ\tlunar_pos_bd_X\tY\tZ\tSC_pos_J2k_X\tY\tZ\tlunar_pos_J2k_X\tY\tZ\tSC_distance\n");

  //Compute from the start time for 5 days with 1 hour interval
  for(et=start_et;et< start_et+24*3600*5;et=et+3600){
    
    //Convert et to UTC time string
    et2utc_c ( et, "ISOC", 0, STRLEN, utc );
    printf("%s\t",utc);
    
    spkpos_c ( "MUSES-C", et,   "J2000",  "NONE", "EARTH", SC_pos_J2k,  &lt_SC);
    spkpos_c ( "MOON", et,   "J2000",  "NONE", "EARTH", lunar_pos_J2k,  &lt_lunar);
    spkpos_c ( "MUSES-C", et,   "IAU_EARTH",  "LT+S", "EARTH", SC_pos_bd,  &lt_SC);
    spkpos_c ( "MOON", et,   "IAU_EARTH",  "LT+S", "EARTH", lunar_pos_bd,  &lt_lunar);
    
    printf ( "%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\t%9.4f\n",
            SC_pos_bd[0], SC_pos_bd[1], SC_pos_bd[2], lunar_pos_bd[0], lunar_pos_bd[1], lunar_pos_bd[2],
            SC_pos_J2k[0], SC_pos_J2k[1], SC_pos_J2k[2], lunar_pos_J2k[0], lunar_pos_J2k[1], lunar_pos_J2k[2],
            lt_SC*clight_c ());
  }
  return 0;
}
