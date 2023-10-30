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
#endif

int an_int_346 = 0;
int an_int_347 = 0;
int an_int_348 = 0;

int8_t *game_fonts[50] = {0};
int character_width[256] = {0};

int32_t *surface_texture_pixels = NULL;

void init_surface_global() {
    memset(game_fonts, '\0', sizeof(game_fonts));

    for (int i = 0; i < 256; i++) {
        int index = 74;

        for (int j = 0; j < CHAR_SET_LENGTH; j++) {
            if (CHAR_SET[j] == (char)i) {
                index = j;
                break;
            }
        }

        character_width[i] = index * 9;
    }

    surface_texture_pixels = calloc(32 * 1024, sizeof(int32_t));
}

void create_font(int8_t *buffer, int id) { game_fonts[id] = buffer; }

void surface_new(Surface *surface, int width, int height, int limit,
                 mudclient *mud) {
    memset(surface, 0, sizeof(Surface));

    surface->limit = limit;
    surface->width = width;
    surface->height = height;
    surface->bounds_max_x = width;
    surface->bounds_max_y = height;

#if defined(RENDER_SW) && (defined(WII) || defined(_3DS))
    surface->pixels = calloc(width * height, sizeof(int32_t));
#endif

#if !defined(WII) && !defined(_3DS)
#ifdef RENDER_SW
    surface->pixels = mud->pixel_surface->pixels;
#elif defined(RENDER_GL)
    // surface->pixels = calloc(width * height, sizeof(int32_t));
#endif
#endif

    surface->surface_pixels = calloc(limit, sizeof(int32_t *));
    surface->sprite_colours = calloc(limit, sizeof(int8_t *));
    surface->sprite_palette = calloc(limit, sizeof(int32_t *));
    surface->sprite_width = calloc(limit, sizeof(int));
    surface->sprite_height = calloc(limit, sizeof(int));
    surface->sprite_width_full = calloc(limit, sizeof(int));
    surface->sprite_height_full = calloc(limit, sizeof(int));
    surface->sprite_translate = calloc(limit, sizeof(int8_t));
    surface->sprite_translate_x = calloc(limit, sizeof(int));
    surface->sprite_translate_y = calloc(limit, sizeof(int));

    surface->mud = mud;

#ifdef RENDER_GL
    surface_gl_create_framebuffer(surface);

#ifdef EMSCRIPTEN
    shader_new(&surface->gl_flat_shader, "./cache/flat.webgl.vs",
               "./cache/flat.webgl.fs");
#elif OPENGL15
    shader_new(&surface->gl_flat_shader, "./cache/flat.gl2.vs", "./cache/flat.gl2.fs");
#elif OPENGL20
    shader_new(&surface->gl_flat_shader, "./cache/flat.gl2.vs", "./cache/flat.gl2.fs");
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

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
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
#endif

#ifdef RENDER_GL
    gl_load_texture(&surface->gl_sprite_texture,
                    "./cache/textures/sprites.png");

    for (int i = 0; i < ENTITY_TEXTURE_LENGTH; i++) {
        char filename[32] = {0};
        sprintf(filename, "./cache/textures/entities_%d.png", i);

        gl_load_texture(&surface->gl_entity_textures[i], filename);
    }

    surface->gl_dynamic_texture_buffer =
        calloc(1024 * 1024 * 3, sizeof(uint8_t));

    gl_create_texture(&surface->gl_dynamic_texture);

    surface_gl_reset_context(surface);
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

    surface_gl_reset_context(surface);
#endif
}

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
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

void surface_gl_quad_new(Surface *surface, gl_quad *quad, int x, int y,
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
        fprintf(stderr, "too many context (texture/boundary) switches!\n");
        return;
    }

    if (surface->gl_flat_count >= GL_MAX_QUADS) {
        fprintf(stderr, "too many quads!\n");
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
    } else if (sprite_id >= surface->mud->sprite_media && sprite_id <= 3166) {
        // TODO magic number ^
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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        GLuint base_texture = context->base_texture;

        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, base_texture);

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

    int offset_y =
        (sprite_id - surface->mud->sprite_logo) * LOGIN_BACKGROUND_HEIGHT;

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

void surface_set_bounds(Surface *surface, int min_x, int min_y, int max_x,
                        int max_y) {
    if (min_x < 0) {
        min_x = 0;
    }

    if (min_y < 0) {
        min_y = 0;
    }

    if (max_x > surface->width) {
        max_x = surface->width;
    }

    if (max_y > surface->height) {
        max_y = surface->height;
    }

    surface->bounds_min_x = min_x;
    surface->bounds_min_y = min_y;
    surface->bounds_max_x = max_x;
    surface->bounds_max_y = max_y;
}

void surface_reset_bounds(Surface *surface) {
    surface->bounds_min_x = 0;
    surface->bounds_min_y = 0;
    surface->bounds_max_x = surface->width;
    surface->bounds_max_y = surface->height;
}

void surface_draw(Surface *surface) {
    mudclient *mud = surface->mud;

#ifdef WII
    uint8_t *fb = mud->framebuffer;
    uint8_t *pixels = (uint8_t *)surface->pixels;
    int index = 0;

    // 64,54

    for (int y = 0; y < surface->height; y++) {
        for (int x = 0; x < surface->width; x += 2) {
            // int index = ((y  * surface->width) + x) * 4;

            int r = pixels[index + 1];
            int g = pixels[index + 2];
            int b = pixels[index + 3];
            int r2 = pixels[index + 5];
            int g2 = pixels[index + 6];
            int b2 = pixels[index + 7];

            index += 8;

            int y1 = RGB2Y(r, g, b);
            int y2 = RGB2Y(r2, g2, b2);
            int u = RGB2U((r + r2) / 2, (g + g2) / 2, (b + b2) / 2);
            int v = RGB2V((r + r2) / 2, (g + g2) / 2, (b + b2) / 2);

            int fb_index =
                (640 * 2 * (GAME_OFFSET_Y + y)) + ((GAME_OFFSET_X + x) * 2);

            fb[fb_index] = y1;
            fb[fb_index + 2] = y2;
            fb[fb_index + 1] = u;
            fb[fb_index + 3] = v;
        }
    }

    if (mud->keyboard_open) {
        draw_keyboard(mud->framebuffer, mud->keyboard_open == 2 ? 1 : 0);
    }

    draw_arrow(mud->framebuffer, mud->last_wii_x, mud->last_wii_y);
    VIDEO_SetNextFramebuffer(mud->framebuffer);
    mud->active_framebuffer ^= 1;
    mud->framebuffer = mud->framebuffers[mud->active_framebuffer];
    VIDEO_Flush();

    if (mud->keyboard_open) {
        VIDEO_WaitVSync();
    }
#elif defined(_3DS)
#ifdef RENDER_SW
    uint8_t *surface_pixels = (uint8_t *)surface->pixels;

    for (int x = 0; x < surface->width; x++) {
        // for (int y = 0; y < 240; y += (mud->surface->interlace ? 2 : 1)) {
        for (int y = 0; y < 240; y++) {
            int framebuffer_index = ((x * 240) + (239 - y)) * 3;
            int pixel_index = ((y * surface->width) + x) * 4;

            memcpy(mud->_3ds_framebuffer_bottom + framebuffer_index,
                   surface_pixels + pixel_index, 3);
        }
    }
    #ifdef SDL12
    SDL_Flip(mud->screen); //Needed?
    #endif
    // gspWaitForVBlank();
#endif
#else
    #ifdef SDL12
        SDL_BlitSurface(mud->pixel_surface, NULL, mud->screen, NULL);
        SDL_Flip(mud->screen);
    #else
    if (mud->window != NULL) {
        SDL_BlitSurface(mud->pixel_surface, NULL, mud->screen, NULL);
        SDL_UpdateWindowSurface(mud->window);
    }
    #endif
#endif

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_draw(surface, GL_DEPTH_BOTH);
#endif
}

void surface_black_screen(Surface *surface) {
#ifdef RENDER_GL
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#elif defined(RENDER_3DS_GL)
    C3D_RenderTargetClear(surface->mud->_3ds_gl_render_target, C3D_CLEAR_ALL,
                          BLACK, 0);
#elif defined(RENDER_SW)
    int area = surface->width * surface->height;

    if (!surface->interlace) {
        memset(surface->pixels, 0, area * sizeof(int32_t));
        return;
    }

    int pixel_index = 0;

    for (int y = -surface->height; y < 0; y += 2) {
        for (int x = -surface->width; x < 0; x++) {
            surface->pixels[pixel_index++] = 0;
        }

        pixel_index += surface->width;
    }
#endif
}

void surface_draw_circle(Surface *surface, int x, int y, int radius, int colour,
                         int alpha, float depth) {
#ifdef RENDER_SW
    int background_alpha = 256 - alpha;
    int red = ((colour >> 16) & 0xff) * alpha;
    int green = ((colour >> 8) & 0xff) * alpha;
    int blue = (colour & 0xff) * alpha;

    int top = y - radius;

    if (top < 0) {
        top = 0;
    }

    int bottom = y + radius;

    if (bottom >= surface->height) {
        bottom = surface->height - 1;
    }

    int8_t y_inc = 1;

    if (surface->interlace) {
        y_inc = 2;

        if ((top & 1) != 0) {
            top++;
        }
    }

    for (int yy = top; yy <= bottom; yy += y_inc) {
        int l3 = yy - y;
        int i4 = (int)sqrt(radius * radius - l3 * l3);
        int j4 = x - i4;

        if (j4 < 0) {
            j4 = 0;
        }

        int k4 = x + i4;

        if (k4 >= surface->width) {
            k4 = surface->width - 1;
        }

        int index = j4 + yy * surface->width;

        for (int i = j4; i <= k4; i++) {
            int pixel = surface->pixels[index];
            int background_red = ((pixel >> 16) & 0xff) * background_alpha;
            int background_green = ((pixel >> 8) & 0xff) * background_alpha;
            int background_blue = (pixel & 0xff) * background_alpha;

            int new_colour = (((red + background_red) >> 8) << 16) +
                             (((green + background_green) >> 8) << 8) +
                             ((blue + background_blue) >> 8);

            surface->pixels[index++] = new_colour;
        }
    }

    (void)depth;
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_buffer_circle(surface, x, y, radius, colour, alpha, depth);
#endif
}

void surface_draw_box_alpha(Surface *surface, int x, int y, int width,
                            int height, int colour, int alpha) {
#ifdef RENDER_SW
    if (x < surface->bounds_min_x) {
        width -= surface->bounds_min_x - x;
        x = surface->bounds_min_x;
    }

    if (y < surface->bounds_min_y) {
        height -= surface->bounds_min_y - y;
        y = surface->bounds_min_y;
    }

    if (x + width > surface->bounds_max_x) {
        width = surface->bounds_max_x - x;
    }

    if (y + height > surface->bounds_max_y) {
        height = surface->bounds_max_y - y;
    }

    int red = ((colour >> 16) & 0xff) * alpha;
    int green = ((colour >> 8) & 0xff) * alpha;
    int blue = (colour & 0xff) * alpha;
    int background_alpha = 256 - alpha;

    int offset = surface->width - width;
    int8_t y_inc = 1;

    if (surface->interlace) {
        y_inc = 2;
        offset += surface->width;

        if ((y & 1) != 0) {
            y++;
            height--;
        }
    }

    int index = x + y * surface->width;

    for (int i = 0; i < height; i += y_inc) {
        for (int j = -width; j < 0; j++) {
            int pixel = surface->pixels[index];
            int background_red = ((pixel >> 16) & 0xff) * background_alpha;
            int background_green = ((pixel >> 8) & 0xff) * background_alpha;
            int background_blue = (pixel & 0xff) * background_alpha;

            int new_colour = (((red + background_red) >> 8) << 16) +
                             (((green + background_green) >> 8) << 8) +
                             ((blue + background_blue) >> 8);

            surface->pixels[index++] = new_colour;
        }

        index += offset;
    }
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_buffer_box(surface, x, y, width, height, colour, alpha);
#endif
}

void surface_draw_gradient(Surface *surface, int x, int y, int width,
                           int height, int top_colour, int bottom_colour) {
#ifdef RENDER_SW
    if (x < surface->bounds_min_x) {
        width -= surface->bounds_min_x - x;
        x = surface->bounds_min_x;
    }

    if (x + width > surface->bounds_max_x) {
        width = surface->bounds_max_x - x;
    }

    int bottom_red = (bottom_colour >> 16) & 0xff;
    int bottom_green = (bottom_colour >> 8) & 0xff;
    int bottom_blue = bottom_colour & 0xff;
    int top_red = (top_colour >> 16) & 0xff;
    int top_green = (top_colour >> 8) & 0xff;
    int top_blue = top_colour & 0xff;

    int offset = surface->width - width;
    int8_t y_inc = 1;

    if (surface->interlace) {
        y_inc = 2;
        offset += surface->width;

        if ((y & 1) != 0) {
            y++;
            height--;
        }
    }

    int index = x + y * surface->width;

    for (int i = 0; i < height; i += y_inc) {
        if (i + y >= surface->bounds_min_y && i + y < surface->bounds_max_y) {
            int new_colour =
                (((bottom_red * i + top_red * (height - i)) / height) << 16) +
                (((bottom_green * i + top_green * (height - i)) / height)
                 << 8) +
                (((bottom_blue * i + top_blue * (height - i)) / height));

            for (int j = -width; j < 0; j++) {
                surface->pixels[index++] = new_colour;
            }

            index += offset;
        } else {
            index += surface->width;
        }
    }
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_buffer_gradient(surface, x, y, width, height, top_colour,
                               bottom_colour);
#endif
}

void surface_draw_box(Surface *surface, int x, int y, int width, int height,
                      int colour) {
#ifdef RENDER_SW
    if (x < surface->bounds_min_x) {
        width -= surface->bounds_min_x - x;
        x = surface->bounds_min_x;
    }

    if (y < surface->bounds_min_y) {
        height -= surface->bounds_min_y - y;
        y = surface->bounds_min_y;
    }

    if (x + width > surface->bounds_max_x) {
        width = surface->bounds_max_x - x;
    }

    if (y + height > surface->bounds_max_y) {
        height = surface->bounds_max_y - y;
    }

    int offset = surface->width - width;
    int8_t y_inc = 1;

    if (surface->interlace) {
        y_inc = 2;
        offset += surface->width;

        if ((y & 1) != 0) {
            y++;
            height--;
        }
    }

    int index = x + y * surface->width;

    for (int i = -height; i < 0; i += y_inc) {
        for (int j = -width; j < 0; j++) {
            surface->pixels[index++] = colour;
        }

        index += offset;
    }
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_buffer_box(surface, x, y, width, height, colour, 255);
#endif
}

void surface_draw_line_horizontal(Surface *surface, int x, int y, int width,
                                  int colour) {
#ifdef RENDER_SW
    if (y < surface->bounds_min_y || y >= surface->bounds_max_y) {
        return;
    }

    if (x < surface->bounds_min_x) {
        width -= surface->bounds_min_x - x;
        x = surface->bounds_min_x;
    }

    if (x + width > surface->bounds_max_x) {
        width = surface->bounds_max_x - x;
    }

    int start = x + y * surface->width;

    for (int i = 0; i < width; i++) {
        surface->pixels[start + i] = colour;
    }
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_buffer_box(surface, x, y, width, 1, colour, 255);
#endif
}

void surface_draw_line_vertical(Surface *surface, int x, int y, int height,
                                int colour) {
#ifdef RENDER_SW
    if (x < surface->bounds_min_x || x >= surface->bounds_max_x) {
        return;
    }

    if (y < surface->bounds_min_y) {
        height -= surface->bounds_min_y - y;
        y = surface->bounds_min_y;
    }

    if (y + height > surface->bounds_max_y) {
        height = surface->bounds_max_y - y;
    }

    int start = x + y * surface->width;

    for (int i = 0; i < height; i++) {
        surface->pixels[start + i * surface->width] = colour;
    }
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_buffer_box(surface, x, y, 1, height, colour, 255);
#endif
}

void surface_draw_border(Surface *surface, int x, int y, int width, int height,
                         int colour) {
    surface_draw_line_horizontal(surface, x, y, width, colour);
    surface_draw_line_horizontal(surface, x, y + height - 1, width, colour);
    surface_draw_line_vertical(surface, x, y, height, colour);
    surface_draw_line_vertical(surface, x + width - 1, y, height, colour);
}

void surface_set_pixel(Surface *surface, int x, int y, int colour) {
#if defined(RENDER_GL) || defined(RENDER_SW)
    if (x < surface->bounds_min_x || y < surface->bounds_min_y ||
        x >= surface->bounds_max_x || y >= surface->bounds_max_y) {
        return;
    }

    surface->pixels[x + y * surface->width] = colour;
#endif
}

void surface_fade_to_black_software(Surface *surface, int32_t *dest,
                                    int add_alpha) {
    int area = surface->mud->game_width * surface->mud->game_height;

    for (int i = 0; i < area; i++) {
        int32_t pixel = dest[i] & 0xffffff;

        dest[i] = ((pixel >> 1) & 0x7f7f7f) + ((pixel >> 2) & 0x3f3f3f) +
                  ((pixel >> 3) & 0x1f1f1f) + ((pixel >> 4) & 0xf0f0f);

        if (add_alpha) {
            dest[i] += 0xff000000;
        }
    }
}

void surface_fade_to_black(Surface *surface) {
#ifdef RENDER_SW
    surface_fade_to_black_software(surface, surface->pixels, 0);
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    // TODO this leaves some sort of residue. https://imgur.com/a/35sqk7v
    surface_gl_buffer_box(surface, 0, 0, surface->width, surface->height, BLACK,
                          16);
#endif
}

void surface_draw_blur_software(Surface *surface, int32_t *dest,
                                int blur_height, int x, int y, int width,
                                int height, int add_alpha) {
    for (int xx = x; xx < x + width; xx++) {
        for (int yy = y; yy < y + height; yy++) {
            int r = 0;
            int g = 0;
            int b = 0;
            int a = 0;

            for (int x2 = xx; x2 <= xx; x2++) {
                if (x2 >= 0 && x2 < surface->width) {
                    for (int y2 = yy - blur_height; y2 <= yy + blur_height;
                         y2++) {
                        if (y2 >= 0 && y2 < surface->height) {
                            int32_t pixel = dest[x2 + surface->width * y2];

                            r += (pixel >> 16) & 0xff;
                            g += (pixel >> 8) & 0xff;
                            b += pixel & 0xff;
                            a++;
                        }
                    }
                }
            }

            dest[xx + surface->width * yy] = ((r / a) << 16) + ((g / a) << 8) +
                                             (b / a) +
                                             (add_alpha ? 0xff000000 : 0);
        }
    }
}

void surface_draw_blur(Surface *surface, int blur_height, int x, int y,
                       int width, int height) {
#ifdef RENDER_SW
    surface_draw_blur_software(surface, surface->pixels, blur_height, x, y,
                               width, height, 1);
#endif
}

void surface_apply_login_filter(Surface *surface, int background_height) {
    surface_fade_to_black(surface);
    surface_fade_to_black(surface);

    surface_draw_box(surface, 0, 0, surface->width, 6, BLACK);

#ifdef RENDER_SW
    for (int i = 6; i >= 1; i--) {
        surface_draw_blur(surface, i, 0, i, surface->width, 8);
    }
#endif

    surface_draw_box(surface, 0, background_height - 6, surface->width, 20,
                     BLACK);

#ifdef RENDER_SW
    for (int i = 6; i >= 1; i--) {
        surface_draw_blur(surface, i, 0, background_height - 6 - i,
                          surface->width, 8);
    }
#endif
}

// TODO remove this
void surface_clear(Surface *surface) {
    for (int i = 0; i < surface->limit; i++) {
        free(surface->surface_pixels[i]);

        surface->surface_pixels[i] = NULL;
        surface->sprite_width[i] = 0;
        surface->sprite_height[i] = 0;
    }
}

void surface_parse_sprite(Surface *surface, int sprite_id, int8_t *sprite_data,
                          int8_t *index_data, int frame_count) {
    /* FIXME: unsafe unchecked access */

    int index_offset = get_unsigned_short(sprite_data, 0, SIZE_MAX);

    int full_width = get_unsigned_short(index_data, index_offset, SIZE_MAX);
    index_offset += 2;

    int full_height = get_unsigned_short(index_data, index_offset, SIZE_MAX);
    index_offset += 2;

    int colour_count = index_data[index_offset++] & 0xff;

#ifdef RENDER_SW
    int32_t *colours = calloc(colour_count, sizeof(int32_t));
    colours[0] = MAGENTA;
#endif

    for (int i = 0; i < colour_count - 1; i++) {
#ifdef RENDER_SW
        int colour = ((index_data[index_offset] & 0xff) << 16) +
                     ((index_data[index_offset + 1] & 0xff) << 8) +
                     (index_data[index_offset + 2] & 0xff);

        colours[i + 1] = colour;
        // colours[i + 1] = GREEN;
#endif

        index_offset += 3;
    }

#ifdef RENDER_SW
    int sprite_offset = 2;
#endif

    for (int i = sprite_id; i < sprite_id + frame_count; i++) {
        surface->sprite_translate_x[i] = index_data[index_offset++] & 0xff;
        surface->sprite_translate_y[i] = index_data[index_offset++] & 0xff;

        surface->sprite_width[i] =
            get_unsigned_short(index_data, index_offset, SIZE_MAX);
        index_offset += 2;

        surface->sprite_height[i] =
            get_unsigned_short(index_data, index_offset, SIZE_MAX);

        index_offset += 2;

#ifdef RENDER_SW
        int type = index_data[index_offset++] & 0xff;
        int area = surface->sprite_width[i] * surface->sprite_height[i];

        surface->sprite_colours[i] = calloc(area, sizeof(int8_t));
        surface->sprite_palette[i] = colours;
#else
        index_offset++;
#endif
        surface->sprite_width_full[i] = full_width;
        surface->sprite_height_full[i] = full_height;

        free(surface->surface_pixels[i]);
        surface->surface_pixels[i] = NULL;

        surface->sprite_translate[i] = 0;

        if (surface->sprite_translate_x[i] != 0 ||
            surface->sprite_translate_y[i] != 0) {
            surface->sprite_translate[i] = 1;
        }

#ifdef RENDER_SW
        if (type == 0) {
            for (int j = 0; j < area; j++) {
                surface->sprite_colours[i][j] = sprite_data[sprite_offset++];

                if (surface->sprite_colours[i][j] == 0) {
                    surface->sprite_translate[i] = 1;
                }
            }
        } else if (type == 1) {
            for (int x = 0; x < surface->sprite_width[i]; x++) {
                for (int y = 0; y < surface->sprite_height[i]; y++) {
                    int index = x + y * surface->sprite_width[i];

                    surface->sprite_colours[i][index] =
                        sprite_data[sprite_offset++];

                    if (surface->sprite_colours[i][index] == 0) {
                        surface->sprite_translate[i] = 1;
                    }
                }
            }
        }
#endif
    }

    free(sprite_data);
}

void surface_read_sleep_word(Surface *surface, int sprite_id,
                             int8_t *sprite_data) {
    if (surface->surface_pixels[sprite_id] == NULL) {
        surface->surface_pixels[sprite_id] =
            malloc(SLEEP_WIDTH * SLEEP_HEIGHT * sizeof(int32_t));
    }

    int32_t *pixels = surface->surface_pixels[sprite_id];

    surface->sprite_width[sprite_id] = SLEEP_WIDTH;
    surface->sprite_height[sprite_id] = SLEEP_HEIGHT;
    surface->sprite_translate_x[sprite_id] = 0;
    surface->sprite_translate_y[sprite_id] = 0;
    surface->sprite_width_full[sprite_id] = SLEEP_WIDTH;
    surface->sprite_height_full[sprite_id] = SLEEP_HEIGHT;
    surface->sprite_translate[sprite_id] = 0;

    int colour = 0;
    int packet_offset = 1;
    int pixel_index = 0;

    for (pixel_index = 0; pixel_index < SLEEP_WIDTH;) {
        int length = sprite_data[packet_offset++] & 0xff;

        for (int i = 0; i < length; i++) {
            pixels[pixel_index++] = colour;
        }

        /* alternate between black and white */
        colour = WHITE - colour;
    }

    for (int y = 1; y < SLEEP_HEIGHT; y++) {
        for (int x = 0; x < SLEEP_WIDTH;) {
            int length = sprite_data[packet_offset++] & 0xff;

            for (int i = 0; i < length; i++) {
                pixels[pixel_index] = pixels[pixel_index - SLEEP_WIDTH];
                pixel_index++;
                x++;
            }

            if (x < SLEEP_WIDTH) {
                pixels[pixel_index] = WHITE - pixels[pixel_index - SLEEP_WIDTH];

                pixel_index++;
                x++;
            }
        }
    }

#ifdef RENDER_GL
    pixel_index = 0;

    for (int y = 0; y < SLEEP_HEIGHT; y++) {
        for (int x = 0; x < SLEEP_WIDTH; x++) {
            int offset = (y * 1024 + x) * 3;

            if (pixels[pixel_index]) {
                surface->gl_dynamic_texture_buffer[offset] = 255;
                surface->gl_dynamic_texture_buffer[offset + 1] = 255;
                surface->gl_dynamic_texture_buffer[offset + 2] = 255;
            } else {
                surface->gl_dynamic_texture_buffer[offset] = 0;
                surface->gl_dynamic_texture_buffer[offset + 1] = 0;
                surface->gl_dynamic_texture_buffer[offset + 2] = 0;
            }

            pixel_index++;
        }
    }

    surface_gl_update_dynamic_texture(surface);
#elif defined(RENDER_3DS_GL)
    int offset_x = 0;
    int offset_y = 0;

    if (!surface_3ds_gl_get_sprite_texture_offsets(surface, sprite_id,
                                                   &offset_x, &offset_y)) {
        return;
    }

    uint16_t *texture_data = (uint16_t *)surface->gl_sprite_texture.data;

    pixel_index = 0;

    for (int y = 0; y < SLEEP_HEIGHT; y++) {
        for (int x = 0; x < SLEEP_WIDTH; x++) {
            int offset = _3ds_gl_translate_texture_index(x + offset_x,
                                                         y + offset_y, 1024) /
                         sizeof(uint16_t);

            if (pixels[pixel_index]) {
                texture_data[offset] = 65535;
            } else {
                texture_data[offset] = 1;
            }

            pixel_index++;
        }
    }
#endif
}

void surface_screen_raster_to_palette_sprite(Surface *surface, int sprite_id) {
    int sprite_size =
        surface->sprite_width[sprite_id] * surface->sprite_height[sprite_id];

    int32_t *sprite_pixels = surface->surface_pixels[sprite_id];

    for (int i = 0; i < sprite_size; i++) {
        int colour = sprite_pixels[i];

        surface_texture_pixels[((colour & 0xf80000) >> 9) +
                               ((colour & 0xf800) >> 6) +
                               ((colour & 0xf8) >> 3)]++;
    }

    int32_t *palette = calloc(256, sizeof(int32_t));
    palette[0] = MAGENTA;

    int32_t ai3[256] = {0};

    for (int i = 0; i < 32768; i++) {
        int32_t pixel = surface_texture_pixels[i];

        if (pixel > ai3[255]) {
            for (int j = 1; j < 256; j++) {
                if (pixel <= ai3[j]) {
                    continue;
                }

                for (int k = 255; k > j; k--) {
                    palette[k] = palette[k - 1];
                    ai3[k] = ai3[k - 1];
                }

                palette[j] = ((i & 0x7c00) << 9) + ((i & 0x3e0) << 6) +
                             ((i & 0x1f) << 3) + 0x40404;

                ai3[j] = pixel;
                break;
            }
        }

        surface_texture_pixels[i] = -1;
    }

    int8_t *colours = calloc(sprite_size, sizeof(int8_t));

    for (int i = 0; i < sprite_size; i++) {
        int pixel_colour = sprite_pixels[i];

        int pixel_index = ((pixel_colour & 0xf80000) >> 9) +
                          ((pixel_colour & 0xf800) >> 6) +
                          ((pixel_colour & 0xf8) >> 3);

        int texture_pixel = surface_texture_pixels[pixel_index];

        if (texture_pixel == -1) {
            int max_colour = 999999999;
            int r = (pixel_colour >> 16) & 0xff;
            int g = (pixel_colour >> 8) & 0xff;
            int b = pixel_colour & 0xff;

            for (int j = 0; j < 256; j++) {
                int palette_colour = palette[j];
                int palette_r = (palette_colour >> 16) & 0xff;
                int palette_g = (palette_colour >> 8) & 0xff;
                int palette_b = palette_colour & 0xff;

                int j5 = (r - palette_r) * (r - palette_r) +
                         (g - palette_g) * (g - palette_g) +
                         (b - palette_b) * (b - palette_b);

                if (j5 < max_colour) {
                    max_colour = j5;
                    texture_pixel = j;
                }
            }

            surface_texture_pixels[pixel_index] = texture_pixel;
        }

        colours[i] = texture_pixel & 0xff;
    }

    surface->sprite_colours[sprite_id] = colours;
    surface->sprite_palette[sprite_id] = palette;

    free(surface->surface_pixels[sprite_id]);
    surface->surface_pixels[sprite_id] = NULL;
}

int32_t *surface_palette_sprite_to_raster(Surface *surface, int sprite_id,
                                          int add_alpha) {
    int8_t *colours = surface->sprite_colours[sprite_id];

    if (colours == NULL) {
        return NULL;
    }

    int area =
        surface->sprite_width[sprite_id] * surface->sprite_height[sprite_id];

    int32_t *palette = surface->sprite_palette[sprite_id];
    int32_t *pixels = calloc(area, sizeof(int32_t));

    for (int i = 0; i < area; i++) {
        int32_t colour = palette[colours[i] & 0xff];

        if (colour == 0) {
            colour = 1;
        } else if (colour == MAGENTA) {
            colour = 0;
        }

        /* add 255 alpha for opengl textures */
        if (add_alpha && colour != 0) {
            colour += 0xff000000;
        }

        pixels[i] = colour;
    }

    return pixels;
}

void surface_load_sprite(Surface *surface, int sprite_id) {
#ifdef RENDER_SW
    surface->surface_pixels[sprite_id] =
        surface_palette_sprite_to_raster(surface, sprite_id, 0);
#endif

    free(surface->sprite_colours[sprite_id]);
    surface->sprite_colours[sprite_id] = NULL;
}

void surface_screen_raster_to_sprite(Surface *surface, int sprite_id, int x,
                                     int y, int width, int height) {
    surface->sprite_width[sprite_id] = width;
    surface->sprite_height[sprite_id] = height;
    surface->sprite_translate[sprite_id] = 0;
    surface->sprite_translate_x[sprite_id] = 0;
    surface->sprite_translate_y[sprite_id] = 0;
    surface->sprite_width_full[sprite_id] = width;
    surface->sprite_height_full[sprite_id] = height;

#ifdef RENDER_SW
    free(surface->surface_pixels[sprite_id]);

    surface->surface_pixels[sprite_id] =
        calloc(width * height, sizeof(int32_t));

    int pixel_index = 0;

    for (int yy = y; yy < y + height; yy++) {
        for (int xx = x; xx < x + width; xx++) {
            surface->surface_pixels[sprite_id][pixel_index++] =
                surface->pixels[xx + yy * surface->width];
        }
    }

    free(surface->sprite_colours[sprite_id]);
    surface->sprite_colours[sprite_id] = NULL;

    free(surface->sprite_palette[sprite_id]);
    surface->sprite_palette[sprite_id] = NULL;
#elif defined(RENDER_GL)
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
#elif defined(RENDER_3DS_GL)
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
#endif
}

// TODO not draw - load from raster reversed
void surface_draw_sprite_reversed(Surface *surface, int sprite_id, int x, int y,
                                  int width, int height) {
    surface->sprite_width[sprite_id] = width;
    surface->sprite_height[sprite_id] = height;
    surface->sprite_translate[sprite_id] = 0;
    surface->sprite_translate_x[sprite_id] = 0;
    surface->sprite_translate_y[sprite_id] = 0;
    surface->sprite_width_full[sprite_id] = width;
    surface->sprite_height_full[sprite_id] = height;

#if defined(RENDER_SW)
    free(surface->surface_pixels[sprite_id]);

    surface->surface_pixels[sprite_id] =
        calloc(width * height, sizeof(int32_t));

    int index = 0;

    for (int xx = x; xx < x + width; xx++) {
        for (int yy = y; yy < y + height; yy++) {
            int colour = surface->pixels[xx + yy * surface->width];
            surface->surface_pixels[sprite_id][index++] = colour;
        }
    }

    free(surface->sprite_colours[sprite_id]);
    surface->sprite_colours[sprite_id] = NULL;

    free(surface->sprite_palette[sprite_id]);
    surface->sprite_palette[sprite_id] = NULL;
#endif
}

void surface_draw_sprite(Surface *surface, int x, int y, int sprite_id) {
#ifdef RENDER_SW
    if (surface->sprite_translate[sprite_id] != 0) {
        x += surface->sprite_translate_x[sprite_id];
        y += surface->sprite_translate_y[sprite_id];
    }

    int dest_pos = x + y * surface->width;
    int src_pos = 0;
    int width = surface->sprite_width[sprite_id];
    int height = surface->sprite_height[sprite_id];
    int dest_offset = surface->width - width;
    int sprite_offset = 0;

    if (y < surface->bounds_min_y) {
        int clip_y = surface->bounds_min_y - y;
        height -= clip_y;
        y = surface->bounds_min_y;
        src_pos += clip_y * width;
        dest_pos += clip_y * surface->width;
    }

    if (y + height >= surface->bounds_max_y) {
        height -= y + height - surface->bounds_max_y + 1;
    }

    if (x < surface->bounds_min_x) {
        int clip_x = surface->bounds_min_x - x;
        width -= clip_x;
        x = surface->bounds_min_x;
        src_pos += clip_x;
        dest_pos += clip_x;
        sprite_offset += clip_x;
        dest_offset += clip_x;
    }

    if (x + width >= surface->bounds_max_x) {
        int clip_x = x + width - surface->bounds_max_x + 1;
        width -= clip_x;
        sprite_offset += clip_x;
        dest_offset += clip_x;
    }

    if (width <= 0 || height <= 0) {
        return;
    }

    int8_t y_inc = 1;

    if (surface->interlace) {
        y_inc = 2;
        dest_offset += surface->width;
        sprite_offset += surface->sprite_width[sprite_id];

        if ((y & 1) != 0) {
            dest_pos += surface->width;
            height--;
        }
    }

    if (surface->surface_pixels[sprite_id] == NULL) {
        surface_plot_palette_sprite(
            surface->pixels, surface->sprite_colours[sprite_id],
            surface->sprite_palette[sprite_id], src_pos, dest_pos, width,
            height, dest_offset, sprite_offset, y_inc);
    } else {
        surface_plot_sprite(surface->pixels, surface->surface_pixels[sprite_id],
                            src_pos, dest_pos, width, height, dest_offset,
                            sprite_offset, y_inc);
    }
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_buffer_sprite(surface, sprite_id, x, y, -1, -1, 0, 0, 0, 255, 0,
                             0, 0, 0);
#endif
}

void surface_draw_sprite_depth(Surface *surface, int x, int y, int sprite_id,
                               float depth_top, float depth_bottom) {
#ifdef RENDER_SW
    surface_draw_sprite(surface, x, y, sprite_id);

    (void)depth_top;
    (void)depth_bottom;
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_buffer_sprite(surface, sprite_id, x, y, -1, -1, 0, 0, 0, 255, 0,
                             0, depth_top, depth_bottom);
#endif
}

void surface_draw_sprite_scale(Surface *surface, int x, int y, int width,
                               int height, int sprite_id, float depth) {
#ifdef RENDER_SW
    int sprite_width = surface->sprite_width[sprite_id];
    int sprite_height = surface->sprite_height[sprite_id];
    int l1 = 0;
    int i2 = 0;
    int j2 = (sprite_width << 16) / width;
    int k2 = (sprite_height << 16) / height;

    if (surface->sprite_translate[sprite_id]) {
        int l2 = surface->sprite_width_full[sprite_id];
        int j3 = surface->sprite_height_full[sprite_id];
        j2 = (l2 << 16) / width;
        k2 = (j3 << 16) / height;

        x += (surface->sprite_translate_x[sprite_id] * width + l2 - 1) / l2;
        y += (surface->sprite_translate_y[sprite_id] * height + j3 - 1) / j3;

        if ((surface->sprite_translate_x[sprite_id] * width) % l2 != 0) {
            l1 = ((l2 - ((surface->sprite_translate_x[sprite_id] * width) % l2))
                  << 16) /
                 width;
        }

        if ((surface->sprite_translate_y[sprite_id] * height) % j3 != 0) {
            i2 =
                ((j3 - ((surface->sprite_translate_y[sprite_id] * height) % j3))
                 << 16) /
                height;
        }

        width = (width * (surface->sprite_width[sprite_id] - (l1 >> 16))) / l2;

        height =
            (height * (surface->sprite_height[sprite_id] - (i2 >> 16))) / j3;
    }

    int dest_pos = x + y * surface->width;
    int k3 = surface->width - width;

    if (y < surface->bounds_min_y) {
        int l3 = surface->bounds_min_y - y;
        height -= l3;
        y = 0;
        dest_pos += l3 * surface->width;
        i2 += k2 * l3;
    }

    if (y + height >= surface->bounds_max_y) {
        height -= y + height - surface->bounds_max_y + 1;
    }

    if (x < surface->bounds_min_x) {
        int i4 = surface->bounds_min_x - x;
        width -= i4;
        x = 0;
        dest_pos += i4;
        l1 += j2 * i4;
        k3 += i4;
    }

    if (x + width >= surface->bounds_max_x) {
        int j4 = x + width - surface->bounds_max_x + 1;
        width -= j4;
        k3 += j4;
    }

    int8_t y_inc = 1;

    if (surface->interlace) {
        y_inc = 2;
        k3 += surface->width;
        k2 += k2;

        if ((y & 1) != 0) {
            dest_pos += surface->width;
            height--;
        }
    }

    surface_plot_scale_from13(
        surface->pixels, surface->surface_pixels[sprite_id], l1, i2, dest_pos,
        k3, width, height, j2, k2, sprite_width, y_inc);

    (void)depth;
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_buffer_sprite(surface, sprite_id, x, y, width, height, 0, 0, 0,
                             255, 0, 0, depth, depth);
#endif
}

void surface_draw_entity_sprite(Surface *surface, int x, int y, int width,
                                int height, int sprite_id, int tx, int ty,
                                float depth_top, float depth_bottom) {
    if (sprite_id >= 50000) {
        mudclient_draw_teleport_bubble(surface->mud, x, y, width, height,
                                       sprite_id - 50000,
                                       (depth_top + depth_bottom) / 2.0f);

        return;
    }

    if (sprite_id >= 40000) {
        mudclient_draw_ground_item(surface->mud, x, y, width, height,
                                   sprite_id - 40000, depth_top, depth_bottom);
        return;
    }

    if (sprite_id >= 20000) {
        mudclient_draw_npc(surface->mud, x, y, width, height, sprite_id - 20000,
                           tx, ty, depth_top, depth_bottom);
        return;
    }

    if (sprite_id >= 5000) {
        mudclient_draw_player(surface->mud, x, y, width, height,
                              sprite_id - 5000, tx, ty, depth_top,
                              depth_bottom);
        return;
    }

    surface_draw_sprite_scale(surface, x, y, width, height, sprite_id,
                              (depth_top + depth_bottom) / 2);
}

void surface_draw_sprite_alpha(Surface *surface, int x, int y, int sprite_id,
                               int alpha) {
#ifdef RENDER_SW
    if (surface->sprite_translate[sprite_id]) {
        x += surface->sprite_translate_x[sprite_id];
        y += surface->sprite_translate_y[sprite_id];
    }

    int size = x + y * surface->width;
    int src_pos = 0;
    int height = surface->sprite_height[sprite_id];
    int width = surface->sprite_width[sprite_id];
    int dest_offset = surface->width - width;
    int src_offset = 0;

    if (y < surface->bounds_min_y) {
        int clip_y = surface->bounds_min_y - y;
        height -= clip_y;
        y = surface->bounds_min_y;
        src_pos += clip_y * width;
        size += clip_y * surface->width;
    }

    if (y + height >= surface->bounds_max_y) {
        height -= y + height - surface->bounds_max_y + 1;
    }

    if (x < surface->bounds_min_x) {
        int clip_x = surface->bounds_min_x - x;
        width -= clip_x;
        x = surface->bounds_min_x;
        src_pos += clip_x;
        size += clip_x;
        src_offset += clip_x;
        dest_offset += clip_x;
    }

    if (x + width >= surface->bounds_max_x) {
        int clip_x = x + width - surface->bounds_max_x + 1;
        width -= clip_x;
        src_offset += clip_x;
        dest_offset += clip_x;
    }

    if (width <= 0 || height <= 0) {
        return;
    }

    int8_t y_inc = 1;

    if (surface->interlace) {
        y_inc = 2;
        dest_offset += surface->width;
        src_offset += surface->sprite_width[sprite_id];

        if ((y & 1) != 0) {
            size += surface->width;
            height--;
        }
    }

    if (surface->surface_pixels[sprite_id] == NULL) {
        surface_draw_sprite_alpha_from11a(
            surface->pixels, surface->sprite_colours[sprite_id],
            surface->sprite_palette[sprite_id], src_pos, size, width, height,
            dest_offset, src_offset, y_inc, alpha);
    } else {
        surface_draw_sprite_alpha_from11(
            surface->pixels, surface->surface_pixels[sprite_id], src_pos, size,
            width, height, dest_offset, src_offset, y_inc, alpha);
    }
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_buffer_sprite(surface, sprite_id, x, y, -1, -1, 0, 0, 0, alpha,
                             0, 0, 0, 0);
#endif
}

void surface_draw_action_bubble(Surface *surface, int x, int y, int scale_x,
                                int scale_y, int sprite_id, int alpha) {
#ifdef RENDER_SW
    int sprite_width = surface->sprite_width[sprite_id];
    int sprite_height = surface->sprite_height[sprite_id];
    int i2 = 0;
    int j2 = 0;
    int k2 = (sprite_width << 16) / scale_x;
    int l2 = (sprite_height << 16) / scale_y;

    if (surface->sprite_translate[sprite_id]) {
        int width_full = surface->sprite_width_full[sprite_id];
        int height_full = surface->sprite_height_full[sprite_id];

        k2 = (width_full << 16) / scale_x;
        l2 = (height_full << 16) / scale_y;

        x += (surface->sprite_translate_x[sprite_id] * scale_x + width_full -
              1) /
             width_full;

        y += (surface->sprite_translate_y[sprite_id] * scale_y + height_full -
              1) /
             height_full;

        if ((surface->sprite_translate_x[sprite_id] * scale_x) % width_full !=
            0) {
            i2 = ((width_full -
                   ((surface->sprite_translate_x[sprite_id] * scale_x) %
                    width_full))
                  << 16) /
                 scale_x;
        }

        if ((surface->sprite_translate_y[sprite_id] * scale_y) % height_full !=
            0) {
            j2 = ((height_full -
                   ((surface->sprite_translate_y[sprite_id] * scale_y) %
                    height_full))
                  << 16) /
                 scale_y;
        }

        scale_x = (scale_x * (surface->sprite_width[sprite_id] - (i2 >> 16))) /
                  width_full;

        scale_y = (scale_y * (surface->sprite_height[sprite_id] - (j2 >> 16))) /
                  height_full;
    }

    int j3 = x + y * surface->width;
    int l3 = surface->width - scale_x;

    if (y < surface->bounds_min_y) {
        int i4 = surface->bounds_min_y - y;
        scale_y -= i4;
        y = 0;
        j3 += i4 * surface->width;
        j2 += l2 * i4;
    }

    if (y + scale_y >= surface->bounds_max_y)
        scale_y -= y + scale_y - surface->bounds_max_y + 1;

    if (x < surface->bounds_min_x) {
        int j4 = surface->bounds_min_x - x;
        scale_x -= j4;
        x = 0;
        j3 += j4;
        i2 += k2 * j4;
        l3 += j4;
    }

    if (x + scale_x >= surface->bounds_max_x) {
        int k4 = x + scale_x - surface->bounds_max_x + 1;
        scale_x -= k4;
        l3 += k4;
    }

    int8_t y_inc = 1;

    if (surface->interlace) {
        y_inc = 2;
        l3 += surface->width;
        l2 += l2;

        if ((y & 1) != 0) {
            j3 += surface->width;
            scale_y--;
        }
    }

    surface_transparent_scale(
        surface->pixels, surface->surface_pixels[sprite_id], i2, j2, j3, l3,
        scale_x, scale_y, k2, l2, sprite_width, y_inc, alpha);
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_buffer_sprite(surface, sprite_id, x, y, scale_x, scale_y, 0, 0,
                             0, alpha, 0, 0, 0, 0);
#endif
}

/* only works with palette sprites */
void surface_draw_sprite_scale_mask(Surface *surface, int x, int y, int width,
                                    int height, int sprite_id, int colour) {
#ifdef RENDER_SW
    int sprite_width = surface->sprite_width[sprite_id];
    int sprite_height = surface->sprite_height[sprite_id];
    int i2 = 0;
    int j2 = 0;
    int k2 = (sprite_width << 16) / width;
    int l2 = (sprite_height << 16) / height;

    if (surface->sprite_translate[sprite_id]) {
        int width_full = surface->sprite_width_full[sprite_id];
        int height_full = surface->sprite_height_full[sprite_id];

        k2 = (width_full << 16) / width;
        l2 = (height_full << 16) / height;

        x += (surface->sprite_translate_x[sprite_id] * width + width_full - 1) /
             width_full;

        y += (surface->sprite_translate_y[sprite_id] * height + height_full -
              1) /
             height_full;

        if ((surface->sprite_translate_x[sprite_id] * width) % width_full !=
            0) {
            i2 = ((width_full -
                   ((surface->sprite_translate_x[sprite_id] * width) %
                    width_full))
                  << 16) /
                 width;
        }

        if ((surface->sprite_translate_y[sprite_id] * height) % height_full !=
            0) {
            j2 = ((height_full -
                   ((surface->sprite_translate_y[sprite_id] * height) %
                    height_full))
                  << 16) /
                 height;
        }

        width = (width * (surface->sprite_width[sprite_id] - (i2 >> 16))) /
                width_full;

        height = (height * (surface->sprite_height[sprite_id] - (j2 >> 16))) /
                 height_full;
    }

    int j3 = x + y * surface->width;
    int l3 = surface->width - width;

    if (y < surface->bounds_min_y) {
        int i4 = surface->bounds_min_y - y;
        height -= i4;
        y = 0;
        j3 += i4 * surface->width;
        j2 += l2 * i4;
    }

    if (y + height >= surface->bounds_max_y) {
        height -= y + height - surface->bounds_max_y + 1;
    }

    if (x < surface->bounds_min_x) {
        int j4 = surface->bounds_min_x - x;
        width -= j4;
        x = 0;
        j3 += j4;
        i2 += k2 * j4;
        l3 += j4;
    }

    if (x + width >= surface->bounds_max_x) {
        int k4 = x + width - surface->bounds_max_x + 1;
        width -= k4;
        l3 += k4;
    }

    int8_t y_inc = 1;

    if (surface->interlace) {
        y_inc = 2;
        l3 += surface->width;
        l2 += l2;

        if ((y & 1) != 0) {
            j3 += surface->width;
            height--;
        }
    }

    surface_plot_scale_from14(
        surface->pixels, surface->surface_pixels[sprite_id], i2, j2, j3, l3,
        width, height, k2, l2, sprite_width, y_inc, colour);
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_buffer_sprite(surface, sprite_id, x, y, width, height, 0, colour,
                             0, 255, 0, 0, 0, 0);
#endif
}

void surface_plot_sprite(int32_t *dest, int32_t *src, int src_pos, int dest_pos,
                         int width, int height, int dest_offset, int src_offset,
                         int y_inc) {
    for (int y = 0 - height; y < 0; y += y_inc) {
        for (int x = -width; x < 0; x++) {
            int colour = src[src_pos++];

            if (colour != 0) {
                dest[dest_pos++] = colour;
            } else {
                dest_pos++;
            }
        }

        dest_pos += dest_offset;
        src_pos += src_offset;
    }
}

void surface_plot_palette_sprite(int32_t *dest, int8_t *colours,
                                 int32_t *palette, int src_pos, int dest_pos,
                                 int width, int height, int dest_offset,
                                 int src_offset, int y_inc) {
    for (int y = 0 - height; y < 0; y += y_inc) {
        for (int x = -width; x < 0; x++) {
            int8_t index = colours[src_pos++];

            if (index != 0) {
                dest[dest_pos++] = palette[index & 0xff];
            } else {
                dest_pos++;
            }
        }

        dest_pos += dest_offset;
        src_pos += src_offset;
    }
}

void surface_plot_scale_from13(int32_t *dest, int32_t *src, int j, int k,
                               int dest_pos, int dest_offset, int width,
                               int height, int l1, int i2, int j2, int y_inc) {
    int l2 = j;

    for (int y = -height; y < 0; y += y_inc) {
        int j3 = (k >> 16) * j2;

        for (int x = -width; x < 0; x++) {
            int colour = src[(j >> 16) + j3];

            if (colour != 0) {
                dest[dest_pos++] = colour;
            } else {
                dest_pos++;
            }

            j += l1;
        }

        k += i2;
        j = l2;
        dest_pos += dest_offset;
    }
}

int surface_blend_alpha(int background_colour, int colour, int alpha) {
    int background_alpha = 256 - alpha;

    return ((((colour & 0xff00ff) * alpha +
              (background_colour & 0xff00ff) * background_alpha) &
             -0xff0100) +
            (((colour & 0x00ff00) * alpha +
              (background_colour & 0x00ff00) * background_alpha) &
             0xff0000)) >>
           8;
}

void surface_draw_sprite_alpha_from11(int32_t *dest, int32_t *src, int src_pos,
                                      int dest_pos, int width, int height,
                                      int dest_offset, int src_offset,
                                      int y_inc, int alpha) {
    for (int y = -height; y < 0; y += y_inc) {
        for (int x = -width; x < 0; x++) {
            int colour = src[src_pos++];

            if (colour != 0) {
                int background_colour = dest[dest_pos];

                dest[dest_pos++] =
                    surface_blend_alpha(background_colour, colour, alpha);
            } else {
                dest_pos++;
            }
        }

        dest_pos += dest_offset;
        src_pos += src_offset;
    }
}

void surface_draw_sprite_alpha_from11a(int32_t *dest, int8_t *colours,
                                       int32_t *palette, int src_pos,
                                       int dest_pos, int width, int height,
                                       int dest_offset, int src_offset,
                                       int y_inc, int alpha) {
    for (int y = -height; y < 0; y += y_inc) {
        for (int x = -width; x < 0; x++) {
            int colour = colours[src_pos++];

            if (colour != 0) {
                colour = palette[colour & 0xff];

                int background_colour = dest[dest_pos];

                dest[dest_pos++] =
                    surface_blend_alpha(background_colour, colour, alpha);
            } else {
                dest_pos++;
            }
        }

        dest_pos += dest_offset;
        src_pos += src_offset;
    }
}

void surface_transparent_scale(int32_t *dest, int32_t *src, int j, int k,
                               int dest_pos, int dest_offset, int width,
                               int height, int l1, int i2, int j2, int y_inc,
                               int alpha) {
    int j3 = j;

    for (int y = -height; y < 0; y += y_inc) {
        int l3 = (k >> 16) * j2;

        for (int x = -width; x < 0; x++) {
            int colour = src[(j >> 16) + l3];

            if (colour != 0) {
                int background_colour = dest[dest_pos];

                dest[dest_pos++] =
                    surface_blend_alpha(background_colour, colour, alpha);
            } else {
                dest_pos++;
            }

            j += l1;
        }

        k += i2;
        j = j3;
        dest_pos += dest_offset;
    }
}

void surface_plot_scale_from14(int32_t *dest, int32_t *src, int j, int k,
                               int dest_pos, int dest_offset, int width,
                               int height, int l1, int i2, int j2, int y_inc,
                               int mask_colour) {
    int mask_r = (mask_colour >> 16) & 0xff;
    int mask_g = (mask_colour >> 8) & 0xff;
    int mask_b = mask_colour & 0xff;
    int a = j;

    for (int y = -height; y < 0; y += y_inc) {
        int j4 = (k >> 16) * j2;

        for (int x = -width; x < 0; x++) {
            int colour = src[(j >> 16) + j4];

            if (colour != 0) {
                int r = (colour >> 16) & 0xff;
                int g = (colour >> 8) & 0xff;
                int b = colour & 0xff;

                if (r == g && g == b) {
                    dest[dest_pos++] = (((r * mask_r) >> 8) << 16) +
                                       (((g * mask_g) >> 8) << 8) +
                                       ((b * mask_b) >> 8);
                } else {
                    dest[dest_pos++] = colour;
                }
            } else {
                dest_pos++;
            }

            j += l1;
        }

        k += i2;
        j = a;
        dest_pos += dest_offset;
    }
}

void surface_draw_minimap_sprite(Surface *surface, int x, int y, int sprite_id,
                                 int rotation, int scale) {
    rotation &= 255;

#ifdef RENDER_SW
    int j1 = surface->width;
    int k1 = surface->height;
    int i2 = -(surface->sprite_width_full[sprite_id] / 2);
    int j2 = -(surface->sprite_height_full[sprite_id] / 2);

    if (surface->sprite_translate[sprite_id]) {
        i2 += surface->sprite_translate_x[sprite_id];
        j2 += surface->sprite_translate_y[sprite_id];
    }

    int k2 = i2 + surface->sprite_width[sprite_id];
    int l2 = j2 + surface->sprite_height[sprite_id];
    int i3 = k2;
    int j3 = j2;
    int k3 = i2;
    int l3 = l2;

    int i4 = sin_cos_512[rotation] * scale;
    int j4 = sin_cos_512[rotation + 256] * scale;
    int k4 = x + ((j2 * i4 + i2 * j4) >> 22);
    int l4 = y + ((j2 * j4 - i2 * i4) >> 22);
    int i5 = x + ((j3 * i4 + i3 * j4) >> 22);
    int j5 = y + ((j3 * j4 - i3 * i4) >> 22);
    int k5 = x + ((l2 * i4 + k2 * j4) >> 22);
    int l5 = y + ((l2 * j4 - k2 * i4) >> 22);
    int i6 = x + ((l3 * i4 + k3 * j4) >> 22);
    int j6 = y + ((l3 * j4 - k3 * i4) >> 22);

    if (scale == 192 && (rotation & 0x3f) == (an_int_348 & 0x3f)) {
        an_int_346++;
    } else if (scale == 128) {
        an_int_348 = rotation;
    } else {
        an_int_347++;
    }

    int k6 = l4;
    int l6 = l4;

    if (j5 < k6) {
        k6 = j5;
    } else if (j5 > l6) {
        l6 = j5;
    }

    if (l5 < k6) {
        k6 = l5;
    } else if (l5 > l6) {
        l6 = l5;
    }

    if (j6 < k6) {
        k6 = j6;
    } else if (j6 > l6) {
        l6 = j6;
    }

    if (k6 < surface->bounds_min_y) {
        k6 = surface->bounds_min_y;
    }

    if (l6 > surface->bounds_max_y) {
        l6 = surface->bounds_max_y;
    }

    if (surface->rotations_0 == NULL || surface->rotations_length != k1 + 1) {
        free(surface->rotations_0);
        free(surface->rotations_1);
        free(surface->rotations_2);
        free(surface->rotations_3);
        free(surface->rotations_4);
        free(surface->rotations_5);

        surface->rotations_0 = calloc((k1 + 1), sizeof(int));
        surface->rotations_1 = calloc((k1 + 1), sizeof(int));
        surface->rotations_2 = calloc((k1 + 1), sizeof(int));
        surface->rotations_3 = calloc((k1 + 1), sizeof(int));
        surface->rotations_4 = calloc((k1 + 1), sizeof(int));
        surface->rotations_5 = calloc((k1 + 1), sizeof(int));

        surface->rotations_length = k1 + 1;
    }

    for (int i7 = k6; i7 <= l6; i7++) {
        surface->rotations_0[i7] = 99999999;
        surface->rotations_1[i7] = -99999999;
    }

    int i8 = 0;
    int k8 = 0;
    int i9 = 0;
    int sprite_width = surface->sprite_width[sprite_id];
    int sprite_height = surface->sprite_height[sprite_id];

    i2 = 0;
    j2 = 0;
    i3 = sprite_width - 1;
    j3 = 0;
    k2 = sprite_width - 1;
    l2 = sprite_height - 1;
    k3 = 0;
    l3 = sprite_height - 1;

    if (j6 != l4) {
        i8 = ((i6 - k4) << 8) / (j6 - l4);
        i9 = ((l3 - j2) << 8) / (j6 - l4);
    }

    int j7 = 0;
    int k7 = 0;
    int l7 = 0;
    int l8 = 0;

    if (l4 > j6) {
        l7 = i6 << 8;
        l8 = l3 << 8;
        j7 = j6;
        k7 = l4;
    } else {
        l7 = k4 << 8;
        l8 = j2 << 8;
        j7 = l4;
        k7 = j6;
    }

    if (j7 < 0) {
        l7 -= i8 * j7;
        l8 -= i9 * j7;
        j7 = 0;
    }

    if (k7 > k1 - 1) {
        k7 = k1 - 1;
    }

    for (int i = j7; i <= k7; i++) {
        surface->rotations_0[i] = l7;
        surface->rotations_1[i] = l7;

        l7 += i8;

        surface->rotations_2[i] = 0;
        surface->rotations_3[i] = 0;
        surface->rotations_4[i] = l8;
        surface->rotations_5[i] = l8;

        l8 += i9;
    }

    if (j5 != l4) {
        i8 = ((i5 - k4) << 8) / (j5 - l4);
        k8 = ((i3 - i2) << 8) / (j5 - l4);
    }

    int j8 = 0;

    if (l4 > j5) {
        l7 = i5 << 8;
        j8 = i3 << 8;
        j7 = j5;
        k7 = l4;
    } else {
        l7 = k4 << 8;
        j8 = i2 << 8;
        j7 = l4;
        k7 = j5;
    }

    if (j7 < 0) {
        l7 -= i8 * j7;
        j8 -= k8 * j7;
        j7 = 0;
    }

    if (k7 > k1 - 1) {
        k7 = k1 - 1;
    }

    for (int i = j7; i <= k7; i++) {
        if (l7 < surface->rotations_0[i]) {
            surface->rotations_0[i] = l7;
            surface->rotations_2[i] = j8;
            surface->rotations_4[i] = 0;
        }

        if (l7 > surface->rotations_1[i]) {
            surface->rotations_1[i] = l7;
            surface->rotations_3[i] = j8;
            surface->rotations_5[i] = 0;
        }

        l7 += i8;
        j8 += k8;
    }

    if (l5 != j5) {
        i8 = ((k5 - i5) << 8) / (l5 - j5);
        i9 = ((l2 - j3) << 8) / (l5 - j5);
    }

    if (j5 > l5) {
        l7 = k5 << 8;
        j8 = k2 << 8;
        l8 = l2 << 8;
        j7 = l5;
        k7 = j5;
    } else {
        l7 = i5 << 8;
        j8 = i3 << 8;
        l8 = j3 << 8;
        j7 = j5;
        k7 = l5;
    }

    if (j7 < 0) {
        l7 -= i8 * j7;
        l8 -= i9 * j7;
        j7 = 0;
    }

    if (k7 > k1 - 1) {
        k7 = k1 - 1;
    }

    for (int i = j7; i <= k7; i++) {
        if (l7 < surface->rotations_0[i]) {
            surface->rotations_0[i] = l7;
            surface->rotations_2[i] = j8;
            surface->rotations_4[i] = l8;
        }

        if (l7 > surface->rotations_1[i]) {
            surface->rotations_1[i] = l7;
            surface->rotations_3[i] = j8;
            surface->rotations_5[i] = l8;
        }

        l7 += i8;
        l8 += i9;
    }

    if (j6 != l5) {
        i8 = ((i6 - k5) << 8) / (j6 - l5);
        k8 = ((k3 - k2) << 8) / (j6 - l5);
    }

    if (l5 > j6) {
        l7 = i6 << 8;
        j8 = k3 << 8;
        l8 = l3 << 8;
        j7 = j6;
        k7 = l5;
    } else {
        l7 = k5 << 8;
        j8 = k2 << 8;
        l8 = l2 << 8;
        j7 = l5;
        k7 = j6;
    }

    if (j7 < 0) {
        l7 -= i8 * j7;
        j8 -= k8 * j7;
        j7 = 0;
    }

    if (k7 > k1 - 1) {
        k7 = k1 - 1;
    }

    for (int i = j7; i <= k7; i++) {
        if (l7 < surface->rotations_0[i]) {
            surface->rotations_0[i] = l7;
            surface->rotations_2[i] = j8;
            surface->rotations_4[i] = l8;
        }

        if (l7 > surface->rotations_1[i]) {
            surface->rotations_1[i] = l7;
            surface->rotations_3[i] = j8;
            surface->rotations_5[i] = l8;
        }

        l7 += i8;
        j8 += k8;
    }

    int l10 = k6 * j1;
    int32_t *ai = surface->surface_pixels[sprite_id];

    for (int i = k6; i < l6; i++) {
        int j11 = surface->rotations_0[i] >> 8;
        int k11 = surface->rotations_1[i] >> 8;

        if (k11 - j11 <= 0) {
            l10 += j1;
        } else {
            int l11 = surface->rotations_2[i] << 9;
            int i12 = ((surface->rotations_3[i] << 9) - l11) / (k11 - j11);
            int j12 = surface->rotations_4[i] << 9;
            int k12 = ((surface->rotations_5[i] << 9) - j12) / (k11 - j11);

            if (j11 < surface->bounds_min_x) {
                l11 += (surface->bounds_min_x - j11) * i12;
                j12 += (surface->bounds_min_x - j11) * k12;
                j11 = surface->bounds_min_x;
            }

            if (k11 > surface->bounds_max_x) {
                k11 = surface->bounds_max_x;
            }

            if (!surface->interlace || (i & 1) == 0) {
                if (!surface->sprite_translate[sprite_id]) {
                    surface_draw_minimap(surface->pixels, ai, l10 + j11, l11,
                                         j12, i12, k12, j11 - k11,
                                         sprite_width);
                } else {
                    surface_draw_minimap_translate(
                        surface->pixels, ai, l10 + j11, l11, j12, i12, k12,
                        j11 - k11, sprite_width);
                }
            }

            l10 += j1;
        }
    }
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    // TODO still *slightly* off
    int gl_sprite_width = surface->sprite_width[sprite_id];
    int gl_sprite_height = surface->sprite_height[sprite_id];

    float gl_scale = scale / 128.0f;

    int draw_width = (int)(gl_sprite_width * gl_scale);
    int draw_height = (int)(gl_sprite_height * gl_scale);

    int gl_x = x - (int)((gl_sprite_width / 2) * gl_scale);
    int gl_y = y - (int)((gl_sprite_height / 2) * gl_scale);

    surface_gl_buffer_sprite(surface, sprite_id, gl_x, gl_y, draw_width,
                             draw_height, 0, 0, 0, 255, 0, rotation, 0, 0);
#endif
}

void surface_draw_minimap(int32_t *dest, int32_t *src, int dest_pos, int k,
                          int l, int i1, int j1, int k1, int l1) {
    for (int i = k1; i < 0; i++) {
        dest[dest_pos++] = src[(k >> 17) + (l >> 17) * l1];
        k += i1;
        l += j1;
    }
}

void surface_draw_minimap_translate(int32_t *dest, int32_t *src, int dest_pos,
                                    int k, int l, int i1, int j1, int k1,
                                    int l1) {
    for (int i2 = k1; i2 < 0; i2++) {
        int colour = src[(k >> 17) + (l >> 17) * l1];

        if (colour != 0) {
            dest[dest_pos++] = colour;
        } else {
            dest_pos++;
        }

        k += i1;
        l += j1;
    }
}

void surface_draw_sprite_transform_mask_software(
    Surface *surface, int x, int y, int draw_width, int draw_height,
    int sprite_id, int mask_colour, int skin_colour, int skew_x, int flip) {
    if (mask_colour == 0) {
        mask_colour = WHITE;
    }

    if (skin_colour == 0) {
        skin_colour = WHITE;
    }

    int sprite_width = surface->sprite_width[sprite_id];
    int sprite_height = surface->sprite_height[sprite_id];
    int offset_x = 0;
    int offset_y = 0;
    int i3 = skew_x << 16;
    int width_ratio = (sprite_width << 16) / draw_width;
    int height_ratio = (sprite_height << 16) / draw_height;
    int l3 = -((skew_x << 16) / draw_height);

    if (surface->sprite_translate[sprite_id]) {
        int full_width = surface->sprite_width_full[sprite_id];
        int full_height = surface->sprite_height_full[sprite_id];

        width_ratio = (full_width << 16) / draw_width;
        height_ratio = (full_height << 16) / draw_height;

        int translate_x = surface->sprite_translate_x[sprite_id];
        int translate_y = surface->sprite_translate_y[sprite_id];

        if (flip) {
            translate_x =
                full_width - surface->sprite_width[sprite_id] - translate_x;
        }

        x += (translate_x * draw_width + full_width - 1) / full_width;

        int l5 = (translate_y * draw_height + full_height - 1) / full_height;

        y += l5;
        i3 += l5 * l3;

        if ((translate_x * draw_width) % full_width != 0) {
            offset_x = ((full_width - ((translate_x * draw_width) % full_width))
                        << 16) /
                       draw_width;
        }

        if ((translate_y * draw_height) % full_height != 0) {
            offset_y =
                ((full_height - ((translate_y * draw_height) % full_height))
                 << 16) /
                draw_height;
        }

        draw_width = ((surface->sprite_width[sprite_id] << 16) - offset_x +
                      width_ratio - 1) /
                     width_ratio;

        draw_height = ((surface->sprite_height[sprite_id] << 16) - offset_y +
                       height_ratio - 1) /
                      height_ratio;
    }

    int j4 = y * surface->width;
    i3 += x << 16;

    if (y < surface->bounds_min_y) {
        int clip_y = surface->bounds_min_y - y;
        draw_height -= clip_y;
        y = surface->bounds_min_y;
        j4 += clip_y * surface->width;
        offset_y += height_ratio * clip_y;
        i3 += l3 * clip_y;
    }

    if (y + draw_height >= surface->bounds_max_y) {
        draw_height -= y + draw_height - surface->bounds_max_y + 1;
    }

    int y_inc = (j4 / surface->width) & 1;

    if (!surface->interlace) {
        y_inc = 2;
    }

    if (skin_colour == WHITE) {
        if (surface->surface_pixels[sprite_id] != NULL) {
            if (!flip) {
                surface_transparent_sprite_plot_from15(
                    surface, surface->pixels,
                    surface->surface_pixels[sprite_id], offset_x, offset_y, j4,
                    draw_width, draw_height, width_ratio, height_ratio,
                    sprite_width, mask_colour, i3, l3, y_inc);
            } else {
                surface_transparent_sprite_plot_from15(
                    surface, surface->pixels,
                    surface->surface_pixels[sprite_id],
                    (surface->sprite_width[sprite_id] << 16) - offset_x - 1,
                    offset_y, j4, draw_width, draw_height, -width_ratio,
                    height_ratio, sprite_width, mask_colour, i3, l3, y_inc);
            }
        } else {
            if (!flip) {
                surface_transparent_sprite_plot_from16a(
                    surface, surface->pixels,
                    surface->sprite_colours[sprite_id],
                    surface->sprite_palette[sprite_id], offset_x, offset_y, j4,
                    draw_width, draw_height, width_ratio, height_ratio,
                    sprite_width, mask_colour, i3, l3, y_inc);
            } else {
                surface_transparent_sprite_plot_from16a(
                    surface, surface->pixels,
                    surface->sprite_colours[sprite_id],
                    surface->sprite_palette[sprite_id],
                    (surface->sprite_width[sprite_id] << 16) - offset_x - 1,
                    offset_y, j4, draw_width, draw_height, -width_ratio,
                    height_ratio, sprite_width, mask_colour, i3, l3, y_inc);
            }
        }
    } else {
        if (surface->surface_pixels[sprite_id] != NULL) {
            if (!flip) {
                surface_transparent_sprite_plot_from16(
                    surface, surface->pixels,
                    surface->surface_pixels[sprite_id], offset_x, offset_y, j4,
                    draw_width, draw_height, width_ratio, height_ratio,
                    sprite_width, mask_colour, skin_colour, i3, l3, y_inc);

            } else {
                surface_transparent_sprite_plot_from16(
                    surface, surface->pixels,
                    surface->surface_pixels[sprite_id],
                    (surface->sprite_width[sprite_id] << 16) - offset_x - 1,
                    offset_y, j4, draw_width, draw_height, -width_ratio,
                    height_ratio, sprite_width, mask_colour, skin_colour, i3,
                    l3, y_inc);
            }
        } else {
            if (!flip) {
                surface_transparent_sprite_plot_from17(
                    surface, surface->pixels,
                    surface->sprite_colours[sprite_id],
                    surface->sprite_palette[sprite_id], offset_x, offset_y, j4,
                    draw_width, draw_height, width_ratio, height_ratio,
                    sprite_width, mask_colour, skin_colour, i3, l3, y_inc);
            } else {
                surface_transparent_sprite_plot_from17(
                    surface, surface->pixels,
                    surface->sprite_colours[sprite_id],
                    surface->sprite_palette[sprite_id],
                    (surface->sprite_width[sprite_id] << 16) - offset_x - 1,
                    offset_y, j4, draw_width, draw_height, -width_ratio,
                    height_ratio, sprite_width, mask_colour, skin_colour, i3,
                    l3, y_inc);
            }
        }
    }
}

/* applies scale, both grey and skin colour masks, skew/shear and flip. used
 * for entity sprites */
void surface_draw_sprite_transform_mask(Surface *surface, int x, int y,
                                        int draw_width, int draw_height,
                                        int sprite_id, int mask_colour,
                                        int skin_colour, int skew_x, int flip) {
#ifdef RENDER_SW
    surface_draw_sprite_transform_mask_software(
        surface, x, y, draw_width, draw_height, sprite_id, mask_colour,
        skin_colour, skew_x, flip);
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_buffer_sprite(surface, sprite_id, x, y, draw_width, draw_height,
                             skew_x, mask_colour, skin_colour, 255, flip, 0, 0,
                             0);
#endif
}

void surface_draw_sprite_transform_mask_depth(Surface *surface, int x, int y,
                                              int draw_width, int draw_height,
                                              int sprite_id, int mask_colour,
                                              int skin_colour, int skew_x,
                                              int flip, float depth_top,
                                              float depth_bottom) {
#ifdef RENDER_SW
    surface_draw_sprite_transform_mask_software(
        surface, x, y, draw_width, draw_height, sprite_id, mask_colour,
        skin_colour, skew_x, flip);

    (void)depth_top;
    (void)depth_bottom;
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
    surface_gl_buffer_sprite(surface, sprite_id, x, y, draw_width, draw_height,
                             skew_x, mask_colour, skin_colour, 255, flip, 0,
                             depth_top, depth_bottom);
#endif
}

void surface_transparent_sprite_plot_from15(Surface *surface, int32_t *dest,
                                            int32_t *src, int j, int k,
                                            int dest_pos, int width, int height,
                                            int k1, int l1, int i2,
                                            int mask_colour, int k2, int l2,
                                            int y_inc) {
    int mask_r = (mask_colour >> 16) & 0xff;
    int mask_g = (mask_colour >> 8) & 0xff;
    int mask_b = mask_colour & 0xff;
    int l4 = j;

    for (int y = -height; y < 0; y++) {
        int j5 = (k >> 16) * i2;
        int x_offset = k2 >> 16;
        int final_width = width;

        if (x_offset < surface->bounds_min_x) {
            int i6 = surface->bounds_min_x - x_offset;
            final_width -= i6;
            x_offset = surface->bounds_min_x;
            j += k1 * i6;
        }

        if (x_offset + final_width >= surface->bounds_max_x) {
            int j6 = x_offset + final_width - surface->bounds_max_x;
            final_width -= j6;
        }

        y_inc = 1 - y_inc;

        if (y_inc != 0) {
            for (int x = x_offset; x < x_offset + final_width; x++) {
                int colour = src[(j >> 16) + j5];

                if (colour != 0) {
                    int r = (colour >> 16) & 0xff;
                    int g = (colour >> 8) & 0xff;
                    int b = colour & 0xff;

                    if (r == g && g == b) {
                        dest[x + dest_pos] = (((r * mask_r) >> 8) << 16) +
                                             (((g * mask_g) >> 8) << 8) +
                                             ((b * mask_b) >> 8);
                    } else {
                        dest[x + dest_pos] = colour;
                    }
                }

                j += k1;
            }
        }

        k += l1;
        j = l4;
        dest_pos += surface->width;
        k2 += l2;
    }
}

void surface_transparent_sprite_plot_from16(Surface *surface, int32_t *dest,
                                            int32_t *src, int j, int k,
                                            int dest_pos, int i1, int j1,
                                            int k1, int l1, int i2,
                                            int mask_colour, int skin_colour,
                                            int l2, int i3, int j3) {
    int mask_r = (mask_colour >> 16) & 0xff;
    int mask_g = (mask_colour >> 8) & 0xff;
    int mask_b = mask_colour & 0xff;
    int skin_r = (skin_colour >> 16) & 0xff;
    int skin_g = (skin_colour >> 8) & 0xff;
    int skin_b = skin_colour & 0xff;
    int l5 = j;

    for (int i6 = -j1; i6 < 0; i6++) {
        int j6 = (k >> 16) * i2;
        int k6 = l2 >> 16;
        int l6 = i1;

        if (k6 < surface->bounds_min_x) {
            int i7 = surface->bounds_min_x - k6;
            l6 -= i7;
            k6 = surface->bounds_min_x;
            j += k1 * i7;
        }

        if (k6 + l6 >= surface->bounds_max_x) {
            int j7 = k6 + l6 - surface->bounds_max_x;
            l6 -= j7;
        }

        j3 = 1 - j3;

        if (j3 != 0) {
            for (int k7 = k6; k7 < k6 + l6; k7++) {
                int colour = src[(j >> 16) + j6];

                if (colour != 0) {
                    int r = (colour >> 16) & 0xff;
                    int g = (colour >> 8) & 0xff;
                    int b = colour & 0xff;

                    if (r == g && g == b) {
                        dest[k7 + dest_pos] = (((r * mask_r) >> 8) << 16) +
                                              (((g * mask_g) >> 8) << 8) +
                                              ((b * mask_b) >> 8);
                    } else if (r == 255 && g == b) {
                        dest[k7 + dest_pos] = (((r * skin_r) >> 8) << 16) +
                                              (((g * skin_g) >> 8) << 8) +
                                              ((b * skin_b) >> 8);
                    } else {
                        dest[k7 + dest_pos] = colour;
                    }
                }

                j += k1;
            }
        }

        k += l1;
        j = l5;
        dest_pos += surface->width;
        l2 += i3;
    }
}

void surface_transparent_sprite_plot_from16a(Surface *surface, int32_t *dest,
                                             int8_t *colour_idx,
                                             int32_t *colours, int j, int k,
                                             int l, int i1, int height, int k1,
                                             int l1, int i2, int mask_colour,
                                             int k2, int l2, int i3) {
    int mask_r = (mask_colour >> 16) & 0xff;
    int mask_g = (mask_colour >> 8) & 0xff;
    int mask_b = mask_colour & 0xff;
    int l4 = j;

    for (int y = -height; y < 0; y++) {
        int j5 = (k >> 16) * i2;
        int k5 = k2 >> 16;
        int l5 = i1;

        if (k5 < surface->bounds_min_x) {
            int i6 = surface->bounds_min_x - k5;
            l5 -= i6;
            k5 = surface->bounds_min_x;
            j += k1 * i6;
        }

        if (k5 + l5 >= surface->bounds_max_x) {
            int j6 = k5 + l5 - surface->bounds_max_x;
            l5 -= j6;
        }

        i3 = 1 - i3;

        if (i3 != 0) {
            for (int k6 = k5; k6 < k5 + l5; k6++) {
                int colour = colour_idx[(j >> 16) + j5] & 0xff;

                if (colour != 0) {
                    colour = colours[colour];

                    int r = (colour >> 16) & 0xff;
                    int g = (colour >> 8) & 0xff;
                    int b = colour & 0xff;

                    if (r == g && g == b) {
                        dest[k6 + l] = (((r * mask_r) >> 8) << 16) +
                                       (((g * mask_g) >> 8) << 8) +
                                       ((b * mask_b) >> 8);
                    } else {
                        dest[k6 + l] = colour;
                    }
                }

                j += k1;
            }
        }

        k += l1;
        j = l4;
        l += surface->width;
        k2 += l2;
    }
}

void surface_transparent_sprite_plot_from17(Surface *surface, int32_t *dest,
                                            int8_t *colous, int32_t *palette,
                                            int j, int k, int l, int i1,
                                            int height, int k1, int l1, int i2,
                                            int mask_colour, int skin_colour,
                                            int l2, int i3, int j3) {
    int mask_r = (mask_colour >> 16) & 0xff;
    int mask_g = (mask_colour >> 8) & 0xff;
    int mask_b = mask_colour & 0xff;
    int skin_r = (skin_colour >> 16) & 0xff;
    int skin_g = (skin_colour >> 8) & 0xff;
    int skin_b = skin_colour & 0xff;
    int l5 = j;

    for (int y = -height; y < 0; y++) {
        int j6 = (k >> 16) * i2;
        int k6 = l2 >> 16;
        int l6 = i1;

        if (k6 < surface->bounds_min_x) {
            int i7 = surface->bounds_min_x - k6;
            l6 -= i7;
            k6 = surface->bounds_min_x;
            j += k1 * i7;
        }

        if (k6 + l6 >= surface->bounds_max_x) {
            int j7 = k6 + l6 - surface->bounds_max_x;
            l6 -= j7;
        }

        j3 = 1 - j3;

        if (j3 != 0) {
            for (int k7 = k6; k7 < k6 + l6; k7++) {
                int colour = colous[(j >> 16) + j6] & 0xff;

                if (colour != 0) {
                    colour = palette[colour];

                    int r = (colour >> 16) & 0xff;
                    int g = (colour >> 8) & 0xff;
                    int b = colour & 0xff;

                    if (r == g && g == b) {
                        dest[k7 + l] = (((r * mask_r) >> 8) << 16) +
                                       (((g * mask_g) >> 8) << 8) +
                                       ((b * mask_b) >> 8);
                    } else if (r == 255 && g == b) {
                        dest[k7 + l] = (((r * skin_r) >> 8) << 16) +
                                       (((g * skin_g) >> 8) << 8) +
                                       ((b * skin_b) >> 8);
                    } else {
                        dest[k7 + l] = colour;
                    }
                }

                j += k1;
            }
        }

        k += l1;
        j = l5;
        l += surface->width;
        l2 += i3;
    }
}

#ifdef RENDER_SW
void surface_plot_letter(int32_t *dest, int8_t *font_data, int colour,
                         int font_pos, int dest_pos, int width, int height,
                         int dest_offset, int font_data_offset) {
    for (int y = -height; y < 0; y++) {
        for (int x = -width; x < 0; x++) {
            if (font_data[font_pos++] != 0) {
                dest[dest_pos++] = colour;
            } else {
                dest_pos++;
            }
        }

        dest_pos += dest_offset;
        font_pos += font_data_offset;
    }
}

void surface_draw_character(Surface *surface, int character_offset, int x,
                            int y, int colour, int8_t *font_data) {
    /* baseline and kerning offsets */
    int draw_x = x + font_data[character_offset + 5];
    int draw_y = y - font_data[character_offset + 6];

    int width = font_data[character_offset + 3];
    int height = font_data[character_offset + 4];

    /* position of pixel data for the font (on/off) */
    int font_pos = font_data[character_offset] * (128 * 128) +
                   font_data[character_offset + 1] * 128 +
                   font_data[character_offset + 2];

    int dest_pos = draw_x + draw_y * surface->width;
    int dest_offset = surface->width - width;
    int font_data_offset = 0;

    if (draw_y < surface->bounds_min_y) {
        int clip_y = surface->bounds_min_y - draw_y;
        height -= clip_y;
        draw_y = surface->bounds_min_y;
        font_pos += clip_y * width;
        dest_pos += clip_y * surface->width;
    }

    if (draw_y + height >= surface->bounds_max_y) {
        height -= draw_y + height - surface->bounds_max_y + 1;
    }

    if (draw_x < surface->bounds_min_x) {
        int clip_x = surface->bounds_min_x - draw_x;
        width -= clip_x;
        draw_x = surface->bounds_min_x;
        font_pos += clip_x;
        dest_pos += clip_x;
        font_data_offset += clip_x;
        dest_offset += clip_x;
    }

    if (draw_x + width >= surface->bounds_max_x) {
        int clip_x = draw_x + width - surface->bounds_max_x + 1;
        width -= clip_x;
        font_data_offset += clip_x;
        dest_offset += clip_x;
    }

    if (width > 0 && height > 0) {
        surface_plot_letter(surface->pixels, font_data, colour, font_pos,
                            dest_pos, width, height, dest_offset,
                            font_data_offset);
    }
}
#endif

void surface_draw_string_depth(Surface *surface, const char *text, int x, int y,
                               FONT_STYLE font, int colour, float depth) {
    int8_t *font_data = game_fonts[font];
    int text_length = strlen(text);

    for (int i = 0; i < text_length; i++) {
        if (text[i] == '@' && i + 4 < text_length && text[i + 4] == '@') {
            int start = i + 1;
            int end = i + 4;

            char sliced[(end - start) + 1];
            memset(sliced, '\0', (end - start) + 1);

            strncpy(sliced, text + start, end - start);
            strtolower(sliced);

            int string_colour = colour_str_to_colour(sliced);

            if (string_colour >= 0) {
                colour = string_colour;
            }

            i += 4;
        } else if (text[i] == '~' && i + 4 < text_length &&
                   text[i + 4] == '~') {
            char c = text[i + 1];
            char c1 = text[i + 2];
            char c2 = text[i + 3];

            if (c >= '0' && c <= '9' && c1 >= '0' && c1 <= '9' && c2 >= '0' &&
                c2 <= '9') {
                int start = i + 1;
                int end = i + 4;
                char sliced[(end - start) + 1];
                sliced[end - start] = '\0';
                strncpy(sliced, text + start, end - start);
                x = atoi(sliced);
            }

            i += 4;
        } else if (text[i] == '~' && i + 5 < text_length &&
                   text[i + 5] == '~') {
            char c = text[i + 1];
            char c1 = text[i + 2];
            char c2 = text[i + 3];
            char c3 = text[i + 4];

            if (c >= '0' && c <= '9' && c1 >= '0' && c1 <= '9' && c2 >= '0' &&
                c2 <= '9' && c3 >= '0' && c3 <= '9') {
                int start = i + 1;
                int end = i + 5;
                char sliced[(end - start) + 1];
                sliced[end - start] = '\0';
                strncpy(sliced, text + start, end - start);
                x = atoi(sliced);
            }

            i += 5;
        } else {
            int index = (unsigned)text[i];

            if (index > 255) {
                index = 0;
            }

            int character_offset = character_width[index];
            int draw_shadow = surface->draw_string_shadow && colour != 0;

#ifdef RENDER_SW
            if (draw_shadow) {
                surface_draw_character(surface, character_offset, x + 1, y,
                                       BLACK, font_data);

                surface_draw_character(surface, character_offset, x, y + 1,
                                       BLACK, font_data);
            }

            surface_draw_character(surface, character_offset, x, y, colour,
                                   font_data);
#elif defined(RENDER_GL) || defined(RENDER_3DS_GL)
            surface_gl_buffer_character(surface, text[i], x, y, colour, font,
                                        draw_shadow, depth);
#endif

            /* character display width */
            x += font_data[character_offset + 7];
        }
    }

#ifdef RENDER_SW
    (void)depth;
#endif
}

void surface_draw_string(Surface *surface, const char *text, int x, int y,
                         FONT_STYLE font, int colour) {
    surface_draw_string_depth(surface, text, x, y, font, colour, 0);
}

void surface_draw_string_right(Surface *surface, const char *text, int x, int y,
                               FONT_STYLE font, int colour) {
    surface_draw_string(surface, text, x - surface_text_width(text, font), y,
                        font, colour);
}

void surface_draw_string_centre_depth(Surface *surface, const char *text, int x,
                                      int y, FONT_STYLE font, int colour,
                                      float depth) {
    surface_draw_string_depth(surface, text,
                              x - (int)(surface_text_width(text, font) / 2), y,
                              font, colour, depth);
}

void surface_draw_string_centre(Surface *surface, const char *text, int x,
                                int y, FONT_STYLE font, int colour) {
    surface_draw_string(surface, text,
                        x - (int)(surface_text_width(text, font) / 2), y, font,
                        colour);
}

int surface_paragraph_height(Surface *surface, const char *text,
                             FONT_STYLE font, int max, int max_height) {
    int y = 0;
    int width = 0;
    int8_t *font_data = game_fonts[font];
    int start = 0;
    int end = 0;
    int text_length = strlen(text);

    for (int i = 0; i < text_length; i++) {
        if (text[i] == '@' && i + 4 < text_length && text[i + 4] == '@') {
            i += 4;
        } else if (text[i] == '~' && i + 4 < text_length &&
                   text[i + 4] == '~') {
            i += 4;
        } else {
            unsigned int character_index = (unsigned)text[i];
            width += font_data[character_width[character_index] + 7];
        }

        if (text[i] == ' ') {
            end = i;
        } else if (text[i] == '%') {
            end = i;
            width = 1000;
        }

        if (width > max) {
            if (end <= start) {
                end = i;
            }

            width = 0;
            start = i = end + 1;

            y += surface_text_height(font);

            if (y >= max_height) {
                return i;
            }
        }
    }

    return -1;
}

void surface_draw_paragraph(Surface *surface, const char *text, int x, int y,
                            FONT_STYLE font, int colour, int max) {
    int width = 0;
    int8_t *font_data = game_fonts[font];
    int start = 0;
    int end = 0;
    int text_length = strlen(text);

    for (int i = 0; i < text_length; i++) {
        if (text[i] == '@' && i + 4 < text_length && text[i + 4] == '@') {
            i += 4;
        } else if (text[i] == '~' && i + 4 < text_length &&
                   text[i + 4] == '~') {
            i += 4;
        } else {
            unsigned int character_index = (unsigned)text[i];
            width += font_data[character_width[character_index] + 7];
        }

        if (text[i] == ' ') {
            end = i;
        } else if (text[i] == '%') {
            end = i;
            width = 1000;
        }

        if (width > max) {
            if (end <= start) {
                end = i;
            }

            char sliced[(end - start) + 1];
            memset(sliced, '\0', (end - start) + 1);
            strncpy(sliced, text + start, end - start);
            surface_draw_string_centre(surface, sliced, x, y, font, colour);

            width = 0;
            start = i = end + 1;

            y += surface_text_height(font);
        }
    }

    if (width > 0) {
        char sliced[(text_length - start) + 1];
        memset(sliced, '\0', (text_length - start) + 1);
        strncpy(sliced, text + start, text_length - start);
        surface_draw_string_centre(surface, sliced, x, y, font, colour);
    }
}

int surface_text_height_font(FONT_STYLE font) {
    if (font == 0) {
        return game_fonts[font][8] - 2;
    }

    return game_fonts[font][8] - 1;
}

int surface_text_height(FONT_STYLE font) {
    switch (font) {
    case FONT_REGULAR_11:
        return 12;
    case FONT_BOLD_12:
    case FONT_REGULAR_12:
        return 14;
    case FONT_BOLD_13:
    case FONT_BOLD_14:
        return 15;
    case FONT_BOLD_16:
        return 19;
    case FONT_BOLD_20:
        return 24;
    case FONT_BOLD_24:
        return 29;
    default:
        return surface_text_height_font(font);
    }
}

int surface_text_width(const char *text, FONT_STYLE font) {
    int total = 0;
    int8_t *font_data = game_fonts[font];
    int text_length = strlen(text);

    for (int i = 0; i < text_length; i++) {
        if (text[i] == '@' && i + 4 < text_length && text[i + 4] == '@') {
            i += 4;
        } else if (text[i] == '~' && i + 4 < text_length &&
                   text[i + 4] == '~') {
            i += 4;
        } else {
            total += font_data[character_width[(unsigned)text[i]] + 7];
        }
    }

    return total;
}

void surface_draw_tabs(Surface *surface, int x, int y, int width, int height,
                       char **tabs, int tabs_length, int selected) {
    int tab_width = (int)ceilf(width / (float)tabs_length);
    int offset_x = 0;

    for (int i = 0; i < tabs_length; i++) {
        int tab_colour = selected == i ? GREY_DC : GREY_A0;

        surface_draw_box_alpha(surface, x + offset_x, y, tab_width, height,
                               tab_colour, 128);

        surface_draw_string_centre(
            surface, tabs[i], x + offset_x + (tab_width / 2), y + 16, 4, BLACK);

        if (i > 0) {
            surface_draw_line_vertical(surface, x + offset_x, y, height, BLACK);
        }

        offset_x += tab_width;
    }

    surface_draw_line_horizontal(surface, x, y + height, width, BLACK);
}

void surface_draw_item(Surface *surface, int x, int y, int slot_width,
                       int slot_height, int item_id) {
    surface_draw_sprite_transform_mask(
        surface, x, y, slot_width - 1, slot_height - 2,
        surface->mud->sprite_item + game_data.items[item_id].sprite,
        game_data.items[item_id].mask, 0, 0, 0);
}

/* used in bank and shop */
void surface_draw_item_grid(Surface *surface, int x, int y, int rows,
                            int columns, int slot_width, int slot_height,
                            int *items, int *items_count, int items_length,
                            int selected, int show_inventory_count) {
    int is_selected_visible = selected >= 0 && selected <= items_length;

    int box_width = (columns * slot_width);
    int box_height = (rows * slot_height);

    if (!is_selected_visible) {
        surface_draw_box_alpha(surface, x + 1, y + 1, box_width - 1,
                               box_height - 1, GREY_D0, 160);
    }

    int item_index = 0;

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            int slot_x = x + column * slot_width;
            int slot_y = y + row * slot_height;
            int slot_colour = selected == item_index ? RED : GREY_D0;
            int offset_x = show_inventory_count ? 0 : 1;
            int offset_y = show_inventory_count ? 0 : 1;

            if (is_selected_visible) {
                surface_draw_box_alpha(surface, slot_x, slot_y, slot_width,
                                       slot_height, slot_colour, 160);
            }

            int item_id = items[item_index];

            if (item_index < items_length && item_id != -1) {
                mudclient_draw_item(surface->mud, slot_x + offset_x,
                                    slot_y + offset_y, slot_width, slot_height,
                                    item_id);

                int item_count = items_count[item_index];
                int font_size = slot_width < ITEM_GRID_SLOT_WIDTH ? 0 : 1;
                if (show_inventory_count) {
                    char formatted_amount[15] = {0};

                    mudclient_format_item_amount(surface->mud, item_count,
                                                 formatted_amount);

                    surface_draw_string(surface, formatted_amount, slot_x + 1,
                                        slot_y + 10, font_size, GREEN);

                    mudclient_format_item_amount(
                        surface->mud,
                        mudclient_get_inventory_count(surface->mud, item_id),
                        formatted_amount);

                    surface_draw_string_right(
                        surface, formatted_amount, slot_x + slot_width - 2,
                        slot_y + slot_height - 5, font_size, CYAN);
                } else if (game_data.items[item_id].stackable == 0) {
                    char formatted_amount[15] = {0};

                    mudclient_format_item_amount(surface->mud, item_count,
                                                 formatted_amount);

                    surface_draw_string(
                        surface, formatted_amount, slot_x + 1 + offset_x,
                        slot_y + 10 + offset_x, font_size, YELLOW);
                }
            }

            item_index++;
        }
    }

    surface_draw_border(surface, x, y, box_width + 1, box_height + 1, BLACK);

    for (int row = 1; row < rows; row++) {
        surface_draw_line_horizontal(surface, x, y + (row * slot_height),
                                     box_width, BLACK);
    }

    for (int column = 1; column < columns; column++) {
        surface_draw_line_vertical(surface, x + (column * slot_width), y,
                                   box_height, BLACK);
    }
}

void surface_draw_scrollbar(Surface *surface, int x, int y, int width,
                            int height, int scrub_y, int scrub_height) {
    x += width - 12;

    surface_draw_border(surface, x, y, 12, height, 0);

    /* up arrow */
    surface_draw_sprite(surface, x + 1, y + 1, surface->mud->sprite_util);

    /* down arrow */
    surface_draw_sprite(surface, x + 1, y + height - 12,
                        surface->mud->sprite_util + 1);

    surface_draw_line_horizontal(surface, x, y + 13, 12, 0);
    surface_draw_line_horizontal(surface, x, y + height - 13, 12, 0);

    surface_draw_gradient(surface, x + 1, y + 14, 11, height - 27,
                          SCROLLBAR_TOP_COLOUR, SCROLLBAR_BOTTOM_COLOUR);

    surface_draw_box(surface, x + 3, scrub_y + y + 14, 7, scrub_height,
                     SCRUB_MIDDLE_COLOUR);

    surface_draw_line_vertical(surface, x + 2, scrub_y + y + 14, scrub_height,
                               SCRUB_LEFT_COLOUR);

    surface_draw_line_vertical(surface, x + 2 + 8, scrub_y + y + 14,
                               scrub_height, SCRUB_RIGHT_COLOUR);
}

void surface_draw_status_bar(Surface *surface, int max, int current,
                             char *label, int x, int y, int width, int height,
                             int background_colour, int foreground_colour) {
    int current_width = current >= max ? width : (current / (float)max) * width;

    surface_draw_box_alpha(surface, x, y, current_width, height,
                           foreground_colour, 128);

    surface_draw_box_alpha(surface, x + current_width, y, width - current_width,
                           height, background_colour, 128);

    surface_draw_border(surface, x, y, width, height, BLACK);

    char formatted_status[strlen(label) + 27];

    if (MUD_IS_COMPACT) {
        sprintf(formatted_status, "%d / %d", current, max);
    } else {
        sprintf(formatted_status, "%s: %d / %d", label, current, max);
    }

    surface_draw_string_centre(surface, formatted_status, x + (width / 2),
                               y + 12, 0, WHITE);
}

#ifdef RENDER_GL
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

#ifdef RENDER_3DS_GL
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

#if 0
void surface_3ds_gl_blur_texture(Surface *surface, int sprite_id,
                                 int blur_height, int x, int y, int height) {
    int offset_x = 0;
    int offset_y = 0;

    if (!surface_3ds_gl_get_sprite_texture_offsets(surface, sprite_id,
                                                   &offset_x, &offset_y)) {
        return;
    }

    uint16_t *texture_data = (uint16_t *)surface->gl_sprite_texture.data;

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
                            int32_t pixel = _3ds_gl_rgba5551_to_rgb32(
                                texture_data[_3ds_gl_translate_texture_index(
                                                 x2 + offset_x, y2 + offset_y,
                                                 1024) /
                                             2]);

                            r += (pixel >> 16) & 0xff;
                            g += (pixel >> 8) & 0xff;
                            b += pixel & 0xff;
                            a++;
                        }
                    }
                }
            }

            /*texture_data[] =
                _3ds_gl_rgb32_to_rgba5551(((r / a) << 16) + ((g / a) << 8) +
                                          (b / a));*/
        }
    }

    surface_gl_update_dynamic_texture(surface);
}

void surface_3ds_gl_apply_login_filter(Surface *surface, int sprite_id) {
    for (int i = 6; i >= 1; i--) {
        surface_3ds_gl_blur_texture(surface, sprite_id, i, 0, i, 8);
    }

    int sprite_height = surface->sprite_height[sprite_id];

    for (int i = 6; i >= 1; i--) {
        surface_3ds_gl_blur_texture(surface, sprite_id, i, 0,
                                    sprite_height - 6 - i, 8);
    }
}
#endif

#endif
