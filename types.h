#ifndef types_h
#define types_h

#include <complex.h>

struct image_s{
    double complex* raw_vals;
    double complex** vals;
    unsigned int width;
    unsigned int height;
};

struct filter_s{
    double complex* raw_vals;
    double complex** vals;
    unsigned int width;
    unsigned int height;
};

struct gabor_filter_bank_s{
    double* angles;
    double* sigmas;
    double* freqs;
    unsigned int height;
    unsigned int width;
    unsigned int num_filters;
};

struct gabor_responses_s{
    struct image_s* channels;
    unsigned int num_channels;
};


#endif
