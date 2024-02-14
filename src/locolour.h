#ifndef LOCOLOUR_H
#define LOCOLOUR_H
#include <stddef.h>
#include <stdint.h>

extern const uint32_t ibm_vga_palette[256];

uint8_t locolour_get_nearest(uint32_t);

void palette_to_locolour(uint8_t *, size_t, uint32_t *);
void rgb_to_locolour(uint32_t *, size_t, uint8_t *);
#endif
