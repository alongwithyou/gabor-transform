#ifndef gabor_h
#define gabor_h

#include "types.h"

struct gabor_filter_bank_s init_gabor_filter_bank_default(int height, int width);

struct gabor_responses_s init_gabor_responses_empty(int height, int width, int num_filters);

struct gabor_responses_s apply_gabor_filter_bank(struct image_s img, struct gabor_filter_bank_s bank);

struct gabor_filter_s init_gabor_filter_from_params(double freq, double angle, double sigma, int filt_height, int filt_width);
struct gabor_filter_s init_gabor_filter_from_bank(struct gabor_filter_bank_s bank, int filter_num);
struct gabor_filter_s init_gabor_filter_empty(int width, int height);

void disp_gabor_filter_bank(struct gabor_filter_bank_s bank);

void free_gabor_responses(struct gabor_responses_s resps);
void free_gabor_filter_bank(struct gabor_filter_bank_s bank);
void free_gabor_filter(struct gabor_filter_s filt);

#endif
