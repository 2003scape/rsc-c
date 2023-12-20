#include "toonscape.h"
#include <string.h>

/*
 * Convert textures to solid colours to save memory and provide
 * a cartoony appearance. ToonScape.
 */

#define TEXTURE_WATER       (1)
#define TEXTURE_WALL        (2)
#define TEXTURE_PLANKS      (3)
#define TEXTURE_ROOF        (6)
#define TEXTURE_LEAFYTREE   (8)
#define TEXTURE_TREESTUMP   (9)
#define TEXTURE_MOSSY       (11)
#define TEXTURE_MARBLE      (15)
#define TEXTURE_MOSSYBRICKS (23)
#define TEXTURE_GUNGYWATER  (25)
#define TEXTURE_CAVERN      (29)
#define TEXTURE_CAVERN2     (30)
#define TEXTURE_LAVA        (31)
#define TEXTURE_TENTBOTTOM  (36)
#define TEXTURE_CHAINMAIL2  (37)
#define TEXTURE_MUMMY       (38)
#define TEXTURE_BARK        (48)
#define TEXTURE_CANVAS      (49)

const int modded_textures[] = {
TEXTURE_WATER,
TEXTURE_WALL,
TEXTURE_PLANKS,
TEXTURE_ROOF,
TEXTURE_LEAFYTREE,
TEXTURE_TREESTUMP,
TEXTURE_MOSSY,
TEXTURE_MARBLE,
TEXTURE_MOSSYBRICKS,
TEXTURE_GUNGYWATER,
TEXTURE_CAVERN,
TEXTURE_CAVERN2,
TEXTURE_LAVA,
TEXTURE_TENTBOTTOM,
TEXTURE_CHAINMAIL2,
TEXTURE_MUMMY,
TEXTURE_BARK,
TEXTURE_CANVAS,
-1
};

int toonscape_avoid_load(int id) {
    const int *mod_id = modded_textures;
    while (*mod_id != -1) {
        if (*mod_id == id) {
            return 1;
        }
        mod_id++;
    }
    return 0;
}

static int32_t encode_rgb(uint8_t r, uint8_t g, uint8_t b) {
    /*
     * Convert r, g, b values to inverted 16-bit colour as used by
     * Jagex's scene graph
     */
    return -1 - ((r >> 3) << 10) - ((g >> 3) << 5) - (b >> 3);
}

int32_t apply_toonscape(int32_t fill) {
    switch (fill) {
    case TEXTURE_BARK:
        return encode_rgb(148, 75, 17);
    case TEXTURE_CANVAS:
    case TEXTURE_TENTBOTTOM:
        return encode_rgb(230, 198, 155);
    case TEXTURE_CAVERN:
    case TEXTURE_CAVERN2:
        return encode_rgb(90, 41, 24);
    case TEXTURE_GUNGYWATER:
        return encode_rgb(54, 84, 102);
    case TEXTURE_LAVA:
        return encode_rgb(255, 111, 12);
    case TEXTURE_LEAFYTREE:
        return encode_rgb(0, 118, 0);
    case TEXTURE_MARBLE:
        return encode_rgb(255, 255, 255);
    case TEXTURE_MOSSY:
        return encode_rgb(120, 122, 121);
    case TEXTURE_MOSSYBRICKS:
        return encode_rgb(51, 54, 51);
    case TEXTURE_MUMMY:
        return encode_rgb(223, 219, 200);
    case TEXTURE_PLANKS:
        return encode_rgb(168, 83, 10);
    case TEXTURE_ROOF:
        return encode_rgb(115, 42, 22);
    case TEXTURE_TREESTUMP:
        return encode_rgb(160, 89, 27);
    case TEXTURE_WALL:
        return encode_rgb(49, 49, 49);
    case TEXTURE_WATER:
        return encode_rgb(80, 145, 255);
    case TEXTURE_CHAINMAIL2:
        return encode_rgb(74, 74, 74);
    }
    return fill;
}
