#include "filter.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <fftw3.h>

struct filter_s init_filter_empty(const unsigned int height, const unsigned int width){

    struct filter_s filt;

    filt.width = width;
    filt.height = height;


    // Allocate the filter array
    filt.raw_vals = (double complex*)fftw_malloc(width*height*sizeof(double complex));
    if (filt.raw_vals == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }

    // Make an array of pointers into each row for 2d indexing
    filt.vals = (double complex**)malloc(height*sizeof(double complex*));
    if (filt.vals == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }
    for (unsigned int i = 0; i < height; i++){
        filt.vals[i] = filt.raw_vals + filt.width*i;
    }

    return filt;

}





struct filter_s init_filter_gaussian(const unsigned int height, const unsigned int width, const double sigma){

    struct filter_s filt;

    // Initialize the filter
    filt = init_filter_empty(height, width);

    // Find the center pixel
    int center_x = filt.width/2;
    int center_y = filt.height/2;

    double sum = 0;

    // Populate the filter with proper values
    for (int i = 0; i < filt.height; i++){
        for (int j = 0; j < filt.width; j++){

            double x = j - center_x;
            double y = i - center_y;

            // Build the filter
            filt.vals[i][j] = cexp(-1 * (pow(x, 2) + pow(y,2))/(2 * pow(sigma, 2)));

            sum += filt.vals[i][j];

        }
    }

    // Normalize the filter
    for (int i = 0; i < filt.height * filt.width; i++){
        filt.raw_vals[i] /= sum;
    }

    return filt;
}






void free_filter(struct filter_s filt){

    fftw_free(filt.raw_vals);
    free(filt.vals);
    filt.raw_vals = NULL;
    filt.vals = NULL;

}


