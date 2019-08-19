
           #include <stdio.h>
           #include <stdlib.h>
           #include <string.h>

           /*
           Standard CSPICE User Include File
           */
           #include   "SpiceUsr.h"

           /*
           Local Parameters 
           */

           #define    METAKR       "./mars.tm"
           #define    STRLEN       50

           #define    ABCORR       "LT+S"
           #define    FRAME        "J2000"
           #define    OBSERVER     "EARTH"
           #define    TARGET       "MARS BARYCENTER"

           #define    AU            1.495978707E8  // Astronomical Unit in km


           int  main (void)
           {

              /*
              Local variables
              */
              SpiceChar           utctim[STRLEN];

              SpiceDouble         et;
              SpiceDouble         state[6];//state vector
              SpiceDouble         lt;//light time


              //     Load the kernels: LSK, SPK
              furnsh_c ( METAKR );

              //     Copy time string
              strcpy(utctim,"JAN 1 2015 12:00:00");

              //     Convert the epoch to ephemeris time.
              str2et_c (utctim, &et);

              //     Look-up the state for the defined parameters.
              spkezr_c ( TARGET,   et,   FRAME,   ABCORR,  OBSERVER , state, &lt);


              //      Output...
              //      The first three entries of state contain the
              //      X, Y, Z position components. The final three contain
              //      the Vx, Vy, Vz velocity components.
             
 
              printf( "\net = %20.10f\n\n",                 et       );
              printf( "J2000 x-position (km):   %20.10f\n", state[0] );
              printf( "J2000 y-position (km):   %20.10f\n", state[1] );
              printf( "J2000 z-position (km):   %20.10f\n", state[2] );
              printf( "J2000 x-velocity (km/s): %20.10f\n", state[3] );
              printf( "J2000 y-velocity (km/s): %20.10f\n", state[4] );
              printf( "J2000 z-velocity (km/s): %20.10f\n", state[5] );

              return(0); 
           }
