

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <complex.h>



#include "util.h"
#include "types.h"
#include "image.h"
#include "gabor.h"



void iterative_reconstruction(struct gabor_responses_s resps, struct gabor_filter_bank_s bank){

    unsigned int height = resps.channels[0].height;
    unsigned int width = resps.channels[0].width;
    int center_y = height/2;
    int center_x = width/2;

    struct image_s img = init_image_empty(height, width);

    save_image(img, "bbb");

    // Compute the histogram of the responses
    unsigned int num_bins = 100;
    unsigned int bins[num_bins];
    double bin_borders[num_bins+1];
    compute_histogram(resps, num_bins, bins, bin_borders);

    // Find the 5% threshold
    unsigned int n_resps = resps.num_channels * height * width;
    unsigned int i = num_bins-1;
    unsigned int sum = bins[i];
    while (sum < 0.005*n_resps){
        i--;
        sum += bins[i];
    }
    double thresh = bin_borders[i];


    // Set everything below the thresh to zero
    for (unsigned int c = 0; c < resps.num_channels; c++){
        for (unsigned int i = 0; i < height*width; i++){
            if (abs(creal(resps.channels[c].raw_vals[i])) < thresh){
                resps.channels[c].raw_vals[i] = 0.0;
            }
        }
    }

    // How many filters to use for reconstruction
    for (unsigned int n = 0; n < 0.0025*n_resps; n++){

        // Generate a starting point
        unsigned int channel = ((double)rand()/(double)RAND_MAX)*16;
        unsigned int row = ((double)rand()/(double)RAND_MAX)*height;
        unsigned int col = ((double)rand()/(double)RAND_MAX)*width;
        double mag = creal(resps.channels[channel].vals[row][col]);

        // HORRIBLE
        // While it's not a good random point, just pick a new one
        while (cabs(resps.channels[channel].vals[row][col]) < thresh){
            channel = ((double)rand()/(double)RAND_MAX)*16;
            row = ((double)rand()/(double)RAND_MAX)*height;
            col = ((double)rand()/(double)RAND_MAX)*width;
            mag = creal(resps.channels[channel].vals[row][col]);
        }

        // Set this one to zero too to avoid repeating
        resps.channels[channel].vals[row][col] = 0.0;

        printf("%f, %d, %d, %d\n", mag, channel, row, col);

        // Build the proper filter
        struct filter_s filt = init_gabor_filter_from_params(bank.freqs[channel], bank.angles[channel], bank.sigmas[channel], height, width);

        // Iterate over each filter pixel
        for (unsigned int i = 0; i < height; i++){
            for (unsigned int j = 0; j < width; j++){

                // add the filter
                int img_y = ((i - center_y) + row) % height;
                int img_x = ((j - center_x) + col) % width;
                img.vals[img_y][img_x] += creal(filt.vals[i][j]) * mag;

            }
        }

        if (!(n % 10)){
            char imname[100];
            snprintf(imname, 100, "/home/glenn/gif/%05d_frame", n);
            save_image(img, imname);
        }

    }


    free_image(img);


}




void compute_histogram(struct gabor_responses_s resps, unsigned int num_bins, unsigned int* bins, double* bin_borders){

    unsigned int height = resps.channels[0].height;
    unsigned int width = resps.channels[0].width;

    for (int i = 0; i < num_bins; i++){
        bins[i] = 0;
    }

    double max_val = 0;
    double min_val = DBL_MAX;

    // Find the extrema
    for (unsigned int c = 0; c < resps.num_channels; c++){
        for (unsigned int i = 0; i < height*width; i++){
            if (abs(creal(resps.channels[c].raw_vals[i])) > max_val){
                max_val = abs(creal(resps.channels[c].raw_vals[i]));
            }
            if (abs(creal(resps.channels[c].raw_vals[i])) < min_val){
                min_val = abs(creal(resps.channels[c].raw_vals[i]));
            }
        }
    }

    // Make the bin borders
    for (unsigned int i = 0; i < num_bins+1; i++){

        bin_borders[i] = min_val + i*(max_val-min_val)/num_bins;

    }

    // Process each point into the histogram
    for (unsigned int c = 0; c < resps.num_channels; c++){
        for (unsigned int i = 0; i < height*width; i++){

            // Find the bin
            unsigned int bin = 0;
            while (abs(creal(resps.channels[c].raw_vals[i])) > bin_borders[bin]){
                bin++;
            }

            bins[bin]++;

        }
    }

}



