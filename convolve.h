#ifndef convolve_h
#define convolve_h

#include "types.h"

void shift_filter(struct filter_s filt);

void convolve_frequency(const struct image_s img_in, struct image_s img_out, const struct filter_s filt);

void cleanup_fftw();

void convolve_spatial(struct image_s img_in, struct image_s img_out, struct filter_s filt);

#endif
