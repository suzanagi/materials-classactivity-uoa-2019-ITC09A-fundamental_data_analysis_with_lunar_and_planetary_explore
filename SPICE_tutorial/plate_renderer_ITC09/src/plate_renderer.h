//Maximum size of the auxiliary data
#define  MAX_PLATES 3200000
//hay_a_amica_5_itokawashape_v1_0_512q.bds: 3145728 plates
//hay_a_amica_5_itokawashape_v1_0_64q.bds: 49152 plates
//#define  MAX_PLATES 50000

//Itokawa-Hayabusa
#ifdef HYB1
  #define  TARGETBODY "2025143"
  #define  BODYFIXEDFRAME "ITOKAWA_FIXED"
  #define  SCFRAME "HAYABUSA_SC_BUS_PRIME"
  #define  ASTEROIDPOINTINGFRAME "ITOKAWA_-Z_SUN_+X"
  #define  SCID -130
  #define  SCNAME "-130"
  #define  INSTID -130102 //Use AMICA
  //Default distance to the S/C in the free LOS mode
  #define  DISTANCE 7 //Itokawa 7 [km], 1999JU3: 20 [km]
#endif

//Ryugu-Hayabusa2
#ifdef HYB2
  #define  TARGETBODY "2162173"
  #define  BODYFIXEDFRAME "RYUGU_FIXED"
  #define  SCFRAME "HAYABUSA2_SC_BUS_PRIME"
  #define  ASTEROIDPOINTINGFRAME "RYUGU_-Z_SUN_+X"
  #define  SCID -37
  #define  SCNAME "-37"
  #define  INSTID -37100 //ONC-T
  #define  DISTANCE 20 //Itokawa 7 [km], 1999JU3: 20 [km]
#endif

//Default distance to the Sun in the free LOS mode
#define  SOLAR_DISTANCE 100

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


double *range_img, *incidence_img, *emission_img, *phase_img;
double *lat_img, *lon_img, *aux_img, *hapke_img, *strike_img;
double *plid_img;

void draw_grid(long int *, long int *, int);

int ONC_getlos_c ( SpiceInt        instid,
                   SpiceDouble     pixel[2],
                   SpiceDouble     los_vector [3] );
double hapke(double i, double e, double g);
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
        );

int get_cam_info (
        SpiceInt        instid,
        SpiceDouble     center[2],
        SpiceDouble     pixels[2],
        SpiceDouble     *pixel_size,
        SpiceDouble     *focal_length );
