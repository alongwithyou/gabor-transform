#ifndef types_h
#define types_h

#include <complex.h>

struct image_s{
    double complex* raw_vals;
    double complex** vals;
    unsigned int width;
    unsigned int height;
};

struct gabor_filter_s{
    double complex* raw_vals;
    double complex** vals;
    unsigned int width;
    unsigned int height;
};

struct gabor_filter_bank_s{
    double* angles;
    double* sigmas;
    double* freqs;
    int height;
    int width;
    int num_filters;
};

struct gabor_responses_s{
    struct image_s* channels;
    int num_channels;
};


#endif
