
#include "gabor.h"
#include "convolve.h"
#include "image.h"

#include <stdio.h>
#include <FreeImage.h>



int main(int argc, char* argv[]){

    // Initialize the image I/O library
    FreeImage_Initialise(FALSE);

    struct image_s img;

    // Read in the image
    //img = init_image_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/deadleaves_2048.tif");
    img = init_image_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/lena_bw.tif");

    struct gabor_filter_bank_s bank = init_gabor_filter_bank_default();

    disp_gabor_filter_bank(bank);

    struct image_s filter;

    char output[100];

    for (int i = 0; i < 16; i++){

        filter = init_gabor_filter(bank.freqs[i], bank.angles[i], bank.sigmas[i], 100, 100);

        snprintf(output, 100, "%d", i);

        save_image(filter, output);

        printf("%f, %f, %f\n", bank.freqs[i], bank.angles[i], bank.sigmas[i]);

    }

    //simulate_gabor(img);


    free_image(img);
    free_image(filter);
    free_gabor_filter_bank(bank);

    cleanup_fftw();

    FreeImage_DeInitialise();

    return 0;

}
