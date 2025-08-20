#include "surface.h"

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
gl_atlas_position gl_white_atlas_position = {
    .left_u = 0.0f,
    .right_u = 1.0f / GL_TEXTURE_SIZE,
    .top_v = (GL_TEXTURE_SIZE - 1.0f) / GL_TEXTURE_SIZE,
    .bottom_v = (GL_TEXTURE_SIZE) / GL_TEXTURE_SIZE};

gl_atlas_position gl_transparent_atlas_position = {
    .left_u = 2.0f / GL_TEXTURE_SIZE,
    .right_u = 3.0f / GL_TEXTURE_SIZE,
    .top_v = (GL_TEXTURE_SIZE - 1.0f) / GL_TEXTURE_SIZE,
    .bottom_v = (GL_TEXTURE_SIZE) / GL_TEXTURE_SIZE};

static unsigned int last_base_texture = 0;

static void surface_gl_quad_new(Surface *surface, gl_quad *quad, int x, int y,
                                int width, int height);
#endif

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
void surface_gl_new(Surface *surface, int width, int height, int limit,
                    mudclient *mud) {
#ifdef RENDER_GL
    surface_gl_create_framebuffer(surface);

#ifdef EMSCRIPTEN
    shader_new(&surface->gl_flat_shader, "./cache/flat.webgl.vs",
               "./cache/flat.webgl.fs");
#elif defined(OPENGL15) || defined(OPENGL20)
    shader_new(&surface->gl_flat_shader, "./cache/flat.gl2.vs",
               "./cache/flat.gl2.fs");
#elif defined(__SWITCH__)
    shader_new(&surface->gl_flat_shader, "romfs:/flat.vs", "romfs:/flat.fs");
#else
    shader_new(&surface->gl_flat_shader, "./cache/flat.vs", "./cache/flat.fs");
#endif

    shader_use(&surface->gl_flat_shader);

    shader_set_int(&surface->gl_flat_shader, "sprite_texture", 0);
    shader_set_int(&surface->gl_flat_shader, "sprite_base_texture", 1);
#elif defined(RENDER_3DS_GL)
    surface->_3ds_gl_flat_shader_dvlb =
        DVLB_ParseFile((u32 *)flat_shbin, flat_shbin_size);

    shaderProgramInit(&surface->_3ds_gl_flat_shader);

    shaderProgramSetVsh(&surface->_3ds_gl_flat_shader,
                        &surface->_3ds_gl_flat_shader_dvlb->DVLE[0]);

    C3D_BindProgram(&surface->_3ds_gl_flat_shader);
#endif

    vertex_buffer_gl_new(&surface->gl_flat_buffer, sizeof(gl_quad_vertex),
                         GL_MAX_QUADS * 4, GL_MAX_QUADS * 6);

    int attribute_offset = 0;

    /* vertex { x, y, z } */
    vertex_buffer_gl_add_attribute(&surface->gl_flat_buffer, &attribute_offset,
                                   3);

    /* colour { r, g, b, a } */
    vertex_buffer_gl_add_attribute(&surface->gl_flat_buffer, &attribute_offset,
                                   4);

    /* greyscale texture { u, v } */
    vertex_buffer_gl_add_attribute(&surface->gl_flat_buffer, &attribute_offset,
                                   2);

    /* base texture { u, v } */
    vertex_buffer_gl_add_attribute(&surface->gl_flat_buffer, &attribute_offset,
                                   2);

#ifdef RENDER_GL
#ifdef __SWITCH__
    gl_load_texture(&surface->gl_sprite_texture, "romfs:/textures/sprites.png");
#else
    gl_load_texture(&surface->gl_sprite_texture,
                    "./cache/textures/sprites.png");
#endif

    for (int i = 0; i < ENTITY_TEXTURE_LENGTH; i++) {
        char filename[32] = {0};
#ifdef __SWITCH__
        sprintf(filename, "romfs:/textures/entities_%d.png", i);
#else
        sprintf(filename, "./cache/textures/entities_%d.png", i);
#endif

        gl_load_texture(&surface->gl_entity_textures[i], filename);
    }

    surface->gl_dynamic_texture_buffer =
        calloc(1024 * 1024 * 3, sizeof(uint8_t));

    gl_create_texture(&surface->gl_dynamic_texture);
#elif defined(RENDER_3DS_GL)
    surface->_3ds_gl_projection_uniform = shaderInstanceGetUniformLocation(
        (&surface->_3ds_gl_flat_shader)->vertexShader, "projection");

    BufInfo_Add(&surface->gl_flat_buffer.buf_info, surface->gl_flat_buffer.vbo,
                sizeof(gl_quad_vertex), 4, 0x3210);

    _3ds_gl_load_tex(sprites_t3x, sprites_t3x_size,
                     &surface->gl_sprite_texture);

    _3ds_gl_load_tex(entities_0_t3x, entities_0_t3x_size,
                     &surface->gl_entity_textures[0]);

    _3ds_gl_load_tex(entities_1_t3x, entities_1_t3x_size,
                     &surface->gl_entity_textures[1]);

    _3ds_gl_load_tex(entities_2_t3x, entities_2_t3x_size,
                     &surface->gl_entity_textures[2]);

    _3ds_gl_load_tex(entities_3_t3x, entities_3_t3x_size,
                     &surface->gl_entity_textures[3]);

    _3ds_gl_load_tex(entities_4_t3x, entities_4_t3x_size,
                     &surface->gl_entity_textures[4]);

    Mtx_OrthoTilt(&surface->_3ds_gl_projection, 0.0, 320.0, 0.0, 240.0, 0.0,
                  1.0, true);
#endif

    surface_gl_reset_context(surface);
}

float surface_gl_translate_x(Surface *surface, int x) {
    return gl_translate_x(x, surface->width);
}

float surface_gl_translate_y(Surface *surface, int y) {
    return gl_translate_y(y, surface->height);
}

void surface_gl_reset_context(Surface *surface) {
    surface->gl_flat_count = 0;

#ifdef RENDER_GL
    surface->gl_contexts[0].texture = surface->gl_sprite_texture;
    surface->gl_contexts[0].base_texture = surface->gl_sprite_texture;
#elif defined(RENDER_3DS_GL)
    surface->gl_contexts[0].texture = &surface->gl_sprite_texture;
    surface->gl_contexts[0].base_texture = &surface->gl_sprite_texture;
#endif

    surface->gl_contexts[0].quad_count = 0;

    surface->gl_contexts[0].min_x = surface->bounds_min_x;
    surface->gl_contexts[0].max_x = surface->bounds_max_x;
    surface->gl_contexts[0].min_y = surface->bounds_min_y;
    surface->gl_contexts[0].max_y = surface->bounds_max_y;

    surface->gl_contexts[0].use_depth = 0;

    surface->gl_context_count = 1;
}

static void surface_gl_quad_new(Surface *surface, gl_quad *quad, int x, int y,
                                int width, int height) {
#ifdef RENDER_GL
    float left_x = surface_gl_translate_x(surface, x);
    float right_x = surface_gl_translate_x(surface, x + width);

    float top_y = surface_gl_translate_y(surface, y);
    float bottom_y = surface_gl_translate_y(surface, y + height);
#elif defined(RENDER_3DS_GL)
    float left_x = x;
    float right_x = x + width;

    float top_y = 240 - y - height;
    float bottom_y = 240 - y;
#endif

    quad->bottom_left.x = left_x;
    quad->bottom_left.y = bottom_y;

    quad->bottom_right.x = right_x;
    quad->bottom_right.y = bottom_y;

    quad->top_right.x = right_x;
    quad->top_right.y = top_y;

    quad->top_left.x = left_x;
    quad->top_left.y = top_y;
}

void surface_gl_quad_apply_atlas(gl_quad *quad,
                                 gl_atlas_position atlas_position, int flip) {
    if (flip) {
        quad->bottom_left.u = atlas_position.right_u;
        quad->bottom_right.u = atlas_position.left_u;

        quad->top_left.u = atlas_position.right_u;
        quad->top_right.u = atlas_position.left_u;
    } else {
        quad->bottom_left.u = atlas_position.left_u;
        quad->bottom_right.u = atlas_position.right_u;

        quad->top_left.u = atlas_position.left_u;
        quad->top_right.u = atlas_position.right_u;
    }

#ifdef RENDER_GL
    quad->bottom_left.v = atlas_position.bottom_v;
    quad->bottom_right.v = atlas_position.bottom_v;

    quad->top_right.v = atlas_position.top_v;
    quad->top_left.v = atlas_position.top_v;
#elif defined(RENDER_3DS_GL)
    quad->bottom_left.v = 1.0f - atlas_position.top_v;
    quad->bottom_right.v = 1.0f - atlas_position.top_v;

    quad->top_right.v = 1.0f - atlas_position.bottom_v;
    quad->top_left.v = 1.0f - atlas_position.bottom_v;
#endif
}

void surface_gl_quad_apply_base_atlas(gl_quad *quad,
                                      gl_atlas_position atlas_position,
                                      int flip) {
    if (flip) {
        quad->bottom_left.base_u = atlas_position.right_u;
        quad->bottom_right.base_u = atlas_position.left_u;

        quad->top_left.base_u = atlas_position.right_u;
        quad->top_right.base_u = atlas_position.left_u;
    } else {
        quad->bottom_left.base_u = atlas_position.left_u;
        quad->bottom_right.base_u = atlas_position.right_u;

        quad->top_left.base_u = atlas_position.left_u;
        quad->top_right.base_u = atlas_position.right_u;
    }

#ifdef RENDER_GL
    quad->bottom_left.base_v = atlas_position.bottom_v;
    quad->bottom_right.base_v = atlas_position.bottom_v;

    quad->top_right.base_v = atlas_position.top_v;
    quad->top_left.base_v = atlas_position.top_v;
#elif defined(RENDER_3DS_GL)
    quad->bottom_left.base_v = 1.0f - atlas_position.top_v;
    quad->bottom_right.base_v = 1.0f - atlas_position.top_v;

    quad->top_right.base_v = 1.0f - atlas_position.bottom_v;
    quad->top_left.base_v = 1.0f - atlas_position.bottom_v;
#endif
}

void surface_gl_vertex_apply_colour(gl_quad_vertex *vertices, int length,
                                    int colour, int alpha) {
    float r = ((colour >> 16) & 0xff) / 255.0f;
    float g = ((colour >> 8) & 0xff) / 255.0f;
    float b = (colour & 0xff) / 255.0f;
    float a = alpha / 255.0f;

    for (int i = 0; i < length; i++) {
        vertices[i].r = r;
        vertices[i].g = g;
        vertices[i].b = b;
        vertices[i].a = a;
    }
}

void surface_gl_vertex_apply_depth(gl_quad_vertex *vertices, int length,
                                   float depth) {
    for (int i = 0; i < length; i++) {
        vertices[i].z = depth;
    }
}

void gl_vertex_apply_rotation(float *x, float *y, float centre_x,
                              float centre_y, float angle) {
    *x -= centre_x;
    *y -= centre_y;

    float sine = sin(angle);
    float cosine = cos(angle);

    float x_new = (*x) * cosine - (*y) * sine;
    float y_new = (*x) * sine + (*y) * cosine;

    *x = x_new + centre_x;
    *y = y_new + centre_y;
}

#ifdef RENDER_GL
void surface_gl_buffer_quad(Surface *surface, gl_quad *quad, GLuint texture,
                            GLuint base_texture) {
#elif defined(RENDER_3DS_GL)
void surface_gl_buffer_quad(Surface *surface, gl_quad *quad, C3D_Tex *texture,
                            C3D_Tex *base_texture) {
#endif
    if (surface->gl_context_count >= GL_MAX_QUADS) {
        mud_error("too many context (texture/boundary) switches!\n");
        return;
    }

    if (surface->gl_flat_count >= GL_MAX_QUADS) {
        mud_error("too many quads!\n");
        return;
    }

    int vertex_offset = surface->gl_flat_count * sizeof(gl_quad);
    int ebo_index = surface->gl_flat_count * 4;

#ifdef RENDER_GL
    vertex_buffer_gl_bind(&surface->gl_flat_buffer);

    glBufferSubData(GL_ARRAY_BUFFER, vertex_offset, sizeof(gl_quad), quad);

    GLuint indices[] = {ebo_index, ebo_index + 1, ebo_index + 2,
                        ebo_index, ebo_index + 2, ebo_index + 3};

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                    surface->gl_flat_count * sizeof(indices), sizeof(indices),
                    indices);
#elif defined(RENDER_3DS_GL)
    memcpy(surface->gl_flat_buffer.vbo + vertex_offset, quad, sizeof(gl_quad));

    uint16_t indices[] = {ebo_index, ebo_index + 1, ebo_index + 2,
                          ebo_index, ebo_index + 2, ebo_index + 3};

    memcpy(surface->gl_flat_buffer.ebo +
               (surface->gl_flat_count * sizeof(indices)),
           indices, sizeof(indices));
#endif

    surface->gl_flat_count++;

    int context_index = surface->gl_context_count - 1;
    SurfaceGlContext *context = &surface->gl_contexts[context_index];

    if (context->use_depth == 0 && quad->bottom_left.z == 0 &&
        context->min_x == surface->bounds_min_x &&
        context->max_x == surface->bounds_max_x &&
        context->min_y == surface->bounds_min_y &&
        context->max_y == surface->bounds_max_y &&
        context->texture == texture && context->base_texture == base_texture) {
        context->quad_count++;
    } else {
        context = &surface->gl_contexts[context_index + 1];

        context->min_x = surface->bounds_min_x;
        context->max_x = surface->bounds_max_x;
        context->min_y = surface->bounds_min_y;
        context->max_y = surface->bounds_max_y;

        context->texture = texture;
        context->base_texture = base_texture;

        context->use_depth = quad->bottom_left.z != 0;

        context->quad_count = 1;

        surface->gl_context_count++;
    }
}

void surface_gl_buffer_box(Surface *surface, int x, int y, int width,
                           int height, int colour, int alpha) {
    gl_quad quad = {0};

    surface_gl_quad_new(surface, &quad, x, y, width, height);
    surface_gl_quad_apply_atlas(&quad, gl_white_atlas_position, 0);
    surface_gl_quad_apply_base_atlas(&quad, gl_transparent_atlas_position, 0);
    surface_gl_vertex_apply_colour((gl_quad_vertex *)(&quad), 4, colour, alpha);

#ifdef RENDER_GL
    surface_gl_buffer_quad(surface, &quad, surface->gl_sprite_texture,
                           surface->gl_sprite_texture);
#elif defined(RENDER_3DS_GL)
    surface_gl_buffer_quad(surface, &quad, &surface->gl_sprite_texture,
                           &surface->gl_sprite_texture);
#endif
}

void surface_gl_buffer_character(Surface *surface, char character, int x, int y,
                                 int colour, int font_id, int draw_shadow,
                                 float depth) {
    if (character == ' ') {
        return;
    }

    int8_t *font_data = game_fonts[font_id];
    int char_set_index = -1;

    for (int i = 0; i < CHAR_SET_LENGTH; i++) {
        if (character == CHAR_SET[i]) {
            char_set_index = i;
            break;
        }
    }

    if (char_set_index == -1) {
        return;
    }

    int character_offset = character_width[(unsigned)CHAR_SET[char_set_index]];
    int width = font_data[character_offset + 3] + (draw_shadow ? 1 : 0);
    int height = font_data[character_offset + 4] + (draw_shadow ? 1 : 0);

    x += font_data[character_offset + 5];
    y -= font_data[character_offset + 6];

    gl_quad quad = {0};

    surface_gl_quad_new(surface, &quad, x, y, width, height);

    gl_atlas_position atlas_position =
        draw_shadow ? gl_font_shadow_atlas_positions[font_id][char_set_index]
                    : gl_font_atlas_positions[font_id][char_set_index];

    surface_gl_quad_apply_atlas(&quad, atlas_position, 0);
    surface_gl_quad_apply_base_atlas(&quad, gl_transparent_atlas_position, 0);

    surface_gl_vertex_apply_colour((gl_quad_vertex *)(&quad), 4, colour, 255);
    surface_gl_vertex_apply_depth((gl_quad_vertex *)(&quad), 4, depth);

#ifdef RENDER_GL
    surface_gl_buffer_quad(surface, &quad, surface->gl_sprite_texture,
                           surface->gl_sprite_texture);
#elif defined(RENDER_3DS_GL)
    surface_gl_buffer_quad(surface, &quad, &surface->gl_sprite_texture,
                           &surface->gl_sprite_texture);
#endif
}

void surface_gl_buffer_sprite(Surface *surface, int sprite_id, int x, int y,
                              int scale_width, int scale_height, int skew_x,
                              int mask_colour, int skin_colour, int alpha,
                              int flip, int rotation, float depth_top,
                              float depth_bottom) {
#ifdef RENDER_GL
    GLuint texture = surface->gl_sprite_texture;
    GLuint base_texture = surface->gl_sprite_texture;
#elif defined(RENDER_3DS_GL)
    C3D_Tex *texture = &surface->gl_sprite_texture;
    C3D_Tex *base_texture = &surface->gl_sprite_texture;
#endif

    gl_atlas_position atlas_position = gl_transparent_atlas_position;
    gl_atlas_position base_atlas_position = gl_transparent_atlas_position;

    if (sprite_id == SPRITE_LIMIT - 1) {
        atlas_position = gl_logo_atlas_position;
    } else if (sprite_id == surface->mud->sprite_logo) {
#ifdef RENDER_GL
        gl_atlas_position test = {MINIMAP_SPRITE_WIDTH / 1024.0f,
                                  (MINIMAP_SPRITE_WIDTH + 512) / 1024.0f, 0.0f,
                                  200.0f / 1024.0f};

        texture = surface->gl_dynamic_texture;
        atlas_position = test;
#elif defined(RENDER_3DS_GL)
        atlas_position = gl_login_atlas_positions[0];
#endif
    } else if (sprite_id == surface->mud->sprite_logo + 1) {
#ifdef RENDER_GL
        gl_atlas_position test = {MINIMAP_SPRITE_WIDTH / 1024.0f,
                                  (MINIMAP_SPRITE_WIDTH + 512) / 1024.0f,
                                  200 / 1024.0f, (200 + 200.0f) / 1024.0f};

        texture = surface->gl_dynamic_texture;
        atlas_position = test;
#elif defined(RENDER_3DS_GL)
        atlas_position = gl_login_atlas_positions[1];
#endif
    } else if (sprite_id == surface->mud->sprite_logo + 2) {
#ifdef RENDER_GL
        gl_atlas_position test = {MINIMAP_SPRITE_WIDTH / 1024.0f,
                                  (MINIMAP_SPRITE_WIDTH + 512) / 1024.0f,
                                  400 / 1024.0f, 600.0f / 1024.0f};

        texture = surface->gl_dynamic_texture;
        atlas_position = test;
#elif defined(RENDER_3DS_GL)
        atlas_position = gl_login_atlas_positions[2];
#endif
    } else if (sprite_id == surface->mud->sprite_media - 1) {
#ifdef RENDER_GL
        gl_atlas_position test = {
            0.0f, (float)MINIMAP_SPRITE_WIDTH / 1024.0f,
            (float)SLEEP_HEIGHT / 1024.0f,
            (float)(SLEEP_HEIGHT + MINIMAP_SPRITE_HEIGHT) / 1024.0f};

        base_texture = surface->gl_dynamic_texture;
        base_atlas_position = test;
#elif defined(RENDER_3DS_GL)
        atlas_position = gl_map_atlas_position;
#endif
    } else if (sprite_id >= 0 && sprite_id < surface->mud->sprite_media) {
        gl_entity_texture texture_position =
            gl_entities_texture_positions[sprite_id];

        int texture_index = texture_position.texture_index;

        if (texture_index >= 0) {
#ifdef RENDER_GL
            texture = surface->gl_entity_textures[texture_index];
#elif defined(RENDER_3DS_GL)
            texture = &surface->gl_entity_textures[texture_index];
#endif
            atlas_position = texture_position.atlas_position;
        }

        gl_entity_texture base_texture_position =
            gl_entities_base_texture_positions[sprite_id];

        if (skin_colour != 0) {
            int skin_index = gl_get_entity_skin_index(skin_colour);
            int skin_sprite_index = gl_get_entity_sprite_index(sprite_id);

            if (skin_sprite_index != -1 && skin_index != -1) {
                base_texture_position =
                    gl_entities_skin_texture_positions[skin_index]
                                                      [skin_sprite_index];
            }
        }

        int base_texture_index = base_texture_position.texture_index;

        if (base_texture_index >= 0) {
#ifdef RENDER_GL
            base_texture = surface->gl_entity_textures[base_texture_index];
#elif defined(RENDER_3DS_GL)
            base_texture = &surface->gl_entity_textures[base_texture_index];
#endif
            base_atlas_position = base_texture_position.atlas_position;
        }
    } else if (sprite_id >= surface->mud->sprite_media &&
               sprite_id < surface->mud->sprite_projectile +
                               game_data.projectile_sprite) {
        int atlas_index = sprite_id - surface->mud->sprite_media;

        atlas_position = gl_media_atlas_positions[atlas_index];
        base_atlas_position = gl_media_base_atlas_positions[atlas_index];
    } else if (sprite_id == surface->mud->sprite_texture + 1) {
#ifdef RENDER_GL
        base_texture = surface->gl_dynamic_texture;

        gl_atlas_position test = {0.0f, (float)SLEEP_WIDTH / 1024.0f, 0.0f,
                                  (float)SLEEP_HEIGHT / 1024.0f};

        base_atlas_position = test;
#elif defined(RENDER_3DS_GL)
        atlas_position = gl_sleep_atlas_position;
#else
        return;
#endif
    } else {
        return;
    }

    float ratio_x = 1.0;
    float ratio_y = 1.0;
    float width_full = surface->sprite_width_full[sprite_id];
    float height_full = surface->sprite_height_full[sprite_id];

    if (scale_width != -1) {
        ratio_x = scale_width / width_full;
        ratio_y = scale_height / height_full;
    }

    float gl_width = surface->sprite_width[sprite_id] * ratio_x;
    float gl_height = surface->sprite_height[sprite_id] * ratio_y;

    float translate_x = surface->sprite_translate_x[sprite_id];

    if (flip) {
        translate_x =
            width_full - surface->sprite_width[sprite_id] - translate_x;
    }

    translate_x *= ratio_x;

    float translate_y = surface->sprite_translate_y[sprite_id] * ratio_y;

    if (surface->sprite_translate[sprite_id]) {
        x += floorf(translate_x);
        y += floorf(translate_y);
    }

    gl_quad quad = {0};

    surface_gl_quad_new(surface, &quad, x, y, gl_width, gl_height);

    if (skew_x != 0) {
#ifdef RENDER_GL
        float top_left_skew =
            (1.0f - (translate_y / (height_full * ratio_y))) * (float)skew_x;

        float bottom_left_skew =
            (1.0f - ((translate_y + gl_height) / (height_full * ratio_y))) *
            (float)skew_x;
#elif defined(RENDER_3DS_GL)
        float bottom_left_skew =
            (1.0f - (translate_y / (height_full * ratio_y))) * (float)skew_x;

        float top_left_skew =
            (1.0f - ((translate_y + gl_height) / (height_full * ratio_y))) *
            (float)skew_x;
#endif

        quad.bottom_left.x =
            surface_gl_translate_x(surface, x + bottom_left_skew);

        quad.bottom_right.x =
            surface_gl_translate_x(surface, x + gl_width + bottom_left_skew);

        quad.top_right.x =
            surface_gl_translate_x(surface, x + gl_width + top_left_skew);

        quad.top_left.x = surface_gl_translate_x(surface, x + top_left_skew);
    } else if (rotation != 0) {
        float centre_x = (gl_width - 1) / 2.0f;
        float centre_y = (gl_height - 1) / 2.0f;
        float angle = TABLE_TO_RADIANS(-rotation, 512);

#ifdef RENDER_GL
        float points[][4] = {
            {0, gl_height},        /* bottom left */
            {gl_width, gl_height}, /* bottom right */
            {gl_width, 0},         /* top right */
            {0, 0},                /* top left */
        };

        for (int i = 0; i < 4; i++) {
            gl_quad_vertex *vertex = ((gl_quad_vertex *)(&quad) + i);

            gl_vertex_apply_rotation(&points[i][0], &points[i][1], centre_x,
                                     centre_y, angle);

            vertex->x = surface_gl_translate_x(surface, x + points[i][0]);
            vertex->y = surface_gl_translate_y(surface, y + points[i][1]);
        }
#elif defined(RENDER_3DS_GL)
        float points[][4] = {
            {gl_width, gl_height}, /* bottom right */
            {0, gl_height},        /* bottom left */
            {0, 0},                /* top left */
            {gl_width, 0},         /* top right */
        };

        for (int i = 0; i < 4; i++) {
            gl_quad_vertex *vertex = ((gl_quad_vertex *)(&quad) + i);

            gl_vertex_apply_rotation(&points[i][0], &points[i][1], centre_x,
                                     centre_y, angle);

            vertex->x = x + points[i][0];
            vertex->y = 240 - y - points[i][1];
        }
#endif
    }

    surface_gl_quad_apply_atlas(&quad, atlas_position, flip);
    surface_gl_quad_apply_base_atlas(&quad, base_atlas_position, flip);

    /* bald head sprites - TODO magic #s */
    if (sprite_id >= 189 && sprite_id <= 216) {
        mask_colour = skin_colour;
    }

    if (mask_colour == 0) {
        mask_colour = WHITE;
    }

    surface_gl_vertex_apply_colour((gl_quad_vertex *)(&quad), 4, mask_colour,
                                   alpha);

    surface_gl_vertex_apply_depth((gl_quad_vertex *)(&quad), 2, depth_bottom);
    surface_gl_vertex_apply_depth((gl_quad_vertex *)(&quad) + 2, 2, depth_top);

    surface_gl_buffer_quad(surface, &quad, texture, base_texture);
}

void surface_gl_buffer_circle(Surface *surface, int x, int y, int radius,
                              int colour, int alpha, float depth) {
    int diameter = radius * 2;

    x -= radius;
    y -= radius;

    gl_quad quad = {0};

    surface_gl_quad_new(surface, &quad, x, y, diameter, diameter);

    surface_gl_quad_apply_atlas(&quad, gl_circle_atlas_position, 0);
    surface_gl_quad_apply_base_atlas(&quad, gl_transparent_atlas_position, 0);

    surface_gl_vertex_apply_colour((gl_quad_vertex *)(&quad), 4, colour, alpha);
    surface_gl_vertex_apply_depth((gl_quad_vertex *)(&quad), 4, depth);

#ifdef RENDER_GL
    surface_gl_buffer_quad(surface, &quad, surface->gl_sprite_texture,
                           surface->gl_sprite_texture);
#elif defined(RENDER_3DS_GL)
    surface_gl_buffer_quad(surface, &quad, &surface->gl_sprite_texture,
                           &surface->gl_sprite_texture);
#endif
}

void surface_gl_buffer_gradient(Surface *surface, int x, int y, int width,
                                int height, int top_colour, int bottom_colour) {
    gl_quad quad = {0};

    surface_gl_quad_new(surface, &quad, x, y, width, height);
    surface_gl_quad_apply_atlas(&quad, gl_white_atlas_position, 0);
    surface_gl_quad_apply_base_atlas(&quad, gl_transparent_atlas_position, 0);

    surface_gl_vertex_apply_colour((gl_quad_vertex *)(&quad), 2, bottom_colour,
                                   255);

    surface_gl_vertex_apply_colour((gl_quad_vertex *)(&quad) + 2, 2, top_colour,
                                   255);

#ifdef RENDER_GL
    surface_gl_buffer_quad(surface, &quad, surface->gl_sprite_texture,
                           surface->gl_sprite_texture);
#elif defined(RENDER_3DS_GL)
    surface_gl_buffer_quad(surface, &quad, &surface->gl_sprite_texture,
                           &surface->gl_sprite_texture);
#endif
}

void surface_gl_draw(Surface *surface, GL_DEPTH_MODE depth_mode) {
#ifdef RENDER_GL
    glEnable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);

    shader_use(&surface->gl_flat_shader);

    vertex_buffer_gl_bind(&surface->gl_flat_buffer);

    int drawn_quads = 0;

    GLuint last_texture = 0;

    for (int i = 0; i < surface->gl_context_count; i++) {
        SurfaceGlContext *context = &surface->gl_contexts[i];

        if (context->quad_count <= 0) {
            continue;
        }

        /* don't apply UI scaling to entities that are drawn within the world */
        int is_ui_scaled =
            context->use_depth ? 0 : mudclient_is_ui_scaled(surface->mud);

        int min_y = context->min_y * (is_ui_scaled + 1);
        int max_y = context->max_y * (is_ui_scaled + 1);
        int min_x = context->min_x * (is_ui_scaled + 1);
        int max_x = context->max_x * (is_ui_scaled + 1);

        int bounds_width = max_x - min_x;
        int bounds_height = max_y - min_y;

        glScissor(min_x, surface->mud->game_height - min_y - bounds_height,
                  bounds_width, bounds_height);

        GLuint texture = context->texture;

        if (texture != last_texture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            last_texture = texture;
        }

        GLuint base_texture = context->base_texture;

        if (base_texture != last_base_texture) {
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, base_texture);

            last_base_texture = base_texture;
        }

        int quad_count = context->quad_count;

        glDrawElements(GL_TRIANGLES, quad_count * 6, GL_UNSIGNED_INT,
                       (void *)(drawn_quads * 6 * sizeof(GLuint)));

        drawn_quads += quad_count;
    }

    glDisable(GL_SCISSOR_TEST);
#elif defined(RENDER_3DS_GL)
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
#endif

    if (depth_mode == GL_DEPTH_BOTH) {
        surface_gl_reset_context(surface);
    }
}

void surface_gl_blur_texture(Surface *surface, int sprite_id, int blur_height,
                             int x, int y, int height) {
#ifdef RENDER_GL
    int offset_x = MINIMAP_SPRITE_WIDTH;

    int offset_y = (sprite_id - surface->mud->sprite_logo) *
                   surface->sprite_height[surface->mud->sprite_logo];

    uint8_t *texture_data = surface->gl_dynamic_texture_buffer;
#elif defined(RENDER_3DS_GL)
    int offset_x = 0;
    int offset_y = 0;

    if (!surface_3ds_gl_get_sprite_texture_offsets(surface, sprite_id,
                                                   &offset_x, &offset_y)) {
        return;
    }

    uint16_t *texture_data = (uint16_t *)surface->gl_sprite_texture.data;
#endif

    for (int xx = x; xx < x + surface->sprite_width[sprite_id]; xx++) {
        for (int yy = y; yy < y + height; yy++) {
            int r = 0;
            int g = 0;
            int b = 0;
            int a = 0;

            for (int x2 = xx; x2 <= xx; x2++) {
                if (x2 >= 0 && x2 < surface->sprite_width[sprite_id]) {
                    for (int y2 = yy - blur_height; y2 <= yy + blur_height;
                         y2++) {
                        if (y2 >= 0 && y2 < surface->sprite_height[sprite_id]) {
#ifdef RENDER_GL
                            int texture_offset =
                                (((offset_y + y2) * 1024) + (offset_x + x2)) *
                                3;

                            r += texture_data[texture_offset];
                            g += texture_data[texture_offset + 1];
                            b += texture_data[texture_offset + 2];
#elif defined(RENDER_3DS_GL)
                            int32_t pixel = _3ds_gl_rgba5551_to_rgb32(
                                texture_data[_3ds_gl_translate_texture_index(
                                                 x2 + offset_x, y2 + offset_y,
                                                 1024) /
                                             2]);

                            r += (pixel >> 16) & 0xff;
                            g += (pixel >> 8) & 0xff;
                            b += pixel & 0xff;
#endif
                            a++;
                        }
                    }
                }
            }

#ifdef RENDER_GL
            int texture_offset =
                (((offset_y + yy) * 1024) + (offset_x + xx)) * 3;

            texture_data[texture_offset] = r / a;
            texture_data[texture_offset + 1] = g / a;
            texture_data[texture_offset + 2] = b / a;
#elif defined(RENDER_3DS_GL)
            int texture_offset = _3ds_gl_translate_texture_index(
                                     xx + offset_x, yy + offset_y, 1024) /
                                 2;

            texture_data[texture_offset] = _3ds_gl_rgb32_to_rgba5551(
                ((r / a) << 16) + ((g / a) << 8) + (b / a));

#endif
        }
    }

#ifdef RENDER_GL
    surface_gl_update_dynamic_texture(surface);
#endif
}

void surface_gl_apply_login_filter(Surface *surface, int sprite_id) {
    for (int i = 6; i >= 1; i--) {
        surface_gl_blur_texture(surface, sprite_id, i, 0, i, 8);
    }

    int sprite_height = surface->sprite_height[sprite_id];

    for (int i = 6; i >= 1; i--) {
        surface_gl_blur_texture(surface, sprite_id, i, 0, sprite_height - 6 - i,
                                8);
    }
}

#endif

#ifndef RENDER_3DS_GL
void surface_gl_raster_to_sprite(Surface *surface, int sprite_id, int x,
                                 int y, int width, int height) {
    (void)x;
    (void)y;

    glReadPixels(0, 0, surface->mud->game_width, surface->mud->game_height,
                 GL_RGBA, GL_UNSIGNED_BYTE, surface->gl_screen_pixels);

    int offset_y = (sprite_id - surface->mud->sprite_logo) * height;
    int offset_x = MINIMAP_SPRITE_WIDTH;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            uint32_t colour =
                surface
                    ->gl_screen_pixels[x + (surface->mud->game_height - y - 1) *
                                               surface->mud->game_width];

            int texture_offset = ((offset_y + y) * 1024 + (offset_x + x)) * 3;

            surface->gl_dynamic_texture_buffer[texture_offset + 2] =
                (colour >> 16) & 255;

            surface->gl_dynamic_texture_buffer[texture_offset + 1] =
                (colour >> 8) & 255;

            surface->gl_dynamic_texture_buffer[texture_offset] = colour & 255;
        }
    }

    surface_gl_update_dynamic_texture(surface);
}

void surface_gl_create_framebuffer(Surface *surface) {
    free(surface->gl_screen_pixels);

    surface->gl_screen_pixels = calloc(
        surface->mud->game_width * surface->mud->game_height, sizeof(uint32_t));
}

void surface_gl_update_dynamic_texture(Surface *surface) {
    glBindTexture(GL_TEXTURE_2D, surface->gl_dynamic_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, surface->gl_dynamic_texture_buffer);
}
#endif
