#ifndef bilateral_h
#define bilateral_h

#include "types.h"

void bilateral_filter(struct image_s img_in, struct image_s img_out, double sigma_spatial, double sigma_range);

#endif
