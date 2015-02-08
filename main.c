
#include "image.h"
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
    struct image_s gabor_filter;

    // Read in the image
    //img = init_image_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/deadleaves_2048.tif");
    img = init_image_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/lena_bw.tif");

    // Allocate the output
    img_out = init_image_empty(img.height, img.width);

    int i = 0; // Loop counter
    const double k = (1.0/3.0)*sqrt(PI/log(2)); // relationship between frequency and gaussian width

    char output[200];

    // Loop over each frequency and angle
    for (int angle = 0; angle < 8; angle++){
        for (int freq = 0; freq < 4; freq++){

            printf("%d\n", i);

            // Compute the frequency term
            double f = 1.0/pow(2.0, (double)freq + 2.0);

            // Compute the gaussian width term
            double alpha = k * f;

            // Build the filter
            gabor_filter = init_gabor_filter(f, (PI/4.0)*angle + (PI/8.0), alpha, img.height, img.width);

            snprintf(output, 200, "gabor_imgs/%d", i);

            convolve_frequency(img, img_out, gabor_filter);

            save_image(img_out, output);

            i++;

        }
    }

    cleanup_fftw();

    free_image(img);
    free_image(img_out);
    free_image(gabor_filter);

    FreeImage_DeInitialise();

    return 0;

}
