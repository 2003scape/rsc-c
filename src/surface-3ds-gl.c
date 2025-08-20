#include "surface.h"

#ifdef RENDER_3DS_GL

void surface_gl_raster_to_sprite(Surface *surface, int sprite_id, int x,
                                 int y, int width, int height) {
    (void)x;
    (void)y;

    int offset_x = 0;
    int offset_y = 0;

    if (!surface_3ds_gl_get_sprite_texture_offsets(surface, sprite_id,
                                                   &offset_x, &offset_y)) {
        return;
    }

    uint16_t *colour_buf =
        (uint16_t *)(surface->mud->_3ds_gl_offscreen_render_target->frameBuf
                         .colorBuf);

    uint16_t *texture_data = (uint16_t *)surface->gl_sprite_texture.data;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int framebuffer_offset =
                _3ds_gl_translate_framebuffer_index((y * 320) + x);

            int texture_offset = _3ds_gl_translate_texture_index(
                                     x + offset_x, y + offset_y, 1024) /
                                 sizeof(uint16_t);

            uint16_t colour = colour_buf[framebuffer_offset];

            if (y < 6 || y >= height - 6) {
                colour = 1;
            }

            texture_data[texture_offset] = colour;
        }
    }
}

int surface_3ds_gl_get_sprite_texture_offsets(Surface *surface, int sprite_id,
                                              int *offset_x, int *offset_y) {
    gl_atlas_position *atlas_position = NULL;

    if (sprite_id == surface->mud->sprite_logo) {
        atlas_position = &gl_login_atlas_positions[0];
    } else if (sprite_id == surface->mud->sprite_logo + 1) {
        atlas_position = &gl_login_atlas_positions[1];
    } else if (sprite_id == surface->mud->sprite_logo + 2) {
        atlas_position = &gl_login_atlas_positions[2];
    } else if (sprite_id == surface->mud->sprite_media - 1) {
        atlas_position = &gl_map_atlas_position;
    } else if (sprite_id == surface->mud->sprite_texture + 1) {
        atlas_position = &gl_sleep_atlas_position;
    }

    if (!atlas_position) {
        return 0;
    }

    *offset_x = (int)(atlas_position->left_u * GL_TEXTURE_SIZE);
    *offset_y = (int)(atlas_position->top_v * GL_TEXTURE_SIZE);

    return 1;
}
#endif
