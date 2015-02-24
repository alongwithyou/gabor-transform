#include "types.h"
#include "image.h"
#include "gabor.h"
#include "filter.h"
#include "convolve.h"
#include "util.h"
#include "bilateral.h"

#include <stdio.h>
#include <stdlib.h>
#include <FreeImage.h>
#include <complex.h>
#include <fftw3.h>

int main(int argc, char* argv[]){

    // Initialize the image I/O library
    FreeImage_Initialise(FALSE);
    srand(1245234);

    struct image_s img;
    struct image_s img_bilateral;
    struct image_s img_gaussian;
    struct gabor_filter_bank_s bank;
    struct gabor_responses_s resps;
    struct gabor_responses_s resps_bilateral;
    struct gabor_responses_s resps_gaussian;

    FILE* fid = fopen("resps.csv", "w");

    // Read in the image
    //img = init_image_from_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/deadleaves_2048.tif");
    img = init_image_from_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/lena_bw.tif");

    // Filter the input image
    img_bilateral = bilateral_filter(img, 5, 40);
    img_gaussian = bilateral_filter(img, 2, 10000);

    // Initialize the filter bank
    bank = init_gabor_filter_bank_default(img.height, img.width);

    // Apply the filter bank to the image
    resps = apply_gabor_filter_bank(img, bank);
    resps_gaussian = apply_gabor_filter_bank(img_gaussian, bank);
    resps_bilateral = apply_gabor_filter_bank(img_bilateral, bank);

    unsigned int c = 5;
        // Write i random responses to the file
    for (unsigned int i = 0; i < 2000; i++){
        unsigned int h = ((double)rand()/(double)RAND_MAX)*(resps.channels[0].height - 60) + 30;
        unsigned int w = ((double)rand()/(double)RAND_MAX)*(resps.channels[0].width - 60) + 30;
        fprintf(fid, "%u,%u,", h,w);
        for (unsigned int c = 0; c < resps.num_channels; c++){
            // unsigned int c = ((double)rand()/(double)RAND_MAX)*resps.num_channels;
            fprintf(fid, "%f,%f,%f,",cabs(resps.channels[c].vals[h][w]), cabs(resps_gaussian.channels[c].vals[h][w]), cabs(resps_bilateral.channels[c].vals[h][w]));
        }
        fprintf(fid,"\n");
    }

    fclose(fid);


    struct image_s channel_disp = init_image_empty(resps.channels[0].height, resps.channels[0].width*3);

    for (unsigned int i = 0; i < resps.channels[0].height; i++){
        for (unsigned int j = 0; j < resps.channels[0].width; j++){
            channel_disp.vals[i][j] = cabs(resps.channels[c].vals[i][j]);
        }
    }
    for (unsigned int i = 0; i < resps.channels[0].height; i++){
        for (unsigned int j = 0; j < resps.channels[0].width; j++){
            channel_disp.vals[i][j+resps.channels[0].width] = cabs(resps_gaussian.channels[c].vals[i][j]);
        }
    }
    for (unsigned int i = 0; i < resps.channels[0].height; i++){
        for (unsigned int j = 0; j < resps.channels[0].width; j++){
            channel_disp.vals[i][j+resps.channels[0].width*2] = cabs(resps_bilateral.channels[c].vals[i][j]);
        }
    }

    // Rebuild the original image from response channels
    // img_reconstruct = reconstruct_image_from_responses(resps);

    // Display the reconstructed image
    save_image_noscale(img_gaussian, "gaussian");
    save_image_noscale(img_bilateral, "bilateral");
    save_image_autoscale(channel_disp, "aaa");
    save_image_autoscale(resps.channels[c], "bbb");
    save_image_autoscale(resps_gaussian.channels[c], "ccc");
    save_image_autoscale(resps_bilateral.channels[c], "ddd");

    // Free memory
    free_image(img_bilateral);
    free_image(img_gaussian);
    free_image(img);
    free_gabor_filter_bank(bank);
    free_gabor_responses(resps);
    free_gabor_responses(resps_bilateral);
    free_gabor_responses(resps_gaussian);
    // Cleanup
    fftw_cleanup();
    FreeImage_DeInitialise();

    return 0;

}
