#ifndef bilateral_h
#define bilateral_h

#include "types.h"


struct image_s bilateral_filter(struct image_s img, double sigma_spatial, double sigma_range);


#endif
