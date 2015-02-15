
#include "types.h"
#include "image.h"
#include "gabor.h"
#include "convolve.h"

#include <stdio.h>
#include <FreeImage.h>


int main(int argc, char* argv[]){

    // Initialize the image I/O library
    FreeImage_Initialise(FALSE);

    struct image_s img;
    struct image_s img_reconstruct;
    struct gabor_filter_bank_s bank;
    struct gabor_responses_s resps;

    // Read in the image
    img = init_image_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/deadleaves_2048.tif");
    //img = init_image_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/lena_bw.tif");

    // Initialize the filter bank
    bank = init_gabor_filter_bank_default(img.height, img.width);

    // Apply the filter bank to the image
    resps = apply_gabor_filter_bank(img, bank);

    img_reconstruct = reconstruct_image_from_responses(resps);

    // Display one of the response channels
    save_image(img_reconstruct, "aaa");

    // Free memory
    free_image(img);
    free_image(img_reconstruct);
    free_gabor_filter_bank(bank);
    free_gabor_responses(resps);

    // Cleanup
    cleanup_fftw();
    FreeImage_DeInitialise();

    return 0;

}
