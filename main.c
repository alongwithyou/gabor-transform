
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
    struct image_s img_filtered[16];
    struct image_s gabor_filter;
    struct image_s img_reconstruct;

    // Read in the image
    //img = init_image_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/deadleaves_2048.tif");
    img = init_image_path("/home/glenn/documents/schoolwork/grad/thesis/imgs/lena_bw.tif");

    // Allocate the output
    for (int i = 0; i < 16; i++){
        img_filtered[i] = init_image_empty(img.height, img.width);
    }
    img_reconstruct = init_image_empty(img.height, img.width);

    int i = 0; // Loop counter
    const double k = (1.0/3.0)*sqrt(PI/log(2)); // relationship between frequency and gaussian width

    // Apply each filter
    for (int angle = 0; angle < 4; angle++){
        for (int freq = 0; freq < 4; freq++){

            printf("%d\n", i);

            // Compute the frequency term
            double f = 1.0/pow(2.0, (double)freq + 2.0);

            // Compute the gaussian width term
            double alpha = k * f;

            // Build the filter
            gabor_filter = init_gabor_filter(f, (PI/4.0)*angle, alpha, img.height, img.width);

            // Apply the filter
            convolve_frequency(img, img_filtered[i], gabor_filter);

            i++;

        }
    }

    // Begin the image reconstruction
    for (int angle = 0; angle < 4; angle++){
        for (int freq = 0; freq < 4; freq++){

            int filt_num = angle*4 + freq;

            // Compute the frequency term
            double f = 1.0/pow(2.0, (double)freq + 2.0);

            // Compute the gaussian width term
            double alpha = k * f;

            // Compute a realistic filter size
            int filt_width = 20*(freq+1) + 1;
            int filt_height = 20*(freq+1) + 1;

            // Build the filter
            gabor_filter = init_gabor_filter(f, (PI/4.0)*angle, alpha, filt_height, filt_width);

            // Find the center pixel
            int center_x = gabor_filter.width/2;
            int center_y = gabor_filter.height/2;

            // iterate over each image pixel
            for (unsigned int i = 0; i < img_reconstruct.height; i++){
                printf("%u\n", i);
                for (unsigned int j = 0; j < img_reconstruct.width; j++){

                    // Iterate over each filter pixel
                    for (unsigned int m = 0; m < gabor_filter.height; m++){
                        for (unsigned int n = 0; n < gabor_filter.width; n++){

                            // add the filter
                            int img_x = ((n - center_x) + j) % img_reconstruct.width;
                            int img_y = ((m - center_y) + i) % img_reconstruct.height;
                            img_reconstruct.vals[img_y][img_x] += creal(gabor_filter.vals[m][n]) * creal(img_filtered[filt_num].vals[i][j]);

                        }
                    }

                }
            }

        }
    }


    save_image(img_reconstruct, "recon");

    cleanup_fftw();

    free_image(img);
    for (int i = 0; i < 16; i++){
        free_image(img_filtered[i]);
    }
    free_image(gabor_filter);

    FreeImage_DeInitialise();

    return 0;

}
