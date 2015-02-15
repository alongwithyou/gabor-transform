#ifndef filter_h
#define filter_h

#include "types.h"

struct filter_s init_filter_empty(const unsigned int width, const unsigned int height);

void free_filter(struct filter_s filt);


#endif
