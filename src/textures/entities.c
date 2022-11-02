#include "entities.h"

#ifdef RENDER_3DS_GL
int _3ds_gl_skin_sprites[] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,
    14,  15,  16,  17,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
    37,  38,  39,  40,  41,  42,  43,  44,  81,  82,  83,  84,  85,  86,
    87,  88,  89,  90,  91,  92,  96,  97,  98,  108, 109, 110, 111, 112,
    113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 135,
    136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
    150, 151, 152, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172,
    173, 174, 175, 176, 177, 178, 179, 486, 487, 488, 489, 490, 491, 492,
    493, 494, 495, 496, 497, 498, 499, 500, 501, 502, 503};

int _3ds_gl_skin_colours[] = {
    0x906020, 0xecded0, 0x997326, 0xccb366, 0xfceee0, 0xdcffd0, 0x55cfff,
    0xecfed0, 0xfffef0, 0xdd3040, 0x55bfee, 0xeceed0, 0xac9e90, 0xff3333,
    0xffded2, 0x000004, 0xff9f55, 0xdcc399, 0x009000, 0x705010, 0xdccea0,
    0x6f5737, 0x3cb371, 0xb38c40, 0xffffff, 0x0066ff, 0x999999, 0x604020,
    0x996633, 0x663300, 0x804000};

_3ds_gl_entity_texture _3ds_gl_entities_texture_positions[] = {
    {0, {0.274414f, 0.466797f, 0.805664f, 0.836914f}}};

int _3ds_gl_get_entity_sprite_index(int sprite_id) {
    for (int i = 0; i < SKIN_SPRITE_LENGTH; i++) {
        if (sprite_id == _3ds_gl_skin_sprites[i]) {
            return i;
        }
    }

    return -1;
}

int _3ds_gl_get_entity_skin_index(int skin_colour) {
    if (skin_colour == 0xeaded2) {
        skin_colour = 0xecded0;
    } else if (skin_colour == 0xecffd0) {
        skin_colour = 0xecfed0;
    }

    for (int i = 0; i < SKIN_COLOUR_LENGTH; i++) {
        if (skin_colour == _3ds_gl_skin_colours[i]) {
            return i;
        }
    }

    return -1;
}
#endif
