#include "gabor.h"
#include "convolve.h"

#include <complex.h>
#include <fftw3.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.1415926535897932384

// Create a bank of Gabor filters for processing
struct gabor_filter_bank_s init_gabor_filter_bank(int filt_height, int filt_width){

    struct gabor_filter_bank_s bank;

    int i = 0;
    const double k = (1.0/3.0)*sqrt(PI/log(2)); // relationship between frequency and gaussian width

    // Loop over each frequency and angle
    for (int angle = 0; angle < 8; angle++){
        for (int freq = 0; freq < 4; freq++){

            // Compute the frequency term
            double f = 1.0/pow(2.0, (double)freq + 2.0);

            // Compute the gaussian width term
            double alpha = k * f;

            // Build each filter
            bank.filters[i] = init_gabor_filter(f, (PI/4.0)*angle + (PI/8.0), alpha, filt_height, filt_width);

            // Populate the structure
            bank.freq[i] = f;
            bank.angle[i] = (PI/4.0)*angle;
            bank.alpha[i] = alpha;
            i++;
        }
    }

    return bank;

}

void free_gabor_filter_bank(struct gabor_filter_bank_s filt_bank){

    for (int i = 0; i < 32; i++){
        free_image(filt_bank.filters[i]);
    }

}

struct image_s init_gabor_filter(double freq, double angle, double alpha, int filt_height, int filt_width){

    struct image_s filt;

    if (filt_width == 0 || filt_height == 0){
        filt.width = ceil((1.0/alpha)*10)+1;
        filt.height = ceil((1.0/alpha)*10)+1;
    } else {
        filt.width = filt_width;
        filt.height = filt_height;
    }

    // Find the center pixel
    int center_x = filt.width/2;
    int center_y = filt.height/2;

    // Allocate the filter array
    filt.raw_vals = (double complex*)fftw_malloc(filt.width*filt.height*sizeof(double complex));
    if (filt.raw_vals == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }

    // Make an array of pointers into each row for 2d indexing
    filt.vals = (double complex**)malloc(filt.height*sizeof(double complex*));
    if (filt.vals == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }
    for (unsigned int i = 0; i < filt.height; i++){
        filt.vals[i] = filt.raw_vals + filt.width*i;
    }

    // Populate the filter with proper values (http://en.wikipedia.org/wiki/Gabor_filter)
    for (int i = 0; i < filt.height; i++){
        for (int j = 0; j < filt.width; j++){
            double x = j*cos(angle) + i*sin(angle); // Gaussian is rot. symm, we only need this for sinusoid

            // Wikipedia formula
            //filt.vals[i][j] = cexp(-1*(pow((j-center_x), 2) + pow((i-center_y), 2))/(2*pow(sigma, 2)))*cexp((double complex)I*2*PI*freq*x);

            // adjusted from Navarro
            filt.vals[i][j] = pow(alpha, 2)*exp(-1*PI*pow(alpha, 2)*(pow((j-center_x), 2) + pow((i-center_y), 2)))*cos(2*PI*freq*x);
            //filt.vals[i][j] = pow(alpha, 2)*(cexp(-1*PI*pow(alpha, 2)*(pow((j-center_x), 2) + pow((i-center_y), 2)))*cexp((double complex)I*2*PI*freq*x));
        }
    }

    return filt;

}


void display_filter_bank(struct gabor_filter_bank_s bank){

    // get image dims
    int height = bank.filters[0].height;
    int width = bank.filters[0].width;

    // Allocate all needed images
    struct image_s disp_img = init_image_empty(height, width);
    struct image_s filt = init_image_empty(height, width);
    struct image_s filt_fft = init_image_empty(height, width);


    // Zero out the display image
    for (int i = 0; i < width*height; i++){
        disp_img.raw_vals[i] = 0.0;
    }

    // Make the FFT plan
    fftw_plan filt_plan = fftw_plan_dft_2d(height, width, filt.raw_vals, filt_fft.raw_vals, FFTW_FORWARD, FFTW_MEASURE);

    // Loop over each Gabor filter
    for (int j = 0; j < 32; j++){

        // Copy the filter into the buffer
        for (unsigned int i = 0; i < width*height; i++){
            filt.raw_vals[i] = bank.filters[j].raw_vals[i];
        }

        // Shift the filter for processing
        shift_filter(filt);

        // Execute the FFT
        fftw_execute(filt_plan);

        // Add the absolute value back on
        for (int i = 0; i < width*height; i++){
            disp_img.raw_vals[i] += cabs(filt_fft.raw_vals[i]);
        }

    }

    // Center for viewing
    shift_filter(disp_img);

    save_image(disp_img, "coverage");

    // Free everything we made
    free_image(disp_img);
    free_image(filt);
    free_image(filt_fft);

}
