#include "gabor.h"
#include "image.h"
#include "convolve.h"

#include <FreeImage.h>
#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>

#define PI 3.1415926535897932384

struct gabor_filter_bank_s init_gabor_filter_bank_default(){

    const int num_filters = 16;

    struct gabor_filter_bank_s bank;

    // Allocate the arrays within the filter bank
    for (int i = 0; i < num_filters; i++){
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
    }

    // Populate values
    int i = 0;
    for (int a = 0; a < 4; a++){
        for (int f = 0; f < 4; f++){

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

void free_gabor_filter_bank(struct gabor_filter_bank_s bank){

    free(bank.angles);
    free(bank.freqs);
    free(bank.sigmas);

}

struct image_s init_gabor_filter(double freq, double angle, double sigma, int filt_height, int filt_width){

    struct image_s filt;

    // Initialize the filter
    filt = init_image_empty(filt_height, filt_width);

    // Find the center pixel
    int center_x = filt.width/2;
    int center_y = filt.height/2;

            printf("%f\n", freq);
    // Populate the filter with proper values (http://en.wikipedia.org/wiki/Gabor_filter)
    for (int i = 0; i < filt.height; i++){
        for (int j = 0; j < filt.width; j++){

            double x_shift = j - center_x;
            double y_shift = i - center_y;

            double x = x_shift*cos(angle) + y_shift*sin(angle);
            double y = y_shift*cos(angle) - x_shift*sin(angle);

            // adjusted from Navarro
            //filt.vals[i][j] = pow(alpha, 2)*(cexp(-1*PI*pow(alpha, 2)*(pow(x, 2) + pow(y, 2)))*cexp((double complex)I*2*PI*freq*x));

            // My version
            filt.vals[i][j] = pow(freq*sqrt(PI)/(3*sqrt(log(2))),2)*cexp(-1 * (pow(x, 2) + pow(y,2))/(2 * pow(sigma, 2))) * cexp((double complex)I*2*PI*freq*x);
            //filt.vals[i][j] = cexp((double complex)I*2*PI*freq*x);

            //filt.vals[i][j] = cexp(-1 * (pow(x, 2) + pow(y,2))/(2 * pow(sigma, 2)));

        }
    }

    return filt;

}


void disp_gabor_filter_bank(struct gabor_filter_bank_s bank){



    // get image dims
    int height = 512;
    int width = 512;

    // Allocate all needed images
    struct image_s img = init_image_empty(height, width);
    struct image_s filt = init_image_empty(height, width);
    struct image_s filt_fft = init_image_empty(height, width);

    fftw_plan filt_plan = fftw_plan_dft_2d(height, width, filt.raw_vals, filt_fft.raw_vals, FFTW_FORWARD, FFTW_MEASURE);

    for (int f = 0; f < 16; f++){

        // initialize a temporary filter
        struct image_s temp_filt = init_gabor_filter(bank.freqs[f], bank.angles[f], bank.sigmas[f], height, width);

        // Load it in
        for (unsigned int i = 0; i < width*height; i++){
            filt.raw_vals[i] = temp_filt.raw_vals[i];
        }

        // Shift the filter
        shift_filter(filt);

        // Execute the plan
        fftw_execute(filt_plan);

        // add the filter to the image response
        for (unsigned int i = 0; i < width*height; i++){
            img.raw_vals[i] += filt_fft.raw_vals[i];
        }

        // Free the temporary filter
        free_image(temp_filt);

    }

    shift_filter(img);
    save_image(img, "coverage");

    free_image(img);
    free_image(filt);
    free_image(filt_fft);

}
void simulate_gabor(struct image_s img){

    // Build a structure for input and output
    struct image_s img_filtered[16];
    struct image_s gabor_filter;
    struct image_s img_reconstruct;

    // Allocate the output
    for (int i = 0; i < 16; i++){
        img_filtered[i] = init_image_empty(img.height, img.width);
    }
    img_reconstruct = init_image_empty(img.height, img.width);

    int i = 0; // Loop counter
    const double k = (1.0/3.0)*sqrt(PI/log(2)); // relationship between frequency and gaussian width

    // Apply each filter
    for (int angle = 0; angle < 4; angle++){
        for (int freq = 0; freq < 4; freq++){

            printf("%d\n", i);

            // Compute the frequency term
            double f = 1.0/pow(2.0, (double)freq + 2.0);

            // Compute the gaussian width term
            double alpha = k * f;

            // Build the filter
            gabor_filter = init_gabor_filter(f, (PI/4.0)*angle, alpha, img.height, img.width);

            // Apply the filter
            convolve_frequency(img, img_filtered[i], gabor_filter);

            i++;

        }
    }

    // Begin the image reconstruction
    for (int angle = 0; angle < 4; angle++){
        for (int freq = 0; freq < 4; freq++){

            int filt_num = angle*4 + freq;

            // Compute the frequency term
            double f = 1.0/pow(2.0, (double)freq + 2.0);

            // Compute the gaussian width term
            double alpha = k * f;

            // Compute a realistic filter size
            int filt_width = 20*(freq+1) + 1;
            int filt_height = 20*(freq+1) + 1;

            // Build the filter
            gabor_filter = init_gabor_filter(f, (PI/4.0)*angle, alpha, filt_height, filt_width);

            // Find the center pixel
            int center_x = gabor_filter.width/2;
            int center_y = gabor_filter.height/2;

            // iterate over each image pixel
            for (unsigned int i = 0; i < img_reconstruct.height; i++){
                printf("%u\n", i);
                for (unsigned int j = 0; j < img_reconstruct.width; j++){

                    // Iterate over each filter pixel
                    for (unsigned int m = 0; m < gabor_filter.height; m++){
                        for (unsigned int n = 0; n < gabor_filter.width; n++){

                            // add the filter
                            int img_x = ((n - center_x) + j) % img_reconstruct.width;
                            int img_y = ((m - center_y) + i) % img_reconstruct.height;
                            img_reconstruct.vals[img_y][img_x] += creal(gabor_filter.vals[m][n]) * creal(img_filtered[filt_num].vals[i][j]);

                        }
                    }

                }
            }

        }
    }


    save_image(img_reconstruct, "recon");

    for (int i = 0; i < 16; i++){
        free_image(img_filtered[i]);
    }
    free_image(gabor_filter);
    free_image(img_reconstruct);

}

