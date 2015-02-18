#ifndef util_h
#define util_h

#include "types.h"

void iterative_reconstruction(struct gabor_responses_s resps, struct gabor_filter_bank_s bank);

void compute_histogram(struct gabor_responses_s resps, unsigned int num_bins, unsigned int* bins, double* bin_borders);

#endif
