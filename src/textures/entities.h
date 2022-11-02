#ifndef _H_ENTITIES_TEXTURES

#include "../surface.h"

#ifdef RENDER_3DS_GL
typedef struct _3ds_gl_entity_texture {
    int texture_id;
    int skin_colour;
    _3ds_gl_atlas_position atlas_position;
} _3ds_gl_entity_texture;

extern _3ds_gl_entity_texture _3ds_gl_entities_texture_positions[];
extern _3ds_gl_entity_texture _3ds_gl_entities_base_texture_positions[];
#endif

#endif
