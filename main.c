
#include "image.h"
#include "gabor.h"
#include "convolve.h"

#include <stdio.h>
#include <FreeImage.h>

#define PI 3.1415926535897932384

int main(int argc, char* argv[]){

    // Initialize the image I/O library
    FreeImage_Initialise(FALSE);

    // Build a structure for input and output
    struct image_s img;
    struct image_s img_out;

    // Read in the image
    //img = init_image_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/deadleaves_2048.tif");
    img = init_image_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/lena_bw.tif");

    // Allocate the output
    img_out = init_image_empty(img.height, img.width);

    // Make a filter bank
    struct gabor_filter_bank_s filt_bank = init_gabor_filter_bank(img.height, img.width);


    // Process each filter
    char output[200];
    for (int i = 0; i < 16; i++){

        printf("%d\n", i);

        snprintf(output, 200, "gabor_imgs/%d", i);

        convolve_frequency(img, img_out, filt_bank.filters[i]);

        save_image(img_out, output);

    }

    display_filter_bank(filt_bank);

    free_gabor_filter_bank(filt_bank);

    cleanup_fftw();

    FreeImage_DeInitialise();

    return 0;

}
