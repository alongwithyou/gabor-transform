#ifndef gabor_h
#define gabor_h

#include "image.h"

struct gabor_filter_bank_s{

    double* angles;
    double* sigmas;
    double* freqs;
    int height;
    int width;

};

struct gabor_responses_s{

    struct image_s* channels;

};

struct gabor_filter_bank_s init_gabor_filter_bank_default(int height, int width);

void free_gabor_filter_bank(struct gabor_filter_bank_s bank);

void disp_gabor_filter_bank(struct gabor_filter_bank_s bank);

struct image_s init_gabor_filter_from_params(double freq, double angle, double sigma, int filt_height, int filt_width);

struct image_s init_gabor_filter_from_bank(struct gabor_filter_bank_s bank, int filter_num);

void simulate_gabor(struct image_s img);

#endif
