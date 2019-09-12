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

void get_plbore (
        SpiceDouble SC_pos_bd[],      //input arguments
        SpiceDouble solar_pos_bd[],
        SpiceDouble bsight_bd[],
        SpiceInt *PLID_ptr,           //output arguments
        SpiceDouble surface_point[],
        SpiceDouble plnorm[],
        SpiceDouble *incidence,
        SpiceDouble *emission,
        SpiceDouble *phase,
        SpiceBoolean *illuminated, //True if illuminated
        SpiceBoolean *found,       //True if an intersection is found
      	SpiceDLADescr dladsc,
      	SpiceInt    handle
        )
{
  SpiceBoolean found1;
  SpiceDouble surface_point_tmp[3];
  SpiceDouble SC_pos_bd_d[3], solar_pos_bd_d[3];
  SpiceInt    PLID;

  dskx02_c (handle,
            &dladsc,
            SC_pos_bd,
            bsight_bd,
            PLID_ptr,
            surface_point,
            &found1);

  
  if (failed_c() || !found1) {
    reset_c();
    *found = SPICEFALSE;
    *illuminated = SPICEFALSE;
  } else {
    dskn02_c ( handle, &dladsc, *PLID_ptr, plnorm );

    vsub_c (solar_pos_bd,surface_point,solar_pos_bd_d);
    vsub_c (SC_pos_bd,surface_point,SC_pos_bd_d);
    *incidence = vsep_c(solar_pos_bd_d,plnorm);
    *emission = vsep_c(SC_pos_bd_d,plnorm);
    *phase = vsep_c(SC_pos_bd_d,solar_pos_bd_d);
    dskx02_c (handle,
            &dladsc,
            surface_point,
            solar_pos_bd_d,
            &PLID,
            surface_point_tmp,
            &found1);
    
    if (!found1 && (*incidence > 0.0) && (*incidence*dpr_c() < 90.0) ) {
      *found = SPICETRUE;
      *illuminated = SPICETRUE;
    } else {
      *found = SPICETRUE;
      *illuminated = SPICEFALSE;
    }
  }
}
