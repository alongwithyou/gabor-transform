#ifndef image_h
#define image_h

#include "types.h"

struct image_s init_image_empty(const unsigned int height, const unsigned int width);

struct image_s init_image_path(const char* const filepath);

void free_image(struct image_s img);

void save_image(struct image_s img, const char* const prefix);

#endif
