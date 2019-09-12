#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Header file for SPICE toolkit
#include "SpiceUsr.h"

//sclk for Hayabusa
#define      SCLKKER   "./kernels/HAYABUSA/sclk/hayabusa.tsc"
  
//leap second kernel
#define      LSK       "./kernels/generic_kernels/lsk/naif0012.tls"

//Define spacecraft ID of Hayabusa (-130)
#define  SCID -130


int main(int argc, char *argv[]) {
  
  
  //Define variables with SPICE original types
  static SpiceChar   ti[80];
  SpiceDouble        et;
  SpiceChar          utc[80];

  //Read kernels
  furnsh_c ( SCLKKER );
  furnsh_c ( LSK );
  
  if (argc != 2 ){
    fprintf(stderr,"Usage:%s TI(in decimal)\n",argv[0]);
    exit (-1);
  }
  
  //Copy TI time string from commandline argument
  strcpy(ti,argv[1]);
  
  //Convert TI time string to ET
  scs2e_c (SCID, ti, &et  );
  
  //Convert ET to UTC time string with the ISOC calendar format
  et2utc_c ( et, "ISOC", 0, 30, utc);
  printf("%s\n",utc);
  
  return 0;
  
}
