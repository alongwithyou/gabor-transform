#include "types.h"
#include "image.h"
#include "gabor.h"
#include "filter.h"
#include "convolve.h"
#include "bilateral.h"

#include <stdio.h>
#include <stdlib.h>
#include <FreeImage.h>
#include <complex.h>
#include <fftw3.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char* argv[]){

    // Path to process
    char path[] = "/home/glenn/documents/schoolwork/grad/thesis/src/texture_grabber/database/";

    // Initialize the image IO library
    FreeImage_Initialise(FALSE);

    // Structures for Gabor Transform
    struct image_s img;
    struct gabor_filter_bank_s bank;
    struct gabor_responses_s resps;

    // Structures for directory parsing
    struct dirent *entry;
    DIR *dp;

    // Open the directory
    dp = opendir(path);
    if (dp == NULL) {
        exit(EXIT_FAILURE);
    }

    bank = init_gabor_filter_bank_exhaustive(800, 800);
    //bank = init_gabor_filter_bank_default(800, 800);
    //disp_gabor_filter_bank(bank, "aaa");

    // Process each file in the directory
    while((entry = readdir(dp))){
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){

            printf("%s\n",(entry->d_name));

            char image_path[300];
            image_path[0] = '\0';

            strncat(image_path, path, 300);
            strncat(image_path, entry->d_name, 300);

            img = init_image_from_path(image_path);
            //img = init_image_from_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/lena_bw.tif");

            resps = apply_gabor_filter_bank(img, bank);

            save_gabor_responses(resps, entry->d_name);

            free_gabor_responses(resps);
            free_image(img);

        }
    }

    free_gabor_filter_bank(bank);

    closedir(dp);

    fftw_cleanup();
    FreeImage_DeInitialise();

}
/*
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
    //img_gaussian = init_image_from_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/lena_ft_gaussian.tif");

    struct filter_s gauss = init_filter_gaussian(512, 512, 2.5);

    // Filter the input image
    img_bilateral = bilateral_filter(img, 5, 40);
    img_gaussian = init_image_empty(512, 512);
    convolve_frequency(img, img_gaussian, gauss);

    // Initialize the filter bank
    bank = init_gabor_filter_bank_default(img.height, img.width);

    // Apply the filter bank to the image
    resps = apply_gabor_filter_bank(img, bank);
    resps_gaussian = apply_gabor_filter_bank(img_gaussian, bank);
    resps_bilateral = apply_gabor_filter_bank(img_bilateral, bank);

    // Write i random responses to the file
    unsigned int c = 3;
    for (unsigned int i = 0; i < 2000; i++){
        unsigned int h = ((double)rand()/(double)RAND_MAX)*(resps.channels[0].height - 60) + 30;
        unsigned int w = ((double)rand()/(double)RAND_MAX)*(resps.channels[0].width - 60) + 30;
        fprintf(fid, "%u,%u,", h,w);
        fprintf(fid, "%f,%f,%f\n",cabs(resps.channels[c].vals[h][w]), cabs(resps_gaussian.channels[c].vals[h][w]), cabs(resps_bilateral.channels[c].vals[h][w]));
    }

    fclose(fid);

    // Rebuild the original image from response channels
    // img_reconstruct = reconstruct_image_from_responses(resps);

    // Display the reconstructed image
    save_image_noscale(img_gaussian, "gaussian");
    save_image_noscale(img_bilateral, "bilateral");
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

*/
