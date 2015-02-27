#include "gabor.h"
#include "convolve.h"
#include "filter.h"
#include "image.h"

#include <FreeImage.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <complex.h>
#include <fftw3.h>
#include <math.h>

#define PI 3.1415926535897932384


struct gabor_filter_bank_s init_gabor_filter_bank_default(const unsigned int height, const unsigned int width){

    const unsigned int num_filters = 16;

    struct gabor_filter_bank_s bank;

    bank.height = height;
    bank.width = width;
    bank.num_filters = num_filters;

    // Allocate the arrays within the filter bank
    bank.angles = (double*)malloc(num_filters*sizeof(double));
    if (bank.angles == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }
    bank.sigmas = (double*)malloc(num_filters*sizeof(double));
    if (bank.sigmas == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }
    bank.freqs = (double*)malloc(num_filters*sizeof(double));
    if (bank.freqs == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }

    // Populate values
    unsigned int i = 0;
    for (unsigned int a = 0; a < 4; a++){
        for (unsigned int f = 0; f < 4; f++){

            // Distributed on pi/4 angles
            bank.angles[i] = (PI/4.0)*a;

            // In cyc/px
            bank.freqs[i] = pow(0.5, f+2);

            // See derivation for the sqrt term
            bank.sigmas[i] = (3*sqrt(log(2))) / (sqrt(2)*PI*bank.freqs[i]);

            i++;

        }
    }

    return bank;

}


struct gabor_filter_bank_s init_gabor_filter_bank_exhaustive(const unsigned int height, const unsigned int width){

    // The frequency standard deviation for the Gabor filters
    const double sigmafreq = 16.0/width;

    // The maximum number of filters that fit in the image half-width
    int filt_count = (0.5)/(2*sigmafreq*sqrt(2*log(2)));

    const int num_filters = (filt_count*2) * (filt_count);

    struct gabor_filter_bank_s bank;

    bank.height = height;
    bank.width = width;
    bank.num_filters = num_filters;

    // Allocate the arrays within the filter bank
    bank.angles = (double*)malloc(num_filters*sizeof(double));
    if (bank.angles == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }
    bank.sigmas = (double*)malloc(num_filters*sizeof(double));
    if (bank.sigmas == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }
    bank.freqs = (double*)malloc(num_filters*sizeof(double));
    if (bank.freqs == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }

    // when you change this, don't forget to change num_filters!
    // for now we're just looping over ints, but this will eventually be nested whiles
    unsigned int i = 0;
    for (int n = (-1*filt_count); n < filt_count; n++){
        for (int m = 0; m < filt_count; m++){

            double xfreq = 2*n*sigmafreq*sqrt(2*log(2)) + sigmafreq*sqrt(2*log(2));
            double yfreq = 2*m*sigmafreq*sqrt(2*log(2)) + sigmafreq*sqrt(2*log(2));

            bank.sigmas[i] = 1.0/(2*PI*sigmafreq);
            bank.freqs[i] = sqrt(pow(xfreq, 2) + pow(yfreq, 2));
            bank.angles[i] = atan2(yfreq, xfreq);

            i++;

        }
    }


    return bank;


}




struct gabor_responses_s init_gabor_responses_empty(const unsigned int height, const unsigned int width, const unsigned int num_filters){

    struct gabor_responses_s resps;
    resps.num_channels = num_filters;

    resps.channels = (struct image_s*)malloc(num_filters*sizeof(struct image_s));
        if (resps.channels == NULL){
            fprintf(stderr, "Malloc failed\n");
            exit(EXIT_FAILURE);
        }

    for (unsigned int i = 0; i < num_filters; i++){
        resps.channels[i] = init_image_empty(height, width);
    }

    return resps;

}






struct gabor_responses_s apply_gabor_filter_bank(struct image_s img, struct gabor_filter_bank_s bank){

    struct gabor_responses_s resps;

    resps = init_gabor_responses_empty(bank.height, bank.width, bank.num_filters);


    for (unsigned int i = 0; i < bank.num_filters; i++){

        struct filter_s filt = init_gabor_filter_from_bank(bank, i);

        convolve_frequency(img, resps.channels[i], filt);

        free_filter(filt);

    }

    return resps;

}






struct filter_s init_gabor_filter_from_params(const double freq, const double angle, const double sigma, const unsigned int filt_height, const unsigned int filt_width){

    struct filter_s filt;

    // Initialize the filter
    filt = init_filter_empty(filt_height, filt_width);

    // Find the center pixel
    int center_x = filt.width/2;
    int center_y = filt.height/2;

    // Populate the filter with proper values (http://en.wikipedia.org/wiki/Gabor_filter)
    for (int i = 0; i < filt.height; i++){
        for (int j = 0; j < filt.width; j++){

            double x_shift = j - center_x;
            double y_shift = i - center_y;

            double x = x_shift*cos(angle) + y_shift*sin(angle);
            double y = y_shift*cos(angle) - x_shift*sin(angle);

            // Build the filter
            //filt.vals[i][j] = pow(freq*sqrt(PI)/(3*sqrt(log(2))),2)*cexp(-1 * (pow(x, 2) + pow(y,2))/(2 * pow(sigma, 2))) * cexp((double complex)I*2*PI*freq*x);
            filt.vals[i][j] = (1/pow(sigma,2))*cexp(-1 * (pow(x, 2) + pow(y,2))/(2 * pow(sigma, 2))) * cexp((double complex)I*2*PI*freq*x);
        }
    }

    return filt;

}






struct filter_s init_gabor_filter_from_bank(struct gabor_filter_bank_s bank, const unsigned int filter_num){

    return init_gabor_filter_from_params(bank.freqs[filter_num], bank.angles[filter_num], bank.sigmas[filter_num], bank.height, bank.width);

}





struct image_s reconstruct_image_from_responses(struct gabor_responses_s resps){

    struct image_s img;

    unsigned int height = resps.channels[0].height;
    unsigned int width = resps.channels[0].width;


    img = init_image_empty(height, width);

    // Zero out the image
    for (unsigned int i = 0; i < height; i++){
        for (unsigned int j = 0; j < width; j++){
            img.vals[j][i] = 0;
        }
    }


    // Sum each channel into the image
    for (unsigned int c = 0; c < resps.num_channels; c++){

        for (unsigned int i = 0; i < height; i++){
            for (unsigned int j = 0; j < width; j++){
                img.vals[j][i] += creal(resps.channels[c].vals[j][i]);
            }
        }

    }

    return img;

}









void disp_gabor_filter_bank(struct gabor_filter_bank_s bank, const char* const prefix){

    // get image dims
    const unsigned int height = 512;
    const unsigned int width = 512;

    // Allocate all needed images
    struct image_s img = init_image_empty(height, width);
    struct filter_s filt = init_filter_empty(height, width);
    struct filter_s filt_fft = init_filter_empty(height, width);

    fftw_plan filt_plan = fftw_plan_dft_2d(height, width, filt.raw_vals, filt_fft.raw_vals, FFTW_FORWARD, FFTW_MEASURE);

    char filtname[200];

    for (unsigned int f = 0; f < bank.num_filters; f++){

        // initialize a temporary filter
        struct filter_s temp_filt = init_gabor_filter_from_params(bank.freqs[f], bank.angles[f], bank.sigmas[f], height, width);

        // Load it in
        for (unsigned int i = 0; i < width*height; i++){
            filt.raw_vals[i] = creal(temp_filt.raw_vals[i]);
        }

        snprintf(filtname, 200, "%s_%u", prefix, f);
        save_image_autoscale(*((struct image_s*)&filt), filtname);

        // Shift the filter
        shift_filter(filt);

        // Execute the plan
        fftw_execute(filt_plan);

        // add the filter to the image response
        for (unsigned int i = 0; i < width*height; i++){
            img.raw_vals[i] += filt_fft.raw_vals[i];
        }

        // Free the temporary filter
        free_filter(temp_filt);

    }

    // Future me, I am tired and I am SO SORRY for writing this line of code.
    shift_filter(*((struct filter_s*)&img));

    snprintf(filtname, 200, "%s_fourier", prefix);
    save_image_autoscale(img, filtname);

    free_image(img);
    free_filter(filt);
    free_filter(filt_fft);

}








void free_gabor_filter_bank(struct gabor_filter_bank_s bank){

    free(bank.angles);
    free(bank.freqs);
    free(bank.sigmas);

    bank.height = 0;
    bank.width = 0;

}






void free_gabor_responses(struct gabor_responses_s resps){

    for (int i = 0; i < resps.num_channels; i++){

        free_image(resps.channels[i]);

    }

    free(resps.channels);

}

