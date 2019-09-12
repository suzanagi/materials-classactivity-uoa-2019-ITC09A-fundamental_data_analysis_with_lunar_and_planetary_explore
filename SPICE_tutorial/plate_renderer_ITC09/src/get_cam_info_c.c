#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fitsio.h>
#include <unistd.h>
#include <getopt.h>
#include <float.h>
#include "SpiceUsr.h"
#include "SpiceDLA.h"
#include "SpiceDSK.h"
#define  STRLEN 100
#define  ROOM    10

int get_cam_info (  SpiceInt        instid,
                    SpiceDouble     center[2],
                    SpiceDouble     pixels[2],
                    SpiceDouble     *pixel_size,
                    SpiceDouble     *focal_length )
{
    
  //Name of the variable whose value is to be returned from the kernel pool.
  SpiceChar        var_name[STRLEN]; 
  SpiceInt         n_return;
  SpiceBoolean     found_center,found_pixel_samples,found_pixel_lines,found_pixel_size,found_focal_length;
  
#ifdef HYB1
  sprintf (var_name,"INS%d_CENTER",instid);
  gdpool_c (var_name,0,ROOM,&n_return,center,&found_center);
#endif
#ifdef HYB2
  sprintf (var_name,"INS%d_CCD_CENTER",instid);
  gdpool_c (var_name,0,ROOM,&n_return,center,&found_center);
#endif
  
  sprintf (var_name,"INS%d_PIXEL_SAMPLES",instid);
  gdpool_c (var_name,0,ROOM,&n_return,&pixels[0],&found_pixel_samples);
  
  sprintf (var_name,"INS%d_PIXEL_LINES",instid);
  gdpool_c (var_name,0,ROOM,&n_return,&pixels[1],&found_pixel_lines);
  
  sprintf (var_name,"INS%d_PIXEL_SIZE",instid);
  gdpool_c (var_name,0,ROOM,&n_return,pixel_size,&found_pixel_size);

  sprintf (var_name,"INS%d_FOCAL_LENGTH",instid);
  gdpool_c (var_name,0,ROOM,&n_return,focal_length,&found_focal_length);
  
	//If required items are not found in the kernel pool, exit the program
  if (!found_center||!found_pixel_samples||!found_pixel_lines||!found_focal_length) return(0);
  
  return(1);
}