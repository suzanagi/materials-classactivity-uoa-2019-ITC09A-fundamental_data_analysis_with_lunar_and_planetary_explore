
           #include <stdio.h>
 
           /*
           Standard CSPICE User Include File
           */
           #include "SpiceUsr.h"
 
           /*
           Local Parameters
           */
 
           #define METAKR "./convtm.tm"
           #define SCLKID -82
           #define STRLEN 50
 
           int main (void)
           {
 
              /*
              Local Variables
              */
              SpiceChar               calet  [STRLEN];
              SpiceChar               sclkst [STRLEN];
              SpiceChar               utctim [STRLEN];
 
              SpiceDouble             et;
 
              /*
              Load the kernels this program requires.
              Both the spacecraft clock kernel and a
              leapseconds kernel should be listed in
              the meta-kernel.
              */
              furnsh_c ( METAKR );
 
              /*
              Prompt the user for the input time string.
              */
              prompt_c ( "Input UTC Time: ", STRLEN, utctim );
 
              printf ( "Converting UTC Time: %s\n", utctim );
 
              /*
              Convert utctim to ET.
              */
              str2et_c ( utctim, &et );
 
              printf ( "   ET Seconds Past 2000: %16.3f\n", et );
 
              /*
              Now convert ET to a calendar time
              string.  This can be accomplished in two
              ways.
              */
              etcal_c ( et, STRLEN, calet );
 
              printf ( "   Calendar ET (etcal_c): %s\n", calet );
 
              /*
              Or use timout_c for finer control over the
              output format.  The picture below was built
              by examining the header of timout_c.
              */
              //timout_c ( et,     "YYYY-MON-DDTHR:MN:SC ::UTC+9",
              timout_c ( et,     "YYYY-MON-DDTHR:MN:SC ::TDB",
                         STRLEN, calet                         );
 
              printf ( "   Calendar ET (timout_c): %s\n", calet );
 
              /*
              Convert ET to spacecraft clock time.
              */
              sce2s_c ( SCLKID, et, STRLEN, sclkst );
 
              printf ( "   Spacecraft Clock Time: %s\n", sclkst );

              return(0);
           }
 

