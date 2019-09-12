#include "plate_renderer.h"

#define  STRLEN 500
#define  BCVLEN 5       //max number of FOV bound vectors
#define  ROOM 10

int main(int argc, char *argv[]) {
  
  //Option related variables
  int opt;
  extern char *optarg;
  extern int  optind, opterr;
  SpiceBoolean GRID = SPICEFALSE;
  int GRID_SPACING = 10;
  SpiceBoolean AUX  = SPICEFALSE;
  SpiceBoolean HAPKE = SPICETRUE;
  SpiceBoolean GEOMETRY = SPICEFALSE;
  SpiceBoolean POLYGONID = SPICEFALSE;
  SpiceBoolean LATLON = SPICEFALSE;
  SpiceBoolean RANGE = SPICEFALSE; //Latitude longitude map
  SpiceBoolean ILLIMINATION = SPICEFALSE; //Illumination condition map
  SpiceBoolean LIGHTCURVE = SPICEFALSE;
  SpiceBoolean FREEVIEW = SPICEFALSE;
  SpiceBoolean FREEVIEWPRM = SPICEFALSE;
  SpiceBoolean EXTERNALPRM = SPICEFALSE;
  SpiceBoolean TI = SPICEFALSE;
  SpiceBoolean DIRECTION = SPICEFALSE;
  struct option longopts[] = {
    { "aux",           required_argument, NULL, 'a'},
    { "basefilename",  required_argument, NULL, 'b'},
    { "grid",          required_argument, NULL, 'g'},
    { "geo",           no_argument,       NULL, 'e'},
    { "range",         no_argument,       NULL, 'r'},
    { "illumination",  no_argument,       NULL, 'l'},
    { "latlon",        no_argument,       NULL, 'n'},
    { "polygonid",     no_argument,       NULL, 'p'},
    { "lightcurve",    no_argument,       NULL, 'c'},
    { "nohapke",       no_argument,       NULL, 'h'},
    { "utc",           required_argument, NULL, 'u'},
    { "ti",            required_argument, NULL, 't'},
    { "direction",     no_argument,       NULL, 'd'},
    { "view",          required_argument, NULL, 'v'},
    { "VIEW",          required_argument, NULL, 'V'},
    { "external",      no_argument,       NULL, 'x'},
    { "All",           no_argument,       NULL, 'z'},
    { 0, 0, 0, 0},
  };
  int longindex;
  
  // general purpose variables
  SpiceChar     instr[STRLEN];
  SpiceChar     *strtoken;
  
  SpiceDouble   pixel_dbl[2];
  SpiceDouble   image_size[2];
  SpiceDouble   image_center[2];
  SpiceDouble   focal_length;
  SpiceDouble   pixel_size;
  SpiceInt      address;
  SpiceBoolean  found, illuminated;
  SpiceInt      n_return;
  
  // fits related variables
  fitsfile *range_fptr, *incidence_fptr, *emission_fptr, *phase_fptr;
  fitsfile *lat_fptr, *lon_fptr, *aux_fptr, *hapke_fptr, *strike_fptr;
  fitsfile *plid_fptr;
  int dim = 2;
  long int naxis[2];
  long int pixel[2];
  int status = 0;
  int i;
  
  // output files related variables
  SpiceChar   incidence_fname[STRLEN],range_fname[STRLEN],phase_fname[STRLEN];
  SpiceChar   emission_fname[STRLEN], lat_fname[STRLEN], lon_fname[STRLEN];
  SpiceChar   aux_fname[STRLEN], hapke_fname[STRLEN], strike_fname[STRLEN];
  SpiceChar   plid_fname[STRLEN];
  SpiceChar   base_fname_1[STRLEN] = "";
  SpiceChar   base_fname_2[STRLEN];
  
  SpiceDouble incidence,emission, phase;
  SpiceDouble range;
  SpiceDouble lat,lon;
  
  double hapke_sum = 0;
  
  SpiceDouble SC_pos_bd[3],solar_pos_bd[3],SC_pos_bdm[3];
  SpiceDouble rotate_x,rotate_y,rotate_z;
  SpiceDouble SC_lat = NAN;
  SpiceDouble SC_lon = NAN;
  SpiceDouble solar_lat = NAN;
  SpiceDouble solar_lon = NAN;
  
  SpiceChar   start_utc[STRLEN] = "";
  SpiceDouble et, start_et;
  SpiceInt    nloop = 1;
  SpiceDouble step_et = 0.0;
  SpiceDouble lt;
  SpiceChar   ti[STRLEN];
  
  SpiceInt         instid = INSTID; //instrument NAIF ID
  
  //getfov_c releted variables
  SpiceChar        shape  [STRLEN];
  SpiceChar        frame  [STRLEN]; //name of frame that FOV/boresight are defined.
  SpiceDouble      bsight [3]; //boresight vector
  SpiceInt         n_bcv; //number of FOV bound vectors
  SpiceDouble      bounds [BCVLEN][3];
  
  SpiceDouble      bsight_bd [3]; //boresight vector
  
  SpiceDouble rotate[3][3];
  SpiceDouble rotate_tmp[3][3];
  SpiceDouble rotate_inv[3][3];
  
  SpiceDouble plnorm[3];
  SpiceInt    PLID;
  SpiceDouble surface_point[3];
  SpiceDouble distan;
  
  FILE *aux_file;
  double *aux = NULL;
  long int plate_id;
  double aux_value;
  
  SpicePlane image_plane;
  SpiceDouble origin[3] = {0,0,0};
  SpiceInt                handle;
  SpiceDLADescr dladsc;
  
  if(argc==1) {
    fprintf(stderr,"Usage: %s [options] shape_kernel_to_load kernels_to_load\n",argv[0]);
    fprintf(stderr,"  NOTE: The DSK should be loaded independently and separately.\n");
    fprintf(stderr,"Options\n");
    fprintf(stderr,"  -a/-aux auxiliary_data_file\n");
    fprintf(stderr,"     Output a aux plane mapped with the auxiliary_data\n");
    fprintf(stderr,"  -b/-basefilename finename_base_string\n");
    fprintf(stderr,"     Define a base string for output files\n");
    fprintf(stderr,"  -g/-grid grid_spacing\n");
    fprintf(stderr,"     Draw lat-lon grid\n");
    fprintf(stderr,"  -p/-polygonid \n");
    fprintf(stderr,"     Output a polygon ID plane\n");
    fprintf(stderr,"  -e/-geo \n");
    fprintf(stderr,"     Output geometry information planes\n");
    fprintf(stderr,"  -r \n");
    fprintf(stderr,"     Output a S/C-target range plane\n");
    fprintf(stderr,"  -l \n");
    fprintf(stderr,"     Output illumination condition planes\n");
    fprintf(stderr,"  -n \n");
    fprintf(stderr,"     Output lat and lon plane\n");
    fprintf(stderr,"  -c/-lightcurve \n");
    fprintf(stderr,"     Output integrated brightness information for lightcurve analysis\n");
    fprintf(stderr,"  -h/-nohapke \n");
    fprintf(stderr,"     Suppress output of a hapke rendering plane\n");
    fprintf(stderr,"  -u/-utc [UTC_EPOCH[/TIME_STEP/LOOP_NUMBERR]]\n");
    fprintf(stderr,"     Define the epoch in UTC\n");
    fprintf(stderr,"     If arguments of the option are not provided, The tool ask UTC_EPOCH with a prompt\n");
    fprintf(stderr,"     Multiple image output is also possibele\n");
    fprintf(stderr,"       by defineing timestep and loop number.\n");
    fprintf(stderr,"     Example: -u 2005-10-01T00:00:00/3600/12\n");
    fprintf(stderr,"       will output 100 images with 1 hour interval\n");
    fprintf(stderr,"       from 2005-10-01T00:00:00 to 2005-10-01T11:00:00\n");
    fprintf(stderr,"  -t/-ti TI\n");
    fprintf(stderr,"     Define the epoch in TI\n");
    fprintf(stderr,"     TI should be supplied as an argument of this option\n");
    fprintf(stderr,"  -d/-direction \n");
    fprintf(stderr,"     S/C pointing is freezed to veiw the asteroid center\n");
    fprintf(stderr,"  -V/-VIEW \n");
    fprintf(stderr,"     Enable free viewing angle mode\n");
    fprintf(stderr,"  -v/-view [SC_lat/SC_lon/SUN_lat_SUN_lon]\n");
    fprintf(stderr,"     Enable free viewing angle mode with supplied parameters\n");
    fprintf(stderr,"     If arguments of the option are not provided, The tool ask them with a prompt\n");
    fprintf(stderr,"  -x/-external \n");
    fprintf(stderr,"     Get UTC, S/C Position and pointing in Body-fixed frame from stdin\n");
    fprintf(stderr,"  -z/-All \n");
    fprintf(stderr,"     Output all additinal information planes\n");
    exit(-1);
  }
  
  while ((opt = getopt_long_only(argc, argv, "a:b:g:zperlnchu:t:Vv:x", longopts, &longindex)) != -1) {
    switch (opt) {
      case 'a':
        AUX = SPICETRUE;
        if ((aux_file = fopen(optarg,"r")) == NULL) {
          fprintf(stderr,"auxiliary data file %s is not found\n",optarg);
          exit(-1);
        }
        break;
      case 'b':
        sprintf(base_fname_1, "%s_",optarg);
        break;
      case 'g':
        GRID = SPICETRUE;
        GRID_SPACING = atoi(optarg);
        break;
      case 'z':
        GEOMETRY = SPICETRUE;
        RANGE = SPICETRUE;
        ILLIMINATION = SPICETRUE;
        LATLON = SPICETRUE;
        POLYGONID = SPICETRUE;
        break;
      case 'p':
        POLYGONID = SPICETRUE;
        break;
      case 'e':
        GEOMETRY = SPICETRUE;
        break;
      case 'r':
        RANGE = SPICETRUE;
        break;
      case 'l':
        ILLIMINATION = SPICETRUE;
        break;
      case 'n':
        LATLON = SPICETRUE;
        break;
      case 'c':
        LIGHTCURVE = SPICETRUE;
        break;
      case 'h':
        HAPKE = SPICEFALSE;
        break;
      case 'u':
        if (strlen(optarg) != 0) {
          strtoken = strtok(optarg, "/");
          strcpy(start_utc,strtoken);
          if((strtoken = strtok( NULL, "/")) != NULL) step_et = atof(strtoken);
          if((strtoken = strtok( NULL, "/")) != NULL) nloop = atoi(strtoken);
        }
        break;
      case 't':
        TI = SPICETRUE;
        strcpy(ti,optarg);
        break;
      case 'd':
        DIRECTION = SPICETRUE;
        break;
      case 'V':
        FREEVIEW = SPICETRUE;
        break;
      case 'x':
        EXTERNALPRM = SPICETRUE;
        break;
      case 'v':
        FREEVIEW = SPICETRUE;
        if (strlen(optarg) != 0) {
          strtoken = strtok(optarg, "/");
          SC_lat = atof(strtoken) * rpd_c();
          if((strtoken = strtok( NULL, "/")) != NULL) SC_lon = atof(strtoken) * rpd_c();
          if((strtoken = strtok( NULL, "/")) != NULL) solar_lat = atof(strtoken) * rpd_c();
          if((strtoken = strtok( NULL, "/")) != NULL) solar_lon = atof(strtoken) * rpd_c();
          if (SC_lat != NAN || SC_lon != NAN || solar_lat != NAN || solar_lon != NAN) FREEVIEWPRM = SPICETRUE;
        }
        break;
      default:
        fprintf(stderr,"Usage: %s options shape_kernel_to_load kernels_to_load\n",argv[0]);
        return 1;
    }
  }
  argc -= optind;
  argv += optind;
  
  if (AUX) {
    aux = (double *)malloc(sizeof(double)*2*MAX_PLATES);
    for (i = 0; i <= -1; i++) {
      fgets(instr, STRLEN, aux_file);
    }
    fscanf(aux_file, "%ld", &plate_id);
    aux[0] = (SpiceDouble) plate_id;
    for (i = 1; i <= aux[0]; i++) {
      fscanf(aux_file, "%ld %lf", &plate_id, &aux_value);
      aux[i*2] = (SpiceDouble) plate_id;
      aux[i*2+1] = (SpiceDouble) aux_value;
    }
  }
  
  if (EXTERNALPRM) {
    fgets(instr, STRLEN, stdin);
    sscanf(instr, "%s%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", 
      start_utc, 
      &SC_pos_bd[0], &SC_pos_bd[1], &SC_pos_bd[2],
      &rotate[0][0], &rotate[0][1], &rotate[0][2],
      &rotate[1][0], &rotate[1][1], &rotate[1][2],
      &rotate[2][0], &rotate[2][1], &rotate[2][2]
      );
    xpose_c(rotate, rotate);
  }
  
  ///Open DSK file
  dasopr_c ( argv[0], &handle );
  //serch the kernel in the DSK file.
  //If not find DSK file, found is 0.
  dlabfs_c ( handle, &dladsc, &found );
  if ( !found  ) {
    /*
      We arrive here only if the kernel
      contains no segments.  This is
      unexpected, but we're prepared for it.
    */
      setmsg_c ( "No segments found in DSK file #.");
      errch_c  ( "#",  argv[0]);
      sigerr_c ( "SPICE(NODATA)");
    }
    fprintf(stderr,"%s is loaded.\n",argv[0]);
    --argc;
    ++argv;
    
    
    
    //loading kernel files
    while (argc > 0) {
    furnsh_c (argv[0]);
    fprintf(stderr,"%s is loaded.\n",argv[0]);
    --argc;
    ++argv;
  }
  
  if (!get_cam_info (instid, image_center, image_size, &pixel_size, &focal_length)) {
    fprintf(stderr,"Camera %d is not found.\n",instid);
    exit(0);
  }

  sprintf (instr,"FRAME_%d_NAME",instid);
  gcpool_c ( instr, 0, ROOM, STRLEN, &n_return, instr, &found );
  fprintf(stderr,"Use %s \n",instr);
 
  naxis[0] = (long int)image_size[0];
  naxis[1] = (long int)image_size[1];
  
  //Get the boresight vector of AMICA
  getfov_c ( instid, BCVLEN, STRLEN, STRLEN,
          shape, frame,  bsight, &n_bcv, bounds );
  nvp2pl_c(bsight, origin, &image_plane);
  
  erract_c ("SET",STRLEN,"RETURN");
      
  //Prepare large buffer
  range_img     = (double *)malloc(sizeof(double)*naxis[0]*naxis[1]);
  incidence_img  = (double *)malloc(sizeof(double)*naxis[0]*naxis[1]);
  emission_img  = (double *)malloc(sizeof(double)*naxis[0]*naxis[1]);
  phase_img  = (double *)malloc(sizeof(double)*naxis[0]*naxis[1]);
  lat_img  = (double *)malloc(sizeof(double)*naxis[0]*naxis[1]);
  lon_img  = (double *)malloc(sizeof(double)*naxis[0]*naxis[1]);
  aux_img  = (double *)malloc(sizeof(double)*naxis[0]*naxis[1]);
  hapke_img  = (double *)malloc(sizeof(double)*naxis[0]*naxis[1]);
  strike_img  = (double *)malloc(sizeof(double)*naxis[0]*naxis[1]);
  plid_img  = (double *)malloc(sizeof(double)*naxis[0]*naxis[1]);
  
  if (strlen(start_utc) == 0 && !FREEVIEW && !TI) {
    prompt_c ( "Input epoch (UTC): ", STRLEN, start_utc );
  }
  if (!FREEVIEW) {
    if (!TI) str2et_c (start_utc,&start_et);
    else scs2e_c (SCID, ti, &start_et );
  }
  for(i = 0; i < nloop; i++){
    et = start_et + step_et * i;
    if (!FREEVIEW) {
      timout_c ( et, "YYYY-MM-DDTHRMNSC::UTC", STRLEN, base_fname_2 );
      fprintf(stderr, "%s\n", base_fname_2);
      
      //To Get S/C position in the body-fixed frame of the target at the specified epoch,
      //Get the target position from the S/C as the origin in in the body-fixed frame,
      //then invert it.
      if (!EXTERNALPRM) {
        spkpos_c ( TARGETBODY,    et,   BODYFIXEDFRAME,  "LT+S", SCNAME,  SC_pos_bd,  &lt);
        vminus_c (SC_pos_bd,SC_pos_bd);
      }
      //Compute a matrix to rotate from the instrument frame to the body-fixed frame
      if (DIRECTION) {
        pxform_c ( ASTEROIDPOINTINGFRAME, BODYFIXEDFRAME, et, rotate);
        pxform_c ( frame, SCFRAME, et, rotate_tmp);
        mxm_c(rotate, rotate_tmp, rotate);
      } else if(!EXTERNALPRM) {
        pxform_c ( frame, BODYFIXEDFRAME, et, rotate);
      }
      
    } else if (!FREEVIEWPRM){
      fprintf(stderr, "==================== Free viewing angle mode ==================\n");
      fprintf(stderr, "Viewing/lighting parameter is not provided. Please input here.\n");
      prompt_c ( "sub-S/C lat: ", STRLEN, instr );
      SC_lat = atof (instr) * rpd_c();
      prompt_c ( "sub-S/C lon: ", STRLEN, instr );
      SC_lon = atof (instr) * rpd_c();
    }
    if (FREEVIEW) {
      latrec_c ( DISTANCE, SC_lon, SC_lat, SC_pos_bd );
      sprintf(base_fname_2, "%3.2f_%3.2f",SC_lat*dpr_c(),SC_lon*dpr_c());
      
      vminus_c(SC_pos_bd,SC_pos_bdm);
      rotate_z = 0;
      rotate_y = -atan2 ((double) SC_pos_bdm[0], (double) SC_pos_bdm[2]);
      rotate_x = atan2 ((double) SC_pos_bdm[1],
                  sqrt(SC_pos_bdm[2]*SC_pos_bdm[2]+SC_pos_bdm[0]*SC_pos_bdm[0]));
      eul2m_c (     rotate_z,  rotate_y,  rotate_x, 3,   2,   1,   rotate );  
    }
    
    if (!FREEVIEW) {
      //Compute an apparent direction to the Sun from Itokawa at the epoch et-lt
      spkpos_c ( "SUN",    et-lt,   BODYFIXEDFRAME,  "LT+S", TARGETBODY,  solar_pos_bd,  &lt);
    } else if (!FREEVIEWPRM){
      prompt_c ( "sub-Solar lat: ", STRLEN, instr );
      solar_lat = atof (instr) * rpd_c();
      prompt_c ( "sub-Solar lon: ", STRLEN, instr );
      solar_lon = atof (instr) * rpd_c();
    } 
    if (FREEVIEW) {
      latrec_c ( SOLAR_DISTANCE, solar_lon, solar_lat, solar_pos_bd );
    }
    
    //vpack_c(-0.081031, 4.771576, 4.355170, SC_pos_bd);
    
    fprintf(stderr,"S/C position (Body-Fixed):\n");
    fprintf(stderr,"    X   = %16.5f\n", SC_pos_bd[0]);
    fprintf(stderr,"    Y   = %16.5f\n", SC_pos_bd[1]);
    fprintf(stderr,"    Z   = %16.5f\n", SC_pos_bd[2]);
    fprintf(stderr,"Solar position (Body-Fixed):\n");
    fprintf(stderr,"    X   = %16.5f\n", solar_pos_bd[0]);
    fprintf(stderr,"    Y   = %16.5f\n", solar_pos_bd[1]);
    fprintf(stderr,"    Z   = %16.5f\n", solar_pos_bd[2]);
    
    mxv_c (rotate,bsight,bsight);
    vhat_c (bsight,bsight);
    fprintf(stderr,"The central LOS vector (body-fixed):\n");
    fprintf(stderr,"    X   = %16.5f\n", bsight[0]);
    fprintf(stderr,"    Y   = %16.5f\n", bsight[1]);
    fprintf(stderr,"    Z   = %16.5f\n", bsight[2]);
    
    timout_c ( et, "YYYY-MM-DDTHR:MN:SC::UTC", STRLEN, start_utc );
    fprintf(stderr, "%s\t", start_utc);
    fprintf(stderr,"%16.5f\t%16.5f\t%16.5f\t",
      SC_pos_bd[0], SC_pos_bd[1], SC_pos_bd[2]);
    fprintf(stderr,"%16.5f\t%16.5f\t%16.5f\n",
      bsight[0], bsight[1], bsight[2]);
    
    invert_c (rotate, rotate_inv);
    
    for (pixel[1] = 0; pixel[1] < naxis[1]; ++pixel[1]) {
      if(!(pixel[1]%100)) {
        fprintf(stderr, "\rline: %ld", pixel[1]);
        fflush(NULL);
      }
      for (pixel[0] = 0; pixel[0] < naxis[0]; ++pixel[0]) {
      
        pixel_dbl[0] = (SpiceDouble)pixel[0];
        pixel_dbl[1] = (SpiceDouble)pixel[1];
        ONC_getlos_c (instid, pixel_dbl, bsight);
        
        //address of the pixel in the image array
        address = pixel[1]*naxis[0] + pixel[0]; 
        
        mxv_c (rotate,bsight,bsight_bd);
        get_plbore ( SC_pos_bd, solar_pos_bd, bsight_bd,
                    &PLID, surface_point, plnorm,
                    &incidence, &emission, &phase,
                    &illuminated,
                    &found,
                    dladsc,
                    handle
                    );
          
        if (!found) { //LOS looks background deep space
          range_img[address] = (double)(0.0);
          incidence_img[address] = (double)(180.0);
          emission_img[address] = (double)(180.0);
          phase_img[address] = (double)(180.0);
          lat_img[address] = (double)(-1.0);
          lon_img[address] = (double)(-1.0);
          if (AUX) aux_img[address] = (double)(0.0);
          hapke_img [address] = 0.0;
          strike_img [address] = 0.0;
          plid_img [address] = 0.0;
        }
          
        if (found) { //LOS looks the target body
          reclat_c ( surface_point, &distan, &lon, &lat);
          lat =   dpr_c()*lat;
          lon =   dpr_c()*lon;
          if (lon < 0) lon += 360.00;
          range = vdist_c(SC_pos_bd,surface_point);
          range_img[address] = (double)range;
          incidence_img[address] = (double)dpr_c()*incidence;
          emission_img[address] = (double)dpr_c()*emission;
          phase_img[address] = (double)dpr_c()*phase;
          lat_img[address] = (double)lat;
          lon_img[address] = (double)lon;
          plid_img[address] = (double)PLID;
          if (AUX) aux_img[address] = (double)aux[PLID*2+1];
        
          mxv_c (rotate_inv,plnorm,plnorm);
          vprjp_c (plnorm, &image_plane, plnorm);
          strike_img[address] = (double)dpr_c()*atan2(plnorm[1],plnorm[0]);
        
          if (!illuminated) { //LOS looks an shaded area of the target body
            hapke_img [address] = 0.0;
          } else {
            hapke_img [address] = hapke ((double)dpr_c()*incidence, 
                        (double)dpr_c()*emission, (double)dpr_c()*phase);
            hapke_sum += hapke_img [address];
          }
        }
      }
    }
  
    fprintf(stderr,"\n\n");
    if (LIGHTCURVE) {
      printf("%s\t%f\n",base_fname_2, hapke_sum);
    }
  
    if (GRID) draw_grid(naxis, pixel, GRID_SPACING);
    
    //Prepare file name. The prefix "!" means that files are always created in the overwrite mode.
    sprintf(range_fname, "!%s%s_range.fits",base_fname_1,base_fname_2);
    sprintf(incidence_fname, "!%s%s_i.fits",base_fname_1,base_fname_2);
    sprintf(emission_fname, "!%s%s_e.fits",base_fname_1,base_fname_2);
    sprintf(phase_fname, "!%s%s_g.fits",base_fname_1,base_fname_2);
    sprintf(lat_fname, "!%s%s_lat.fits",base_fname_1,base_fname_2);
    sprintf(lon_fname, "!%s%s_lon.fits",base_fname_1,base_fname_2);
    sprintf(hapke_fname, "!%s%s_hapke.fits",base_fname_1,base_fname_2);
    sprintf(aux_fname, "!%s%s_a.fits",base_fname_1,base_fname_2);
    sprintf(strike_fname, "!%s%s_s.fits",base_fname_1,base_fname_2);
    sprintf(plid_fname, "!%s%s_plid.fits",base_fname_1,base_fname_2);
    
    //Output fits files
    status = 0;
    if (RANGE) {
      fits_create_file(&range_fptr, range_fname, &status);
      fits_create_img(range_fptr, DOUBLE_IMG, dim, naxis, &status);
      fits_write_2d_dbl(range_fptr, 0, naxis[0], naxis[0], naxis[1], range_img, &status);
      fits_close_file(range_fptr, &status);
    }
  
    if (ILLIMINATION) {
      fits_create_file(&incidence_fptr, incidence_fname, &status);
      fits_create_img(incidence_fptr, DOUBLE_IMG, dim, naxis, &status);
      fits_write_2d_dbl(incidence_fptr, 0, naxis[0], naxis[0], naxis[1], incidence_img, &status);
      fits_close_file(incidence_fptr, &status);
      
      fits_create_file(&emission_fptr, emission_fname, &status);
      fits_create_img(emission_fptr, DOUBLE_IMG, dim, naxis, &status);
      fits_write_2d_dbl(emission_fptr, 0, naxis[0], naxis[0], naxis[1], emission_img, &status);
      fits_close_file(emission_fptr, &status);
      
      fits_create_file(&phase_fptr, phase_fname, &status);
      fits_create_img(phase_fptr, DOUBLE_IMG, dim, naxis, &status);
      fits_write_2d_dbl(phase_fptr, 0, naxis[0], naxis[0], naxis[1], phase_img, &status);
      fits_close_file(phase_fptr, &status);
    }
  
    if (LATLON) {
      fits_create_file(&lat_fptr, lat_fname, &status);
      fits_create_img(lat_fptr, DOUBLE_IMG, dim, naxis, &status);
      fits_write_2d_dbl(lat_fptr, 0, naxis[0], naxis[0], naxis[1], lat_img, &status);
      fits_close_file(lat_fptr, &status);
      
      fits_create_file(&lon_fptr, lon_fname, &status);
      fits_create_img(lon_fptr, DOUBLE_IMG, dim, naxis, &status);
      fits_write_2d_dbl(lon_fptr, 0, naxis[0], naxis[0], naxis[1], lon_img, &status);
      fits_close_file(lon_fptr, &status);
    }
    
    if (AUX) {
      fits_create_file(&aux_fptr, aux_fname, &status);
      fits_create_img(aux_fptr, DOUBLE_IMG, dim, naxis, &status);
      fits_write_2d_dbl(aux_fptr, 0, naxis[0], naxis[0], naxis[1], aux_img, &status);
      fits_close_file(aux_fptr, &status);
    }
  
    if (HAPKE) {
      fits_create_file(&hapke_fptr, hapke_fname, &status);
      fits_create_img(hapke_fptr, DOUBLE_IMG, dim, naxis, &status);
      fits_write_2d_dbl(hapke_fptr, 0, naxis[0], naxis[0], naxis[1], hapke_img, &status);
      fits_close_file(hapke_fptr, &status);
    }
    
    if (GEOMETRY) {
      fits_create_file(&strike_fptr, strike_fname, &status);
      fits_create_img(strike_fptr, DOUBLE_IMG, dim, naxis, &status);
      fits_write_2d_dbl(strike_fptr, 0, naxis[0], naxis[0], naxis[1], strike_img, &status);
      fits_close_file(strike_fptr, &status);
    }
    
    if (POLYGONID) {
      fits_create_file(&plid_fptr, plid_fname, &status);
      fits_create_img(plid_fptr, DOUBLE_IMG, dim, naxis, &status);
      fits_write_2d_dbl(plid_fptr, 0, naxis[0], naxis[0], naxis[1], plid_img, &status);
      fits_close_file(plid_fptr, &status);
    }
    
    if (status != 0) {
      fprintf(stderr,"file create error!\n");
      exit(-1);
    }
  }
}