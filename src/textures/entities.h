#ifndef _H_ENTITIES_TEXTURES

#include "../surface.h"

#ifdef RENDER_3DS_GL
#define SKIN_SPRITE_LENGTH 123
#define SKIN_COLOUR_LENGTH 31

typedef struct _3ds_gl_entity_texture {
    int texture_index;
    _3ds_gl_atlas_position atlas_position;
} _3ds_gl_entity_texture;

extern int _3ds_gl_skin_sprites[];
extern int _3ds_gl_skin_colours[];

extern _3ds_gl_entity_texture _3ds_gl_entities_texture_positions[];
extern _3ds_gl_entity_texture _3ds_gl_entities_base_texture_positions[];

/* [sprite_index][skin_index] */
extern _3ds_gl_entity_texture
    _3ds_gl_entities_skin_texture_positions[][SKIN_COLOUR_LENGTH];

int _3ds_gl_get_entity_sprite_index(int sprite_id);
int _3ds_gl_get_entity_skin_index(int skin_colour);
#endif

#endif
