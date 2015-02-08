#include "image.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <complex.h>
#include <float.h>
#include <FreeImage.h>
#include <fftw3.h>

struct image_s init_image_path(char* filepath){

    struct image_s img;

    FIBITMAP* freeimg;
    FIBITMAP* grayimg;
    FIBITMAP* compleximg;

    // Find the image format from file
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    fif = FreeImage_GetFileType(filepath, 0);

    // If it failed, find it from the filename
    if(fif == FIF_UNKNOWN) {
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
    FreeImage_Unload(freeimg);
    freeimg = NULL;
    compleximg = FreeImage_ConvertToType(grayimg, FIT_COMPLEX, TRUE);
    FreeImage_Unload(grayimg);
    grayimg = NULL;

    // Initialize the image
    img = init_image_empty(FreeImage_GetWidth(compleximg), FreeImage_GetHeight(compleximg));

    // Copy values into new image array
    for (unsigned int i = 0; i < img.height; i++){
        double complex* line = (double complex*)FreeImage_GetScanLine(compleximg, i);
        for (unsigned int j = 0; j < img.width; j++){
            img.vals[i][j] = line[j];
        }
    }

    return img;
}



struct image_s init_image_empty(unsigned int width, unsigned int height){

    struct image_s img;

    img.width = width;
    img.height = height;

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

    return img;

}



void free_image(struct image_s img){

    fftw_free(img.raw_vals);
    free(img.vals);
    img.raw_vals = NULL;
    img.vals = NULL;

}



void save_image(struct image_s img, char* prefix){

    uint8_t* abs_img;
    uint8_t* arg_img;
    uint8_t* real_img;
    uint8_t* imag_img;

    // Allocate the real array
    real_img = (uint8_t*)malloc(img.width*img.height*sizeof(uint8_t));
    if (real_img == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }
    // Allocate the imaginary array
    imag_img = (uint8_t*)malloc(img.width*img.height*sizeof(uint8_t));
    if (imag_img == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }
    // Allocate the magnitude array
    abs_img = (uint8_t*)malloc(img.width*img.height*sizeof(uint8_t));
    if (abs_img == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }
    // Allocate the argument array
    arg_img = (uint8_t*)malloc(img.width*img.height*sizeof(uint8_t));
    if (arg_img == NULL){
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }

    double abs_min = DBL_MAX;
    double abs_max = DBL_MIN;
    double arg_min = DBL_MAX;
    double arg_max = DBL_MIN;
    double real_min = DBL_MAX;
    double real_max = DBL_MIN;
    double imag_min = DBL_MAX;
    double imag_max = DBL_MIN;

    // Find the minimum and maximum of each component
    for (unsigned int i = 0; i < (img.height*img.width); i++){
        if (cabs(img.raw_vals[i]) < abs_min){
            abs_min = cabs(img.raw_vals[i]);
        }
        if (cabs(img.raw_vals[i]) > abs_max){
            abs_max = cabs(img.raw_vals[i]);
        }
        if (carg(img.raw_vals[i]) < arg_min){
            arg_min = carg(img.raw_vals[i]);
        }
        if (carg(img.raw_vals[i]) > arg_max){
            arg_max = carg(img.raw_vals[i]);
        }
        if (creal(img.raw_vals[i]) < real_min){
            real_min = creal(img.raw_vals[i]);
        }
        if (creal(img.raw_vals[i]) > real_max){
            real_max = creal(img.raw_vals[i]);
        }
        if (cimag(img.raw_vals[i]) < imag_min){
            imag_min = cimag(img.raw_vals[i]);
        }
        if (cimag(img.raw_vals[i]) > imag_max){
            imag_max = cimag(img.raw_vals[i]);
        }
    }

    printf("abs: %f, %f\n", abs_min, abs_max);
    printf("arg: %f, %f\n", arg_min, arg_max);
    printf("real: %f, %f\n", real_min, real_max);
    printf("imag: %f, %f\n", imag_min, imag_max);


    // Copy the real and imaginary values in
    for (unsigned int i = 0; i < (img.height*img.width); i++){
        abs_img[i] = ((cabs(img.raw_vals[i]) - abs_min) / (abs_max - abs_min))*255;
        arg_img[i] = ((carg(img.raw_vals[i]) - arg_min) / (arg_max - arg_min))*255;
        real_img[i] = ((creal(img.raw_vals[i]) - real_min) / (real_max - real_min))*255;
        imag_img[i] = ((cimag(img.raw_vals[i]) - imag_min) / (imag_max - imag_min))*255;
    }

    FIBITMAP *abs_freeimg = FreeImage_ConvertFromRawBits(abs_img, img.width, img.height, img.width, 8, 0, 0, 0, FALSE);
    FIBITMAP *arg_freeimg = FreeImage_ConvertFromRawBits(arg_img, img.width, img.height, img.width, 8, 0, 0, 0, FALSE);
    FIBITMAP *real_freeimg = FreeImage_ConvertFromRawBits(real_img, img.width, img.height, img.width, 8, 0, 0, 0, FALSE);
    FIBITMAP *imag_freeimg = FreeImage_ConvertFromRawBits(imag_img, img.width, img.height, img.width, 8, 0, 0, 0, FALSE);

    char pathname[200];

    snprintf(pathname, 200, "%s_abs.png", prefix);
    FreeImage_Save(FIF_PNG, abs_freeimg, pathname, 0);
    snprintf(pathname, 200, "%s_arg.png", prefix);
    FreeImage_Save(FIF_PNG, arg_freeimg, pathname, 0);
    snprintf(pathname, 200, "%s_real.png", prefix);
    FreeImage_Save(FIF_PNG, real_freeimg, pathname, 0);
    snprintf(pathname, 200, "%s_imag.png", prefix);
    FreeImage_Save(FIF_PNG, imag_freeimg, pathname, 0);

    FreeImage_Unload(abs_freeimg);
    FreeImage_Unload(arg_freeimg);
    FreeImage_Unload(real_freeimg);
    FreeImage_Unload(imag_freeimg);
    free(abs_img);
    free(arg_img);
    free(real_img);
    free(imag_img);
    abs_img = NULL;
    arg_img = NULL;
    real_img = NULL;
    imag_img = NULL;

}


