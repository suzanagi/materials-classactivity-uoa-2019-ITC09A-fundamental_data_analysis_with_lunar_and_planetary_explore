# Requierd libraries
- SPICE toolkit https://naif.jpl.nasa.gov/naif/toolkit.html
- CFITSIO http://heasarc.gsfc.nasa.gov/fitsio/fitsio.html
  - also available in several package manegament systems (MacPorts and homebrew for macOS)

# HowToCompile
- Modify src/Makefile to fit your environment.
- Choose your mission: Hayabusa1 or Hayabusq2.
- Then type "make"

# Usage

> ./plate_renderer [options] shape_kernel_to_load kernels_to_load

Although SPICE toolkit N66 allows to load it with furnsh_c, the current plate_renderer load it as a DAS file with dasopr_c to use DSK Level 2 functions. Thus, plate_renderer loads a DSK shape kernel independently and separately. It assume that a DSK shape kernel file will be provided at the first of the list of kernels to load.

If you will use a simple kernel list, a DSK shape kernel should be placed at the first line of the list. If you will use a meta kernel, a DSK shape kernel should not be inluceded in the meta kernel, and it should be placed in arguments independently and separately.

# Options
> -a/-aux auxiliary_data_file
>    Output a aux plane mapped with the auxiliary_data
> -b/-basefilename finename_base_string
>    Define a base string for output files
> -g/-grid grid_spacing
>    Draw lat-lon grid
> -p/-polygonid 
>    Output a polygon ID plane
> -e/-geo 
>    Output geometry information planes
> -r 
>    Output a S/C-target range plane
> -l 
>    Output illumination condition planes
> -n 
>    Output lat and lon plane
> -c/-lightcurve 
>    Output integrated brightness information for lightcurve analysis
> -h/-nohapke 
>    Suppress output of a hapke rendering plane
> -u/-utc [UTC_EPOCH[/TIME_STEP/LOOP_NUMBERR]]
>    Define the epoch in UTC
>    If arguments of the option are not provided, The tool ask UTC_EPOCH with a prompt
>    Multiple image output is also possibele
>      by defineing timestep and loop number.
>    Example: -u 2005-10-01T00:00:00/3600/12
>      will output 100 images with 1 hour interval
>      from 2005-10-01T00:00:00 to 2005-10-01T11:00:00
> -t/-ti TI
>    Define the epoch in TI
>    TI should be supplied as an argument of this option
> -d/-direction 
>    S/C pointing is freezed to veiw the asteroid center
> -V/-VIEW 
>    Enable free viewing angle mode
> -v/-view [SC_lat/SC_lon/SUN_lat_SUN_lon]
>    Enable free viewing angle mode with supplied parameters
>    If arguments of the option are not provided, The tool ask them with a prompt
> -x/-external 
>    Get UTC, S/C Position and pointing in Body-fixed frame from stdin
> -z/-All 
>    Output all additinal information planes
     
# Example (bash/zsh)
> % ./plate_renderer -u 2005-10-01T01:00:00 -g 30 -All ./kernels/Itokawa/dsk/hay_a_amica_5_itokawashape_v1_0_512q.bds $(cat kernel_list_rendezvous.txt)

will output an image at 2005-10-01T01:00:00 with grids of 30 degree interval and other all related files.