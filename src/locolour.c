#include "locolour.h"
#include <limits.h>
#include <stdlib.h>

/*
 * Standardized 256-colour palette for reduced memory usage
 */

#define GREYSCALE_START     (16)
#define GREYSCALE_NUM       (16)
#define GREYSCALE_END       (32)

#define PINKISH_START       (248)
#define PINKISH_END         (254)

#define GET_RED(colour)     (int)((colour >> 16) & 0xff)
#define GET_GREEN(colour)   (int)((colour >> 8) & 0xff)
#define GET_BLUE(colour)    (int)((colour >> 0) & 0xff)

/* 
 * Modifications:
 * - FF00FF added for colour key blitting
 * - replaced "empty" black slots at the end of the map with
 *   FFxxxx shades for skin recolouring
 */
const uint32_t ibm_vga_palette[256] = {
    0xFF00FF, 0x0000AA, 0x00AA00, 0x00AAAA, 0xAA0000, 0xAA00AA, 0xAA5500,
    0xAAAAAA, 0x555555, 0x5555FF, 0x55FF55, 0x55FFFF, 0xFF5555, 0xFF55FF,
    0xFFFF55, 0xFFFFFF, 0x000000, 0x101010, 0x202020, 0x353535, 0x454545,
    0x555555, 0x656565, 0x757575, 0x8A8A8A, 0x9A9A9A, 0xAAAAAA, 0xBABABA,
    0xCACACA, 0xDFDFDF, 0xEFEFEF, 0xFFFFFF, 0x0000FF, 0x4100FF, 0x8200FF,
    0xBE00FF, 0xFF00FF, 0xFF00BE, 0xFF0082, 0xFF0041, 0xFF0000, 0xFF4100,
    0xFF8200, 0xFFBE00, 0xFFFF00, 0xBEFF00, 0x82FF00, 0x41FF00, 0x00FF00,
    0x00FF41, 0x00FF82, 0x00FFBE, 0x00FFFF, 0x00BEFF, 0x0082FF, 0x0041FF,
    0x8282FF, 0x9E82FF, 0xBE82FF, 0xDF82FF, 0xFF82FF, 0xFF82DF, 0xFF82BE,
    0xFF829E, 0xFF8282, 0xFF9E82, 0xFFBE82, 0xFFDF82, 0xFFFF82, 0xDFFF82,
    0xBEFF82, 0x9EFF82, 0x82FF82, 0x82FF9E, 0x82FFBE, 0x82FFDF, 0x82FFFF,
    0x82DFFF, 0x82BEFF, 0x829EFF, 0xBABAFF, 0xCABAFF, 0xDFBAFF, 0xEFBAFF,
    0xFFBAFF, 0xFFBAEF, 0xFFBADF, 0xFFBACA, 0xFFBABA, 0xFFCABA, 0xFFDFBA,
    0xFFEFBA, 0xFFFFBA, 0xEFFFBA, 0xDFFFBA, 0xCAFFBA, 0xBAFFBA, 0xBAFFCA,
    0xBAFFDF, 0xBAFFEF, 0xBAFFFF, 0xBAEFFF, 0xBADFFF, 0xBACAFF, 0x000071,
    0x1C0071, 0x390071, 0x550071, 0x710071, 0x710055, 0x710039, 0x71001C,
    0x710000, 0x711C00, 0x713900, 0x715500, 0x717100, 0x557100, 0x397100,
    0x1C7100, 0x007100, 0x00711C, 0x007139, 0x007155, 0x007171, 0x005571,
    0x003971, 0x001C71, 0x393971, 0x453971, 0x553971, 0x613971, 0x713971,
    0x713961, 0x713955, 0x713945, 0x713939, 0x714539, 0x715539, 0x716139,
    0x717139, 0x617139, 0x557139, 0x457139, 0x397139, 0x397145, 0x397155,
    0x397161, 0x397171, 0x396171, 0x395571, 0x394571, 0x515171, 0x595171,
    0x615171, 0x695171, 0x715171, 0x715169, 0x715161, 0x715159, 0x715151,
    0x715951, 0x716151, 0x716951, 0x717151, 0x697151, 0x617151, 0x597151,
    0x517151, 0x517159, 0x517161, 0x517169, 0x517171, 0x516971, 0x516171,
    0x515971, 0x000041, 0x100041, 0x200041, 0x310041, 0x410041, 0x410031,
    0x410020, 0x410010, 0x410000, 0x411000, 0x412000, 0x413100, 0x414100,
    0x314100, 0x204100, 0x104100, 0x004100, 0x004110, 0x004120, 0x004131,
    0x004141, 0x003141, 0x002041, 0x001041, 0x202041, 0x282041, 0x312041,
    0x392041, 0x412041, 0x412039, 0x412031, 0x412028, 0x412020, 0x412820,
    0x413120, 0x413920, 0x414120, 0x394120, 0x314120, 0x284120, 0x204120,
    0x204128, 0x204131, 0x204139, 0x204141, 0x203941, 0x203141, 0x202841,
    0x2D2D41, 0x312D41, 0x352D41, 0x3D2D41, 0x412D41, 0x412D3D, 0x412D35,
    0x412D31, 0x412D2D, 0x41312D, 0x41352D, 0x413D2D, 0x41412D, 0x3D412D,
    0x35412D, 0x31412D, 0x2D412D, 0x2D4131, 0x2D4135, 0x2D413D, 0x2D4141,
    0x2D3D41, 0x2D3541, 0x2D3141, 0xFF0000, 0xFF9090, 0xFF8080, 0xFF7070,
    0xFFB0B0, 0xFFC0C0, 0xFFA0A0, 0x000000
};

static uint8_t locolour_get_greyscale(int);

static uint8_t locolour_get_greyscale(int target_shade) {
    uint8_t best_entry = 0;
    uint8_t lowest_score = 255;

    for (unsigned i = GREYSCALE_START; i < GREYSCALE_END; ++i) {
        int shade = GET_BLUE(ibm_vga_palette[i]);
        int difference = abs(shade - target_shade);
        if (difference < lowest_score) {
            best_entry = i;
            lowest_score = difference;
        }
    }
    return best_entry;
}

static uint8_t locolour_get_pinkish(int target_shade) {
    uint8_t best_entry = 0;
    uint8_t lowest_score = 255;

    for (unsigned i = PINKISH_START; i < PINKISH_END; ++i) {
        int shade = GET_BLUE(ibm_vga_palette[i]);
        int difference = abs(shade - target_shade);
        if (difference < lowest_score) {
            best_entry = i;
            lowest_score = difference;
        }
    }
    return best_entry;
}

static void try_colour_range(int *best_entry, int *best_value,
                             unsigned min, unsigned max,
                             int r, int g, int b) {
    for (unsigned i = min; i < max; ++i) {
        int palette_r = GET_RED(ibm_vga_palette[i]);
        int palette_g = GET_GREEN(ibm_vga_palette[i]);
        int palette_b = GET_BLUE(ibm_vga_palette[i]);

        int value = (r - palette_r) * (r - palette_r) +
                    (g - palette_g) * (g - palette_g) +
                    (b - palette_b) * (b - palette_b);

        if (value < *best_value) {
            *best_entry = i;
            *best_value = value;
        }
    }
}

uint8_t locolour_get_nearest(uint32_t target) {
    int r = GET_RED(target);
    int g = GET_GREEN(target);
    int b = GET_BLUE(target);

    if (target == 0xFF00FF) {
        return 0;
    }

    if (target == 0) {
        return 255;
    }

    /* mask type 1, properties must be preserved */
    if (r == g && g == b) {
        return locolour_get_greyscale(r);
    }

    /* mask type 2, properties must be preserved */
    if (r == 255 && g == b) {
        return locolour_get_pinkish(r);
    }

    int best_entry = 0;
    int best_value = INT_MAX;

    /* check the text mode range first */
    try_colour_range(&best_entry, &best_value,
                     1, GREYSCALE_START, r, g, b);

    uint32_t found_colour = ibm_vga_palette[best_entry];
    if (GET_RED(found_colour) == GET_GREEN(found_colour) &&
        GET_GREEN(found_colour) == GET_BLUE(found_colour)) {
         /*
          * there are some problematic greyscales in the text mode
          * range. we have to avoid them so masks aren't applied.
          */
         best_entry = 0;
         best_value = INT_MAX;
    }

    /* now check extended VGA colours */
    try_colour_range(&best_entry, &best_value,
                     GREYSCALE_END, PINKISH_START, r, g, b);

    return best_entry;
}

void palette_to_locolour(uint8_t *pixels, size_t pixels_len,
                         uint32_t *palette) {
    for (size_t i = 0; i < pixels_len; ++i) {
        pixels[i] = locolour_get_nearest(palette[pixels[i]]);
    }
}

void rgb_to_locolour(uint32_t *pixels, size_t pixels_len,
                     uint8_t *output) {
    for (size_t i = 0; i < pixels_len; ++i) {
        output[i] = locolour_get_nearest(pixels[i]);
    }
}
