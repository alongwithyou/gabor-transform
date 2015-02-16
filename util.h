#ifndef util_h
#define util_h

#include "types.h"

void iterative_reconstruction(struct gabor_responses_s resps, struct gabor_filter_bank_s bank);

void print_histograms(struct gabor_responses_s resps);

#endif
