
#include <math.h>
#include <stdio.h>

#include "bilateral.h"
#include "types.h"
#include "image.h"

struct image_s bilateral_filter(struct image_s img, double sigma_spatial, double sigma_range){

    // Initialize the output image
    struct image_s img_out = init_image_empty(img.height, img.width);

    // Compute the filter size
    unsigned int filt_height = 6*sigma_spatial+1;
    unsigned int filt_width = 6*sigma_spatial+1;
    // Find the center pixel of the filter
    int center_x = filt_width/2;
    int center_y = filt_height/2;

    // iterate over each image pixel
    for (unsigned int i = 0; i < img.height; i++){
        printf("%d\n", i);
        for (unsigned int j = 0; j < img.width; j++){

            // Normalization term for the filter
            double filt_weight = 0;

            // Iterate over each filter pixel
            for (unsigned int m = 0; m < filt_height; m++){
                for (unsigned int n = 0; n < filt_width; n++){

                    // Image pixel "under" filter pixel (can be outside image size!)
                    int img_y = ((m - center_y) + i) % img.height;
                    int img_x = ((n - center_x) + j) % img.width;

                    // Compute the spatial difference (euclidian distance)
                    double spatial_diff = sqrt(pow((img_x - j),2) + pow((img_y - i),2));

                    // "wrap" pixel values into the image size
                    img_x %= img.width;
                    img_y %= img.height;

                    // Compute the range difference (digital count difference)
                    double range_diff = cabs(img.vals[img_y][img_x]) - cabs(img.vals[i][j]);

                    // Compute the multiplication of both gaussians
                    double filt_val = exp(-1.0 * pow(spatial_diff,2) / (2*pow(sigma_spatial, 2))) * exp(-1.0 * pow(range_diff,2) / (2*pow(sigma_range, 2)));

                    // Accumulate the filter sum
                    filt_weight += filt_val;

                    // Convolve
                    img_out.vals[i][j] += img.vals[img_y][img_x] * filt_val;

                }
            }

            // Normalize by the filter sum
            img_out.vals[i][j] /= filt_weight;

        }
    }

    return img_out;

}
