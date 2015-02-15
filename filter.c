#include "filter.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
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


void free_filter(struct filter_s filt){

    fftw_free(filt.raw_vals);
    free(filt.vals);
    filt.raw_vals = NULL;
    filt.vals = NULL;

}


