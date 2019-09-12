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
  SpiceChar          utc[80];
  SpiceDouble        et;
  SpiceDouble        ti;
  
  
  //Read kernels
  furnsh_c ( SCLKKER );
  furnsh_c ( LSK );
  
  if (argc != 2 ){
    fprintf(stderr,"Usage:%s UTC\n",argv[0]);
    exit (-1);
  }
  
  //Copy UTC time string from commandline argument
  strcpy(utc,argv[1]);
  
  //Convert UTC time string to ET
  utc2et_c (utc, &et );
  
  //Convert ET to TI (1/8192 sec format)
  sce2t_c (SCID, et, &ti  );
  
  //Output with conversion from 1/8192 sec format to 1/32 sec format
  printf("%.0f\n",ti / 256);
  
  return 0;
  
}
