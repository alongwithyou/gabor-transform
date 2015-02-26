#ifndef filter_h
#define filter_h

#include "types.h"

struct filter_s init_filter_empty(const unsigned int height, const unsigned int width);

struct filter_s init_filter_gaussian(const unsigned int height, const unsigned int width, const double sigma);

void free_filter(struct filter_s filt);


#endif
