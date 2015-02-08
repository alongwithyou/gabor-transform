#include "convolve.h"

#include <stdio.h>
#include <complex.h>
#include <fftw3.h>


fftw_plan img_in_plan;
fftw_plan filt_plan;
fftw_plan img_out_plan;
int fft_isplanned = 0;


// FFTW uses the image origin (0,0) as the FFT origin.
// Normally it doesn't matter because I can take the abs in the freq. domain
// But, Gabor filter is a complex filter, so I can't.
// Thus, I need to get the phase to align before taking the FFT.
void shift_filter(struct image_s filt){

    // Make a target image
    struct image_s filt_shift = init_image_empty(filt.height, filt.width);

    // Find the center pixel
    int center_x = filt.width/2;
    int center_y = filt.height/2;

    // Swap the top-left and bottom-right fields, bringing the center to 0,0.
    for (unsigned int i = 0; i < center_y; i++){
        for (unsigned int j = 0; j < center_x; j++){
            filt_shift.vals[i][j] = filt.vals[i+center_y][j+center_x];
        }
    }
    for (unsigned int i = 0; i < center_y; i++){
        for (unsigned int j = 0; j < center_x; j++){
            filt_shift.vals[i+center_y][j+center_x] = filt.vals[i][j];
        }
    }

    // Swap the bottom-left and top-right fields
    for (unsigned int i = center_y; i < filt.height; i++){
        for (unsigned int j = 0; j < center_x; j++){
            filt_shift.vals[i][j] = filt.vals[i-center_y][j+center_x];
        }
    }
    for (unsigned int i = center_y; i < filt.height; i++){
        for (unsigned int j = 0; j < center_x; j++){
            filt_shift.vals[i-center_y][j+center_x] = filt.vals[i][j];
        }
    }

    // Copy back and free the results
    for (unsigned int i = 0; i < filt.height*filt.width; i++){
        filt.raw_vals[i] = filt_shift.raw_vals[i];
    }
    free_image(filt_shift);

}

// This is as unoptomized as the American Congress
void convolve_spatial(struct image_s img_in, struct image_s img_out, struct image_s filt){

    // Find the center pixel
    int center_x = filt.width/2;
    int center_y = filt.height/2;

    // iterate over each image pixel
    for (unsigned int i = 0; i < img_in.height; i++){
        printf("%u\n", i);
        for (unsigned int j = 0; j < img_in.width; j++){

            img_out.vals[i][j] = 0;

            // Iterate over each filter pixel
            for (unsigned int m = 0; m < filt.height; m++){
                for (unsigned int n = 0; n < filt.width; n++){

                    // Convolve
                    int img_x = ((n - center_x) + j) % img_in.width;
                    int img_y = ((m - center_y) + i) % img_in.height;
                    img_out.vals[i][j] += img_in.vals[img_x][img_y] * filt.vals[m][n];

                }
            }

        }
    }

}



void convolve_frequency(struct image_s img_in_raw, struct image_s img_out_raw, struct image_s filt_raw){

    // get image dims
    int height = img_in_raw.height;
    int width = img_in_raw.width;

    // Allocate all 5 needed images
    struct image_s img = init_image_empty(height, width);
    struct image_s img_fft = init_image_empty(height, width);
    struct image_s filt = init_image_empty(height, width);
    struct image_s filt_fft = init_image_empty(height, width);

    // Make the FFT plans
    printf("Planning...");
    fflush(stdout);
    img_in_plan = fftw_plan_dft_2d(height, width, img.raw_vals, img_fft.raw_vals, FFTW_FORWARD, FFTW_MEASURE);
    printf("...");
    fflush(stdout);
    filt_plan = fftw_plan_dft_2d(height, width, filt.raw_vals, filt_fft.raw_vals, FFTW_FORWARD, FFTW_MEASURE);
    printf("...");
    fflush(stdout);
    img_out_plan = fftw_plan_dft_2d(height, width, img_fft.raw_vals, img.raw_vals, FFTW_BACKWARD, FFTW_MEASURE);
    printf("done\n");
    fft_isplanned = 1;

    // Flag the FFT as planned
    if (~fft_isplanned){
        fft_isplanned = 1;
    }

    // Copy the image and filter data into the planned arrays
    for (unsigned int i = 0; i < width*height; i++){
        img.raw_vals[i] = img_in_raw.raw_vals[i];
    }
    for (unsigned int i = 0; i < width*height; i++){
        filt.raw_vals[i] = filt_raw.raw_vals[i];
    }

    // Shift the filter
    shift_filter(filt);

    // Execute the FFTs
    fftw_execute(img_in_plan);
    fftw_execute(filt_plan);

    // Perform pointwise multiplication
    for (unsigned int i = 0; i < width*height; i++){
        img_fft.raw_vals[i] *= filt_fft.raw_vals[i];
    }

    // Execute the inverse transform
    fftw_execute(img_out_plan);

    // Copy data to output image, with normalization
    for (unsigned int i = 0; i < width*height; i++){
        img_out_raw.raw_vals[i] = img.raw_vals[i]/(height*width);
    }

    // Free everything we made
    free_image(img);
    free_image(img_fft);
    free_image(filt);
    free_image(filt_fft);

}

void cleanup_fftw(){
    if (fft_isplanned){
        fftw_destroy_plan(img_in_plan);
        fftw_destroy_plan(filt_plan);
        fftw_destroy_plan(img_out_plan);
        fft_isplanned = 0;
    }
}
