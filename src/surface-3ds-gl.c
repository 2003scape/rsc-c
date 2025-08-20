#include "surface.h"

#ifdef RENDER_3DS_GL

void surface_gl_draw(Surface *surface, GL_DEPTH_MODE depth_mode) {
    C3D_BindProgram(&surface->_3ds_gl_flat_shader);

    C3D_CullFace(GPU_CULL_FRONT_CCW);

    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, surface->_3ds_gl_projection_uniform,
                     &surface->_3ds_gl_projection);

    vertex_buffer_gl_bind(&surface->gl_flat_buffer);

    C3D_TexEnv *tex_env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(tex_env);

    /* multiply the primary colour by the first texture colour */
    C3D_TexEnvSrc(tex_env, C3D_Both, GPU_PRIMARY_COLOR, GPU_TEXTURE0, 0);
    C3D_TexEnvFunc(tex_env, C3D_Both, GPU_MODULATE);

    tex_env = C3D_GetTexEnv(1);
    C3D_TexEnvInit(tex_env);

    /* add the second texture to the empty pixels */
    C3D_TexEnvSrc(tex_env, C3D_Both, GPU_PREVIOUS, GPU_TEXTURE1, 0);
    C3D_TexEnvFunc(tex_env, C3D_Both, GPU_ADD);

    int drawn_quads = 0;

    for (int i = 0; i < surface->gl_context_count; i++) {
        SurfaceGlContext *context = &surface->gl_contexts[i];

        if (context->quad_count <= 0) {
            continue;
        }

        if (depth_mode == GL_DEPTH_DISABLED && !context->use_depth) {
            drawn_quads += context->quad_count;
            continue;
        }

        if (depth_mode == GL_DEPTH_ENABLED && context->use_depth) {
            drawn_quads += context->quad_count;
            continue;
        }

        if (context->use_depth) {
            C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);
        } else {
            C3D_DepthTest(true, GPU_ALWAYS, GPU_WRITE_ALL);
        }

        C3D_SetScissor(GPU_SCISSOR_NORMAL, 240 - context->max_y,
                       320 - context->max_x, 240 - context->min_y,
                       320 - context->min_x);

        C3D_TexBind(0, context->texture);
        C3D_TexBind(1, context->base_texture);

        C3D_DrawElements(
            GPU_TRIANGLES, context->quad_count * 6, C3D_UNSIGNED_SHORT,
            surface->gl_flat_buffer.ebo + (drawn_quads * 6) * sizeof(uint16_t));

        drawn_quads += context->quad_count;
    }

    if (depth_mode == GL_DEPTH_BOTH) {
        surface_gl_reset_context(surface);
    }
}

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
