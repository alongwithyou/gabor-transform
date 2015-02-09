#ifndef convolve_h
#define convolve_h

#include "image.h"

void shift_filter(struct image_s filt);

void convolve_frequency(struct image_s img_in, struct image_s img_out, struct image_s filt);

void cleanup_fftw();

void convolve_spatial(struct image_s img_in, struct image_s img_out, struct image_s filt);




#endif
