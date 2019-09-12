#include "plate_renderer.h"

void draw_grid(long int *naxis, long int *pixel, int GRID_SPACING) {
  SpiceInt    address, address_col;
  
    for (pixel[1] = 1; pixel[1] < naxis[1]; ++pixel[1]) {
      if(!(pixel[1]%100)) {
        printf ( "\r drawing grid lines... line: %ld", pixel[1]);
        fflush(NULL);
      }
    
    for (pixel[0] = 1; pixel[0] < naxis[0]; ++pixel[0]) {
        address = pixel[1]*naxis[0] + pixel[0]; 
        address_col = (pixel[1]-1)*naxis[0] + pixel[0]; 
        
        if (lat_img[address] != -1.0 && lat_img[address-1] != -1.0 && lat_img[address_col] != -1.0) {
          if (lat_img[address] * lat_img[address-1] < 0.0 ||
                  lat_img[address] * lat_img[address_col] < 0.0 ) {
            incidence_img[address] = (double)(0.0);
            emission_img[address] = (double)(180.0);
            phase_img[address] = (double)(180.0);
            hapke_img [address] = 0.1;
          }
        }
      }
    }
    
    //Draw latitudal grid
    for (pixel[1] = 1; pixel[1] < naxis[1]; ++pixel[1]) {
      if(!(pixel[1]%100)) {
        printf ( "\r drawing grid lines... line: %ld", pixel[1]);
        fflush(NULL);
      }
    
    for (pixel[0] = 1; pixel[0] < naxis[0]; ++pixel[0]) {
        address = pixel[1]*naxis[0] + pixel[0]; 
        address_col = (pixel[1]-1)*naxis[0] + pixel[0]; 
        
        if (lat_img[address] != -1.0 && lat_img[address-1] != -1.0 && lat_img[address_col] != -1.0) {
          if (((int)lat_img[address] / GRID_SPACING) != ((int)lat_img[address-1] / GRID_SPACING) ||
                  ((int)lat_img[address] / GRID_SPACING) != ((int)lat_img[address_col] / GRID_SPACING) ) {
            incidence_img[address] = (double)(0.0);
            emission_img[address] = (double)(180.0);
            phase_img[address] = (double)(180.0);
            hapke_img [address] = 0.1;
          }
        }
      }
    }

    //Draw longitudal grid
    for (pixel[1] = 1; pixel[1] < naxis[1]; ++pixel[1]) {
      if(!(pixel[1]%100)) {
        printf ( "\r drawing grid lines... line: %ld", pixel[1]);
        fflush(NULL);
      }

    for (pixel[0] = 1; pixel[0] < naxis[0]; ++pixel[0]) {
        address = pixel[1]*naxis[0] + pixel[0]; 
        address_col = (pixel[1]-1)*naxis[0] + pixel[0]; 
        
        if (lon_img[address] != -1.0 && lon_img[address-1] != -1.0 && lon_img[address_col] != -1.0) {
          if (((int)lon_img[address] / GRID_SPACING) != ((int)lon_img[address-1] / GRID_SPACING) ||
                  ((int)lon_img[address] / GRID_SPACING) != ((int)lon_img[address_col] / GRID_SPACING) ) {
            incidence_img[address] = (double)(0.0);
            emission_img[address] = (double)(180.0);
            phase_img[address] = (double)(180.0);
            hapke_img [address] = 0.1;
          }
        }
      }
    }
    printf ( "\n");
}