
#include "gabor.h"
#include "convolve.h"
#include "image.h"

#include <stdio.h>
#include <FreeImage.h>



int main(int argc, char* argv[]){

    // Initialize the image I/O library
    FreeImage_Initialise(FALSE);

    struct image_s img;
    struct gabor_filter_bank_s bank = init_gabor_filter_bank_default(512, 512);
    struct image_s filter;

    // Read in the image
    //img = init_image_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/deadleaves_2048.tif");
    img = init_image_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/lena_bw.tif");



    //disp_gabor_filter_bank(bank);

    //simulate_gabor(img);

    free_image(img);
    //free_image(filter);
    free_gabor_filter_bank(bank);

    cleanup_fftw();

    FreeImage_DeInitialise();

    return 0;

}
