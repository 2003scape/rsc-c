#ifndef _H_ENTITIES_TEXTURES
#define _H_ENTITIES_TEXTURES

#define SKIN_SPRITE_LENGTH 123
#define SKIN_COLOUR_LENGTH 31

#include "../../surface.h"

typedef struct gl_entity_texture {
    int texture_index;
    gl_atlas_position atlas_position;
} gl_entity_texture;

extern int gl_entity_skin_sprites[];
extern int gl_entity_skin_colours[];

extern gl_entity_texture gl_entities_texture_positions[];
extern gl_entity_texture gl_entities_base_texture_positions[];

/* [sprite_index][skin_index] */
extern gl_entity_texture
    gl_entities_skin_texture_positions[][SKIN_SPRITE_LENGTH];

int gl_get_entity_sprite_index(int sprite_id);
int gl_get_entity_skin_index(int skin_colour);
#endif
