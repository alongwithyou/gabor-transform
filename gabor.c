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

    // Add the low-pass filter in
    /*
    bank.angles[16] = 0;
    bank.freqs[16] = 0;
    bank.sigmas[16] = (3*sqrt(2*log(2))) / (4*PI*bank.freqs[15]);
    */


    return bank;

}


struct gabor_filter_bank_s init_gabor_filter_bank_exhaustive(const unsigned int height, const unsigned int width){

    // The frequency standard deviation for the Gabor filters
    const double sigmafreq1 = 0.025;
    const double sigmafreq2 = 0.05;

    // The maximum number of filters that fit in the image half-width
    int filt_count1 = (0.5)/(2*sigmafreq1*sqrt(2*log(2)));
    int filt_count2 = (0.5)/(2*sigmafreq2*sqrt(2*log(2)));

    const int num_filters = (filt_count1*2) * (filt_count1) + (filt_count2*2) * (filt_count2);

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
    for (int n = (-1*filt_count1); n < filt_count1; n++){
        for (int m = 0; m < filt_count1; m++){

            double xfreq = 2*n*sigmafreq1*sqrt(2*log(2)) + sigmafreq1*sqrt(2*log(2));
            double yfreq = 2*m*sigmafreq1*sqrt(2*log(2)) + sigmafreq1*sqrt(2*log(2));

            bank.sigmas[i] = 1.0/(2*PI*sigmafreq1);
            bank.freqs[i] = sqrt(pow(xfreq, 2) + pow(yfreq, 2));
            bank.angles[i] = atan2(yfreq, xfreq);

            i++;

        }
    }

    for (int n = (-1*filt_count2); n < filt_count2; n++){
        for (int m = 0; m < filt_count2; m++){

            double xfreq = 2*n*sigmafreq2*sqrt(2*log(2)) + sigmafreq2*sqrt(2*log(2));
            double yfreq = 2*m*sigmafreq2*sqrt(2*log(2)) + sigmafreq2*sqrt(2*log(2));

            bank.sigmas[i] = 1.0/(2*PI*sigmafreq2);
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
            //filt.vals[i][j] = cexp((double complex)I*2*PI*freq*x);

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

    FILE* fid = fopen("filterdata.csv", "w");

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

        double max_val = DBL_MIN;

        fprintf(fid, "%u,%f,%f,%f\n",f, bank.freqs[f], bank.angles[f], bank.sigmas[f]);

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

        for (unsigned int i = 0; i < width*height; i++){
            if (abs(filt_fft.raw_vals[i]) > max_val){
                max_val = abs(filt_fft.raw_vals[i]);
            }
        }

        // add the filter to the image response
        for (unsigned int i = 0; i < width*height; i++){
            img.raw_vals[i] += filt_fft.raw_vals[i]/max_val;
        }

        // Free the temporary filter
        free_filter(temp_filt);

    }

    // Future me, I am tired and I am SO SORRY for writing this line of code.
    shift_filter(*((struct filter_s*)&img));

    snprintf(filtname, 200, "%s_fourier", prefix);
    save_image_autoscale(img, filtname);

    fclose(fid);
    free_image(img);
    free_filter(filt);
    free_filter(filt_fft);

}




void save_gabor_responses(struct gabor_responses_s resps, const char* const prefix){

    FILE* fid;
    char filename[200];

    snprintf(filename, 200, "%s.dat", prefix);

    fid = fopen(filename, "w");

    unsigned int height = resps.channels[0].height;
    unsigned int width = resps.channels[0].width;

    fwrite(&height, sizeof(height), 1, fid);
    fwrite(&width, sizeof(width), 1, fid);
    fwrite(&resps.num_channels, sizeof(resps.num_channels), 1, fid);

    for (unsigned int i = 0; i < resps.num_channels; i++){

        fwrite(resps.channels[i].raw_vals, sizeof(resps.channels[i].raw_vals[0]), width*height, fid);

    }

    fclose(fid);

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

