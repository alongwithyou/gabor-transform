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
    srand(2);

    struct image_s img;
    struct image_s img_filt;
    //struct gabor_filter_bank_s bank;
    //struct gabor_responses_s resps;

    // Read in the image
    //img = init_image_from_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/deadleaves_2048.tif");
    img = init_image_from_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/lena_bw.tif");

    // Initialize the filter bank
    // bank = init_gabor_filter_bank_default(img.height, img.width);

    // Apply the filter bank to the image
    // resps = apply_gabor_filter_bank(img, bank);

    // Rebuild the original image from response channels
    // img_reconstruct = reconstruct_image_from_responses(resps);

    // Filter the input image
    img_filt = bilateral_filter(img, 4, 40);

    // Display the reconstructed image
    save_image_noscale(img_filt, "aaa");

    // Free memory
    free_image(img_filt);
    free_image(img);
    //free_gabor_filter_bank(bank);
    //free_gabor_responses(resps);
    // Cleanup
    fftw_cleanup();
    FreeImage_DeInitialise();

    return 0;

}
