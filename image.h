#ifndef image_h
#define image_h

#include <complex.h>
#include <math.h>

struct image_s{
    double complex* raw_vals;
    double complex** vals;
    unsigned int width;
    unsigned int height;
};

struct image_s init_image_empty(unsigned int height, unsigned int width);

struct image_s init_image_path(char* filepath);


void free_image(struct image_s img);

void save_image(struct image_s img, char* prefix);

#endif
