#ifndef gabor_h
#define gabor_h

#include "types.h"

struct gabor_filter_bank_s init_gabor_filter_bank_default(const unsigned int height, const unsigned int width);

struct gabor_responses_s init_gabor_responses_empty(const unsigned int height, const unsigned int width, const unsigned int num_filters);

struct gabor_responses_s apply_gabor_filter_bank(struct image_s img, struct gabor_filter_bank_s bank);

struct filter_s init_gabor_filter_from_params(const double freq, const double angle, const double sigma, const unsigned int filt_height, const unsigned int filt_width);
struct filter_s init_gabor_filter_from_bank(struct gabor_filter_bank_s bank, const unsigned int filter_num);

struct image_s reconstruct_image_from_responses(struct gabor_responses_s resps);

void disp_gabor_filter_bank(struct gabor_filter_bank_s bank);

void free_gabor_responses(struct gabor_responses_s resps);
void free_gabor_filter_bank(struct gabor_filter_bank_s bank);

#endif
