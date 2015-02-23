#include "image.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <complex.h>
#include <float.h>
#include <FreeImage.h>
#include <fftw3.h>

#define PI 3.1415926535897932384

struct image_s init_image_from_path(const char* const filepath){

    // Define structures for reading the image
    struct image_s img;
    FIBITMAP* freeimg;
    FIBITMAP* grayimg;
    FIBITMAP* compimg;

    // Find the image format from file
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    fif = FreeImage_GetFileType(filepath, 0);

    // If it failed, find it from the filename
    if(fif == FIF_UNKNOWN) {
        printf("falling back to filename for image format identification");
        fif = FreeImage_GetFIFFromFilename(filepath);
    }

    // If the image is readable, read it
    if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
        freeimg = FreeImage_Load(fif, filepath, 0);
    }
    else{
        fprintf(stderr, "Image read failed\n");
        exit(EXIT_FAILURE);
    }

    // Make sure there was no error
    if (freeimg == NULL){
        fprintf(stderr, "Image read failed\n");
        exit(EXIT_FAILURE);
    }

    // Convert the image to grayscale, double complex.
    grayimg = FreeImage_ConvertToGreyscale(freeimg);
    compimg = FreeImage_ConvertToType(grayimg, FIT_COMPLEX, TRUE);

    // Initialize the image structure
    img = init_image_empty(FreeImage_GetWidth(freeimg), FreeImage_GetHeight(freeimg));

    // Copy values into new image array
    for (unsigned int i = 0; i < img.height; i++){
        double complex* line = (double complex*)FreeImage_GetScanLine(compimg, i);
        for (unsigned int j = 0; j < img.width; j++){
            img.vals[i][j] = line[j];
        }
    }

    // Free the image
    FreeImage_Unload(freeimg);
    FreeImage_Unload(grayimg);
    FreeImage_Unload(compimg);

    return img;
}



struct image_s init_image_empty(const unsigned int height, const unsigned int width){

    // Create the structure
    struct image_s img;

    // Set height and width
    img.height = height;
    img.width = width;

    // Allocate the filter array
    img.raw_vals = (double complex*)fftw_malloc(width*height*sizeof(double complex));
    if (img.raw_vals == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }

    // Make an array of pointers into each row for 2d indexing
    img.vals = (double complex**)malloc(height*sizeof(double complex*));
    if (img.vals == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }
    for (unsigned int i = 0; i < height; i++){
        img.vals[i] = img.raw_vals + img.width*i;
    }


    // Zero the image!!!
    for (unsigned int i = 0; i < height*width; i++){

        img.raw_vals[i] = 0;

    }

    return img;

}



void free_image(struct image_s img){

    fftw_free(img.raw_vals);
    free(img.vals);
    img.raw_vals = NULL;
    img.vals = NULL;

}





void save_image_scale(struct image_s img, const char* const prefix, double min_val, double max_val){

    uint8_t* out_img;

    // Allocate the real array
    out_img = (uint8_t*)malloc(img.width*img.height*sizeof(uint8_t));
    if (out_img == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }

    // Copy the real and imaginary values in, scaling to fit in 8 bits
    for (unsigned int i = 0; i < (img.height*img.width); i++){
        out_img[i] = ((cabs(img.raw_vals[i]) - min_val) / (max_val - min_val))*255;
    }

    // Create the FreeImage for writing
    FIBITMAP *out_freeimg = FreeImage_ConvertFromRawBits(out_img, img.width, img.height, img.width, 8, 0, 0, 0, FALSE);

    // Write the files
    char pathname[200];
    snprintf(pathname, 200, "%s.png", prefix);
    FreeImage_Save(FIF_PNG, out_freeimg, pathname, 0);

    FreeImage_Unload(out_freeimg);
    free(out_img);
    out_img = NULL;
}


void save_image_noscale(struct image_s img, const char* const prefix){

    // Save with no value scaling
    save_image_scale(img, prefix, 0, 255);

}


void save_image_autoscale(struct image_s img, const char* const prefix){

    double img_min = DBL_MAX;
    double img_max = DBL_MIN;

    // Find the minimum and maximum of each component
    for (unsigned int i = 0; i < (img.height*img.width); i++){
        if (cabs(img.raw_vals[i]) < img_min){
            img_min = cabs(img.raw_vals[i]);
        }
        if (cabs(img.raw_vals[i]) > img_max){
            img_max = cabs(img.raw_vals[i]);
        }
    }

    // Save the image
    save_image_scale(img, prefix, img_min, img_max);

}
