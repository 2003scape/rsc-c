#include "entities.h"

int gl_entity_skin_sprites[] = { $skin_sprites };

int gl_entity_skin_colours[] = { $skin_colours };

gl_entity_texture gl_entities_texture_positions[] = {
$positions
};

gl_entity_texture gl_entities_base_texture_positions[] = {
$base_positions
};

gl_entity_texture gl_entities_skin_texture_positions[][SKIN_SPRITE_LENGTH] = {
$skin_positions
};

int gl_get_entity_sprite_index(int sprite_id) {
    for (int i = 0; i < SKIN_SPRITE_LENGTH; i++) {
        if (sprite_id == gl_entity_skin_sprites[i]) {
            return i;
        }
    }

    return -1;
}

int gl_get_entity_skin_index(int skin_colour) {
    if (skin_colour == 0xeaded2) {
        skin_colour = 0xecded0;
    } else if (skin_colour == 0xecffd0) {
        skin_colour = 0xecfed0;
    }

    for (int i = 0; i < SKIN_COLOUR_LENGTH; i++) {
        if (skin_colour == gl_entity_skin_colours[i]) {
            return i;
        }
    }

    return -1;
}
