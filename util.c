

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

    // How many filters to use for reconstruction
    for (unsigned int n = 0; n < 1000; n++){

        unsigned int channel = ((double)rand()/(double)RAND_MAX)*16;
        unsigned int row = ((double)rand()/(double)RAND_MAX)*height;
        unsigned int col = ((double)rand()/(double)RAND_MAX)*width;

    //    double max_val = 0;
    //    unsigned int channel = 0;
    //    unsigned int row = 0;
    //    unsigned int col = 0;

    //    // Find the max value
    //    for (unsigned int c = 0; c < resps.num_channels; c++){
    //        for (unsigned int i = 0; i < height; i++){
    //            for (unsigned int j = 0; j < width; j++){
    //                if (abs(creal(resps.channels[c].vals[i][j])) > abs(max_val)){
    //                    max_val = creal(resps.channels[c].vals[i][j]);
    //                    channel = c;
    //                    row = i;
    //                    col = j;

    //                    // Set the location to zero (destructive!!!!!!!!!!)
    //                    resps.channels[c].vals[i][j] = 0;
    //                }
    //            }
    //        }
    //    }
    //

        printf("%f, %d, %d, %d\n", creal(resps.channels[channel].vals[row][col]), channel, row, col);

        // Build the proper filter
        struct filter_s filt = init_gabor_filter_from_params(bank.freqs[channel], bank.angles[channel], bank.sigmas[channel], height, width);

        // Iterate over each filter pixel
        for (unsigned int i = 0; i < height; i++){
            for (unsigned int j = 0; j < width; j++){

                // add the filter
                int img_y = ((i - center_y) + row) % height;
                int img_x = ((j - center_x) + col) % width;
                img.vals[img_y][img_x] += creal(filt.vals[i][j]) * creal(resps.channels[channel].vals[row][col]);

            }
        }

    }

    save_image(img, "test");

    free_image(img);


}




void print_histograms(struct gabor_responses_s resps){

    const int num_bins = 20;


    unsigned int height = resps.channels[0].height;
    unsigned int width = resps.channels[0].width;


    // Find the extrema
    //for (unsigned int c = 0; c < resps.num_channels; c++){
    for (unsigned int c = 0; c < 16; c++){
        double bin_borders[num_bins+1];
        unsigned int bins[num_bins];

        for (int i = 0; i < num_bins; i++){
            bins[i] = 0;
        }

        double max_val = DBL_MIN;
        double min_val = DBL_MAX;

        for (unsigned int i = 0; i < height*width; i++){
            if (creal(resps.channels[c].raw_vals[i]) > max_val){
                max_val = creal(resps.channels[c].raw_vals[i]);
            }
            if (creal(resps.channels[c].raw_vals[i]) < min_val){
                min_val = creal(resps.channels[c].raw_vals[i]);
            }
        }

        // Make the bin borders
        for (unsigned int i = 0; i < num_bins+1; i++){

            bin_borders[i] = min_val + i*(max_val-min_val)/num_bins;

            printf("%.2f,", bin_borders[i]);

        }

        printf("\n");

        // Process each point into the histogram
        for (unsigned int i = 0; i < height*width; i++){

            // Find the bin
            unsigned int bin = 0;
            while (creal(resps.channels[c].raw_vals[i]) > bin_borders[bin]){
                bin++;
            }

            bins[bin]++;

        }

        for (unsigned int i = 0; i < num_bins+1; i++){
            printf("%d,", bins[i]);
        }
        printf("\n");




    }


}



