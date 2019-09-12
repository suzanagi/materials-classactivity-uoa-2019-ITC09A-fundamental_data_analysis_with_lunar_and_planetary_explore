/******************************************************************************/
/*  ONC_getlos_c                                                              */
/*  Compute a line of sight vector of a given pixel of an imager              */
/*                                                                            */
/*  v0.1 -- June 24, 2005 -- Naru Hirata Naru Hirata, Kobe Univ.              */
/*  based on LISM_getlos.c                                                    */
/*                                                                            */
/*  Return value                                                              */
/*  1               Normal.                                                   */
/*  0               Some variables cannot be found in the kernel pool.        */
/*                                                                            */
/*  VARIABLE        I/O  DESCRIPTION                                          */
/*  --------------- ---  ---------------------------------------------------- */
/*   instid          I   NAIF ID of an instrument.                            */
/*   pixel[2]        I   Pixel coordinate (0: horizontal, 1: vertical).       */
/*   los_vector [3]  O   LOS vector.                                          */
/*                                                                            */
/******************************************************************************/

#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "SpiceUsr.h"
#define  STRLEN 100
#define  ROOM    10

int ONC_getlos_c ( SpiceInt        instid,
                    SpiceDouble     pixel[2],
                    SpiceDouble     los_vector [3] )
{
	
	SpiceDouble      focal_length;
	SpiceDouble      center[2];
#ifdef HYB1
	SpiceDouble      pixel_size;
#endif
#ifdef HYB2
	SpiceDouble      pixel_size[2];
#endif
	
	SpiceDouble      d_h; //horizontal distance from the boresight direction
	SpiceDouble      d_v; //vertical distance from the boresight direction
	
	//Name of the variable whose value is to be returned from the kernel pool.
	SpiceChar        var_name[STRLEN]; 
	SpiceInt         n_return;
	SpiceBoolean     found_fl,found_center,found_pixel_size;
	
	sprintf (var_name,"INS%d_FOCAL_LENGTH",instid);
	gdpool_c (var_name,0,ROOM,&n_return,&focal_length,&found_fl);
	
#ifdef HYB1
	sprintf (var_name,"INS%d_CENTER",instid);
	gdpool_c (var_name,0,ROOM,&n_return,center,&found_center);
#endif
#ifdef HYB2
	sprintf (var_name,"INS%d_CCD_CENTER",instid);
	gdpool_c (var_name,0,ROOM,&n_return,center,&found_center);
#endif
	
	sprintf (var_name,"INS%d_PIXEL_SIZE",instid);
#ifdef HYB1
	gdpool_c (var_name,0,ROOM,&n_return,&pixel_size,&found_pixel_size);
#endif
#ifdef HYB2
	gdpool_c (var_name,0,ROOM,&n_return,pixel_size,&found_pixel_size);
#endif
	
	//If required items are not found in the kernel pool, exit the program
	if (!found_fl||!found_center||!found_pixel_size) return(0);
	
	
#ifdef DEBUG
	printf ("focal length: %f\n", focal_length);
	printf ("center: %f %f\n", center[0],center[1]);
	printf ("pixel size: %f\n", pixel_size);
#endif /*DEBUG*/

	
  //Compute the pixel offset from the FOV center
#ifdef HYB1
	d_h = (pixel[0] - center[0]) * pixel_size;
	d_v = (pixel[1] - center[1]) * pixel_size;
#endif
#ifdef HYB2
	d_h = (pixel[0] - center[0]) * pixel_size[0];
	d_v = (pixel[1] - center[1]) * pixel_size[1];
#endif
	
	// Compute a line of sight vector with r = f x tan(theta) function
	// in which function, a LOS should be (d_h, d_v, focal_length)
	los_vector[0] = d_h;
	los_vector[1] = d_v;
	los_vector[2] = focal_length;
	
#ifdef DEBUG
	printf ("los_vector: %f %f %f\n", los_vector[0], los_vector[1], los_vector[2]);
#endif /*DEBUG*/
	return(1);
}