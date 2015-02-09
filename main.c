
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

    simulate_gabor(img);


    free_image(img);

    cleanup_fftw();

    FreeImage_DeInitialise();

    return 0;

}
