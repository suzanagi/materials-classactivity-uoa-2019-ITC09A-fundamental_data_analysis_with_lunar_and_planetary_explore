#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SpiceUsr.h"

#define  STRLEN 100

//Macro definitions for getfov_c
#define  BCVLEN 5       //max number of FOV bound vectors
#define  ROOM    10


int main(int argc, char *argv[]) {


  SpiceDouble             lat;
  SpiceDouble             lon;
  SpiceDouble             radius;
  
  SpiceDouble             phase, incidence, emission;
  
  SpiceDouble             xpoint[3];
  SpiceDouble             xsrfvec[3];
  SpiceDouble             subpoint[3];
  SpiceDouble             subsrfvec[3];

  SpiceDouble             et;
  SpiceDouble             start_et;
  SpiceDouble             trgepc;
  SpiceChar               utc[STRLEN];
  SpiceChar               trgutc[STRLEN];
  
  SpiceDouble             SC_pos_bd[3];
  SpiceDouble             lt_SC, lt_lunar;

  
  //getfov_c releted variables
  SpiceChar        shape  [STRLEN];
  SpiceChar        frame  [STRLEN]; //name of frame that FOV/boresight are defined.
  SpiceDouble      bsight [3]; //boresight vector
  SpiceInt         n_bcv; //number of FOV bound vectors
  SpiceDouble      bounds [BCVLEN][3];
  SpiceBoolean     found;
  
  //Define start time
  SpiceChar   time[STRLEN]="2004-05-17T09:30:00";
  
  //instrument NAIF ID
  //AMICA (camera): -130102
  SpiceInt         instid = -130102;
  
  if (argc < 2) {
    fprintf(stderr, "Usage: %s kernel ...\n",argv[0]);
    exit(-1);
  }
  
  
  //Read kernels
  while (argc > 1) {
    furnsh_c (argv[1]);
    fprintf(stderr,"%s is loaded.\n",argv[1]);
    --argc;
    ++argv;
  }
  
  //obtaining instrument info
  getfov_c ( instid, BCVLEN, STRLEN, STRLEN,
          shape, frame,  bsight, &n_bcv, bounds );
  
  //convert start time in UTC to et
  str2et_c ( time, &start_et );
  
  //Set error actions (see exercise 3)
  erract_c("SET", STRLEN, "RETURN");
  errdev_c("SET", STRLEN, "NULL");
  
  //Compute from the start time for 2 hours with 10 sec interval
  for(et=start_et;et< start_et+3600*2;et=et+10){
    
    //Output time in UTC
    et2utc_c(et, "ISOC", 0, STRLEN, utc);
    printf("%s\t",utc);
    
    //compute sub-S/C point in XYZ
    /* fill here below */
    subpnt_c("Intercept: ellipsoid", "MOON", et, "IAU_MOON", "LT+S", "HAYABUSA", subpoint, &trgepc, subsrfvec); 

    //Error handling for subpnt_c() (see exercise 3)
    if(failed_c()){
      found = SPICEFALSE;
      reset_c();
    }
    
    //convert sub-S/C point to lat/lon and print
    reclat_c(subpoint, &radius, &lon, &lat);
    lon *= dpr_c();
    if (lon < 0) {
      lon = 180 - lon;
    }
    lat *= dpr_c();
    printf ("%4.2f\t%4.2f\t", lat, lon);
    
    //compute a footprint of the instrument on the lunar surface
    sincpt_c("Ellipsoid", "MOON", et, "IAU_MOON", "LT+S", "HAYABUSA", frame, bsight, xpoint, &trgepc, xsrfvec, &found);
    
    //Error handling for sincpt_c() (see exercise 3)
    if(failed_c()){
      found = SPICEFALSE;
      reset_c();
    }

    //if found a footprint
    if(found) {
      //print observation time in UTC
      et2utc_c ( trgepc, "ISOC", 0, STRLEN, trgutc );
      printf("%s\t",trgutc);
      
      //convert footprint location to lat/lon and print
      reclat_c(xpoint, &radius, &lon, &lat);
      lon *= dpr_c();
      if (lon < 0) {
        lon = 180 - lon;
      }
      lat *= dpr_c();
      printf ("%4.2f\t%4.2f\t", lat, lon);
      
      //compute illumination conditions and print
      ilumin_c("Ellipsoid", "MOON", et, "IAU_MOON", "LT+S", "HAYABUSA", xpoint, &trgepc, xsrfvec, &phase, &incidence, &emission);
      phase *= dpr_c();
      incidence *= dpr_c();
      emission *= dpr_c();
      printf ("%4.2f\t%4.2f\t%4.2f", phase, incidence, emission);
      
    }
  printf ("\n");
  }
  return 0;
}
