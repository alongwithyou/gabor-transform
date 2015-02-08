#ifndef gabor_h
#define gabor_h

#include "image.h"

struct gabor_filter_bank_s{

    struct image_s filters[32];
    double freq[32];
    double angle[32];
    double alpha[32];

};

struct gabor_filter_bank_s init_gabor_filter_bank(int filt_height, int filt_width);

void free_gabor_filter_bank(struct gabor_filter_bank_s filt_bank);

struct image_s init_gabor_filter(double freq, double angle, double sigma, int filt_height, int filt_width);

void display_filter_bank(struct gabor_filter_bank_s bank);

#endif
