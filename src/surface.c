#include "surface.h"

int an_int_346 = 0;
int an_int_347 = 0;
int an_int_348 = 0;

int8_t *game_fonts[50];
int character_width[256];

int32_t *surface_texture_pixels;

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

        // removed pound sign
        if (index > 63) {
            index++;
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
#elif RENDER_GL
    surface->pixels = calloc(width * height, sizeof(int32_t));
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
    // TODO surface_init_gl()
    surface_gl_create_framebuffer(surface);

    /* coloured quads */
#ifdef EMSCRIPTEN
    shader_new(&surface->gl_flat_shader, "./cache/flat.webgl.vs",
               "./cache/flat.webgl.fs");
#else
    shader_new(&surface->gl_flat_shader, "./cache/flat.vs", "./cache/flat.fs");
#endif

    shader_use(&surface->gl_flat_shader);

    glGenVertexArrays(1, &surface->gl_flat_vao);
    glBindVertexArray(surface->gl_flat_vao);

    glGenBuffers(1, &surface->gl_flat_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, surface->gl_flat_vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 13 * FLAT_QUAD_COUNT * 4,
                 NULL, GL_DYNAMIC_DRAW);

    /* vertex { x, y, z } */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float),
                          (void *)0);

    glEnableVertexAttribArray(0);

    /* colour { r, g, b, a } */
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 13 * sizeof(float),
                          (void *)(3 * sizeof(float)));

    glEnableVertexAttribArray(1);

    /* skin colour { r, g, b } */
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float),
                          (void *)(7 * sizeof(float)));

    glEnableVertexAttribArray(2);

    /* texture { s, t, index } */
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float),
                          (void *)(10 * sizeof(float)));

    glEnableVertexAttribArray(3);

    glGenBuffers(1, &surface->gl_flat_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface->gl_flat_ebo);

    /* two triangles per quad (6 vertex indices) */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6 * FLAT_QUAD_COUNT,
                 NULL, GL_DYNAMIC_DRAW);

    /* +1 for circle, +2 for extra login screen scenes, +1 for sleep, +1 for
     * logo */
    surface_gl_create_texture_array(&surface->gl_sprite_media_textures,
                                    MEDIA_TEXTURE_WIDTH, MEDIA_TEXTURE_HEIGHT,
                                    (mud->sprite_item - mud->sprite_media) + 5);

    surface_gl_create_texture_array(&surface->gl_map_textures,
                                    MAP_TEXTURE_WIDTH, MAP_TEXTURE_HEIGHT, 1);

    surface_gl_create_texture_array(&surface->gl_font_textures,
                                    FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT,
                                    FONT_COUNT * 2);

    surface_gl_create_circle_texture(surface);
    surface_gl_reset_context(surface);
#endif

#ifdef RENDER_3DS_GL
    surface->_3ds_gl_flat_shader_dvlb =
        DVLB_ParseFile((u32 *)flat_shbin, flat_shbin_size);

    shaderProgramInit(&surface->_3ds_gl_flat_shader);

    shaderProgramSetVsh(&surface->_3ds_gl_flat_shader,
                        &surface->_3ds_gl_flat_shader_dvlb->DVLE[0]);

    C3D_BindProgram(&surface->_3ds_gl_flat_shader);

    surface->_3ds_gl_projection_uniform = shaderInstanceGetUniformLocation(
        (&surface->_3ds_gl_flat_shader)->vertexShader, "projection");

    surface->_3ds_gl_interlace_uniform = shaderInstanceGetUniformLocation(
        (&surface->_3ds_gl_flat_shader)->vertexShader, "interlace");

    C3D_AttrInfo *attr_info = C3D_GetAttrInfo();
    AttrInfo_Init(attr_info);

    /* vertex { x, y, z } */
    AttrInfo_AddLoader(attr_info, 0, GPU_FLOAT, 3);

    /* colour { r, g, b, a } */
    AttrInfo_AddLoader(attr_info, 1, GPU_FLOAT, 4);

    /* skin colour { r, g, b } */
    AttrInfo_AddLoader(attr_info, 2, GPU_FLOAT, 3);

    /* texture { s, t } ({ u ,v }) */
    AttrInfo_AddLoader(attr_info, 3, GPU_FLOAT, 2);

    surface->_3ds_gl_flat_vbo =
        linearAlloc(FLAT_QUAD_COUNT * sizeof(_3ds_gl_flat_vertex) * 4);

    surface->_3ds_gl_flat_ebo =
        linearAlloc(FLAT_QUAD_COUNT * sizeof(uint16_t) * 6);

    C3D_BufInfo *buf_info = C3D_GetBufInfo();
    BufInfo_Init(buf_info);

    BufInfo_Add(buf_info, surface->_3ds_gl_flat_vbo,
                sizeof(_3ds_gl_flat_vertex), 4, 0x3210);

    C3D_TexEnv* tex_env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(tex_env);
    C3D_TexEnvSrc(tex_env, C3D_Both, GPU_PRIMARY_COLOR, 0, 0);
    C3D_TexEnvFunc(tex_env, C3D_Both, GPU_REPLACE);

    Mtx_OrthoTilt(&surface->_3ds_gl_projection, 0.0, 320.0, 0.0, 240.0, 0.0,
                  1.0, true);

    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, surface->_3ds_gl_projection_uniform,
                     &surface->_3ds_gl_projection);
#endif
}

#ifdef RENDER_GL
float surface_gl_translate_x(Surface *surface, int x) {
    return gl_translate_x(x, surface->width);
}

float surface_gl_translate_y(Surface *surface, int y) {
    return gl_translate_y(y, surface->height);
}

void surface_gl_create_texture_array(GLuint *texture_array_id, int width,
                                     int height, int length) {
    glGenTextures(1, texture_array_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, *texture_array_id);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, length);
}

void surface_gl_create_font_texture(int32_t *dest, int font_id,
                                    int draw_shadow) {
    int8_t *font_data = game_fonts[font_id];
    int slot_size = surface_text_height(font_id);

    if (draw_shadow) {
        slot_size += 1;
    }

    for (int i = 0; i < CHAR_SET_LENGTH; i++) {
        int draw_x = (i % 10) * slot_size;
        int draw_y = (i / 10) * slot_size;

        int character_offset = character_width[(unsigned)CHAR_SET[i]];
        int width = font_data[character_offset + 3];
        int height = font_data[character_offset + 4];

        /* position of pixel data for the font (on/off) */
        int font_pos = font_data[character_offset] * 16384 +
                       font_data[character_offset + 1] * 128 +
                       font_data[character_offset + 2];

        int dest_offset = FONT_TEXTURE_WIDTH - width;

        if (draw_shadow) {
            int dest_pos_right = (draw_x + 1) + draw_y * FONT_TEXTURE_WIDTH;

            surface_plot_letter(dest, font_data, 0xff000000, font_pos,
                                dest_pos_right, width, height, dest_offset, 0);

            int dest_pos_bottom = draw_x + (draw_y + 1) * FONT_TEXTURE_WIDTH;

            surface_plot_letter(dest, font_data, 0xff000000, font_pos,
                                dest_pos_bottom, width, height, dest_offset, 0);
        }

        int dest_pos = draw_x + draw_y * FONT_TEXTURE_WIDTH;

        surface_plot_letter(dest, font_data, 0xffffffff, font_pos, dest_pos,
                            width, height, dest_offset, 0);
    }
}

void surface_gl_create_font_textures(Surface *surface) {
    glBindTexture(GL_TEXTURE_2D_ARRAY, surface->gl_font_textures);

    int font_area = FONT_TEXTURE_WIDTH * FONT_TEXTURE_HEIGHT;
    int32_t *font_raster = calloc(font_area, sizeof(int32_t));

    /* create two textures for shadow and non-shadow characters */
    for (int i = 0; i < FONT_COUNT; i++) {
        surface_gl_create_font_texture(font_raster, i, 0);

        gl_update_texture_array(surface->gl_font_textures, i,
                                FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT,
                                font_raster, 0);

        memset(font_raster, 0, font_area * sizeof(int32_t));
    }

    for (int i = 0; i < FONT_COUNT; i++) {
        surface_gl_create_font_texture(font_raster, i, 1);

        gl_update_texture_array(surface->gl_font_textures, FONT_COUNT + i,
                                FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT,
                                font_raster, 0);

        memset(font_raster, 0, font_area * sizeof(int32_t));
    }

    free(font_raster);
}

void surface_gl_create_circle_texture(Surface *surface) {
    surface_draw_box(surface, 0, 0, CIRCLE_TEXTURE_SIZE, CIRCLE_TEXTURE_SIZE,
                     0);

    surface_draw_circle_software(surface, CIRCLE_TEXTURE_SIZE / 2,
                                 CIRCLE_TEXTURE_SIZE / 2,
                                 CIRCLE_TEXTURE_SIZE / 2, 0xffffffff, 255);

    int *circle_pixels =
        calloc(MEDIA_TEXTURE_WIDTH * MEDIA_TEXTURE_HEIGHT, sizeof(int));

    int circle_index = 0;

    for (int x = 0; x < MEDIA_TEXTURE_WIDTH; x++) {
        for (int y = 0; y < MEDIA_TEXTURE_HEIGHT; y++) {
            int colour = surface->pixels[y + x * MEDIA_TEXTURE_HEIGHT];

            if (colour != 0) {
                colour += 0xff000000;
            }

            circle_pixels[circle_index++] = colour;
        }
    }

    int texture_index = surface->mud->sprite_item - surface->mud->sprite_media;

    gl_update_texture_array(surface->gl_sprite_media_textures, texture_index,
                            MEDIA_TEXTURE_WIDTH, MEDIA_TEXTURE_HEIGHT,
                            circle_pixels, 0);

    free(circle_pixels);
}

void surface_gl_reset_context(Surface *surface) {
    surface->gl_flat_count = 0;

    surface->gl_contexts[0].texture_id = 0;
    surface->gl_contexts[0].quad_count = 0;

    surface->gl_contexts[0].min_x = surface->bounds_min_x;
    surface->gl_contexts[0].max_x = surface->bounds_max_x;

    surface->gl_contexts[0].min_y = surface->bounds_min_y;
    surface->gl_contexts[0].max_y = surface->bounds_max_y;

    surface->gl_context_count = 1;
}

void surface_gl_buffer_flat_quad(Surface *surface, GLfloat *quad,
                                 GLuint texture_array_id) {
    if (surface->gl_context_count >= FLAT_MAX_CONTEXTS) {
        fprintf(stderr, "too many context (texture/boundary) switches!\n");
        return;
    }

    glBindVertexArray(surface->gl_flat_vao);
    glBindBuffer(GL_ARRAY_BUFFER, surface->gl_flat_vbo);

    glBufferSubData(GL_ARRAY_BUFFER,
                    sizeof(GLfloat) * surface->gl_flat_count * 13 * 4,
                    sizeof(GLfloat) * 13 * 4, quad);

    GLuint index = surface->gl_flat_count * 4;

    GLuint indices[] = {index, index + 1, index + 2,
                        index, index + 2, index + 3};

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface->gl_flat_ebo);

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(indices) * surface->gl_flat_count, sizeof(indices),
                    indices);

    surface->gl_flat_count++;

    int context_index = surface->gl_context_count - 1;
    SurfaceGlContext *context = &surface->gl_contexts[context_index];

    if (context->min_x == surface->bounds_min_x &&
        context->max_x == surface->bounds_max_x &&
        context->min_y == surface->bounds_min_y &&
        context->max_y == surface->bounds_max_y &&
        (context->texture_id == texture_array_id ||
         texture_array_id == 0 &&
             context->texture_id != surface->gl_font_textures)) {
        context->quad_count++;
    } else {
        context = &surface->gl_contexts[context_index + 1];

        context->min_x = surface->bounds_min_x;
        context->max_x = surface->bounds_max_x;
        context->min_y = surface->bounds_min_y;
        context->max_y = surface->bounds_max_y;

        context->texture_id = texture_array_id;
        context->quad_count = 1;

        surface->gl_context_count++;
    }
}

int surface_gl_sprite_texture_array_id(Surface *surface, int sprite_id) {
    mudclient *mud = surface->mud;

    if (sprite_id == mud->sprite_media - 1) {
        return surface->gl_map_textures;
    }

    if (sprite_id == SPRITE_LIMIT - 1) {
        return surface->gl_sprite_media_textures;
    }

    if (sprite_id == mud->sprite_texture + 1) {
        return surface->gl_sprite_media_textures;
    }

    if (sprite_id == mud->sprite_logo || sprite_id == mud->sprite_logo + 1) {
        return surface->gl_sprite_media_textures;
    }

    if (sprite_id >= mud->sprite_media && sprite_id < mud->sprite_item) {
        return surface->gl_sprite_media_textures;
    }

    if ((sprite_id >= mud->sprite_item &&
         sprite_id <= mud->sprite_item + game_data_item_sprite_count)) {
        return surface->gl_sprite_item_textures;
    }

    if (sprite_id <= mud->sprite_media) {
        return surface->gl_sprite_entity_textures;
    }

    return 0;
}

int surface_gl_sprite_texture_width(Surface *surface, GLuint texture_array_id) {
    if (texture_array_id == surface->gl_map_textures) {
        return MAP_TEXTURE_WIDTH;
    }

    if (texture_array_id == surface->gl_sprite_media_textures) {
        return MEDIA_TEXTURE_WIDTH;
    }

    if (texture_array_id == surface->gl_sprite_entity_textures) {
        return ENTITY_TEXTURE_WIDTH;
    }

    if (texture_array_id == surface->gl_sprite_item_textures) {
        return ITEM_TEXTURE_WIDTH;
    }

    if (texture_array_id == surface->gl_font_textures) {
        return FONT_TEXTURE_WIDTH;
    }

    if (texture_array_id == surface->gl_framebuffer_textures) {
        return surface->mud->game_width;
    }

    return 0;
}

int surface_gl_sprite_texture_height(Surface *surface,
                                     GLuint texture_array_id) {
    if (texture_array_id == surface->gl_map_textures) {
        return MAP_TEXTURE_HEIGHT;
    }

    if (texture_array_id == surface->gl_sprite_media_textures) {
        return MEDIA_TEXTURE_HEIGHT;
    }

    if (texture_array_id == surface->gl_sprite_entity_textures) {
        return ENTITY_TEXTURE_HEIGHT;
    }

    if (texture_array_id == surface->gl_sprite_item_textures) {
        return ITEM_TEXTURE_HEIGHT;
    }

    if (texture_array_id == surface->gl_font_textures) {
        return FONT_TEXTURE_WIDTH;
    }

    if (texture_array_id == surface->gl_framebuffer_textures) {
        return surface->mud->game_height;
    }

    return 0;
}

/* index in the 2D texture array */
int surface_gl_sprite_texture_index(Surface *surface, int sprite_id) {
    mudclient *mud = surface->mud;

    if (sprite_id == SPRITE_LIMIT - 1) {
        return (mud->sprite_item - mud->sprite_media) + 4;
    }

    /* map texture */
    if (sprite_id == mud->sprite_media - 1) {
        return 0;
    }

    if (sprite_id == mud->sprite_logo || sprite_id == mud->sprite_logo + 1) {
        /* +1 for circle texture */
        int offset =
            (surface->mud->sprite_item - surface->mud->sprite_media) + 1;

        return offset + (sprite_id - mud->sprite_logo);
    }

    /* sleep texture */
    if (sprite_id == mud->sprite_texture + 1) {
        return (surface->mud->sprite_item - surface->mud->sprite_media) + 3;
    }

    if (sprite_id >= mud->sprite_media && sprite_id < mud->sprite_item) {
        return sprite_id - mud->sprite_media;
    }

    if (sprite_id >= mud->sprite_item &&
        sprite_id <= mud->sprite_item + game_data_item_sprite_count) {
        return sprite_id - mud->sprite_item;
    }

    if (sprite_id < mud->sprite_media) {
        return surface->gl_entity_sprite_indices[sprite_id];
    }

    return 0;
}

void surface_gl_buffer_textured_quad(Surface *surface, GLuint texture_array_id,
                                     int texture_index, int mask_colour,
                                     int skin_colour, int alpha, int width,
                                     int height, int (*points)[2],
                                     float depth_top, float depth_bottom) {
    GLfloat mask_r = -1.0f;
    GLfloat mask_g = -1.0f;
    GLfloat mask_b = -1.0f;
    GLfloat mask_a = alpha / 255.0f;

    if (mask_colour != 0) {
        mask_r = ((mask_colour >> 16) & 0xff) / 255.0f;
        mask_g = ((mask_colour >> 8) & 0xff) / 255.0f;
        mask_b = (mask_colour & 0xff) / 255.0f;
    }

    GLfloat skin_r = -1.0f;
    GLfloat skin_g = -1.0f;
    GLfloat skin_b = -1.0f;

    if (skin_colour != 0) {
        skin_r = ((skin_colour >> 16) & 0xff) / 255.0f;
        skin_g = ((skin_colour >> 8) & 0xff) / 255.0f;
        skin_b = (skin_colour & 0xff) / 255.0f;
    }

    GLfloat texture_width =
        (float)surface_gl_sprite_texture_width(surface, texture_array_id);

    GLfloat texture_height =
        (float)surface_gl_sprite_texture_height(surface, texture_array_id);

    if (texture_array_id == surface->gl_framebuffer_textures) {
        // printf("%f %f\n", texture_height, surface_gl_translate_y(surface,
        // points[2][1]));
    }

    GLfloat textured_quad[] = {
        /* top left / northwest */
        surface_gl_translate_x(surface, points[0][0]),
        surface_gl_translate_y(surface, points[0][1]), //
        depth_top,                                     //
        mask_r, mask_g, mask_b, mask_a,                //
        skin_r, skin_g, skin_b,                        //
        0, 0,                                          //
        texture_index,                                 //

        /* top right / northeast */
        surface_gl_translate_x(surface, points[1][0]),
        surface_gl_translate_y(surface, points[1][1]), //
        depth_top,                                     //
        mask_r, mask_g, mask_b, mask_a,                //
        skin_r, skin_g, skin_b,                        //
        width / texture_width, 0,                      //
        texture_index,                                 //

        /* bottom right / southeast */
        surface_gl_translate_x(surface, points[2][0]),
        surface_gl_translate_y(surface, points[2][1]),  //
        depth_bottom,                                   //
        mask_r, mask_g, mask_b, mask_a,                 //
        skin_r, skin_g, skin_b,                         //
        width / texture_width, height / texture_height, //
        texture_index,                                  //

        /* bottom left / southwest */
        surface_gl_translate_x(surface, points[3][0]),
        surface_gl_translate_y(surface, points[3][1]), //
        depth_bottom,                                  //
        mask_r, mask_g, mask_b, mask_a,                //
        skin_r, skin_g, skin_b,                        //
        0, height / texture_height,                    //
        texture_index                                  //
    };

    surface_gl_buffer_flat_quad(surface, textured_quad, texture_array_id);
}

void surface_gl_buffer_sprite(Surface *surface, int sprite_id, int x, int y,
                              int draw_width, int draw_height, int skew_x,
                              int mask_colour, int skin_colour, int alpha,
                              int flip, int rotation, float depth_top,
                              float depth_bottom) {
    GLuint texture_array_id =
        surface_gl_sprite_texture_array_id(surface, sprite_id);

    if (texture_array_id == 0) {
        return;
    }

    int full_width = surface->sprite_width_full[sprite_id];
    int full_height = surface->sprite_height_full[sprite_id];

    if (draw_width == -1) {
        if (surface->sprite_translate[sprite_id]) {
            draw_width = full_width;
            draw_height = full_height;
        } else {
            draw_width = surface->sprite_width[sprite_id];
            draw_height = surface->sprite_height[sprite_id];
        }
    }

    int points[4][2] = {0};

    if (flip) {
        /* top right */
        points[0][0] = draw_width;
        points[0][1] = 0;

        /* top left */
        points[1][0] = 0;
        points[1][1] = 0;

        /* bottom left */
        points[2][0] = 0;
        points[2][1] = draw_height;

        /* bottom right */
        points[3][0] = draw_width;
        points[3][1] = draw_height;
    } else {
        /* top left */
        points[0][0] = 0;
        points[0][1] = 0;

        /* top right */
        points[1][0] = draw_width;
        points[1][1] = 0;

        /* bottom right */
        points[2][0] = draw_width;
        points[2][1] = draw_height;

        /* bottom left */
        points[3][0] = 0;
        points[3][1] = draw_height;
    }

    if (rotation != 0) {
        // TODO maybe use floats?
        int centre_x = (draw_width - 1) / 2;
        int centre_y = (draw_height - 1) / 2;
        float angle = TABLE_TO_RADIANS(-rotation, 512);

        for (int i = 0; i < 4; i++) {
            rotate_point(centre_x, centre_y, angle, points[i]);
        }
    }

    for (int i = 0; i < 4; i++) {
        int *point = points[i];

        point[0] += x + (i < 2 ? skew_x : 0);
        point[1] += y;
    }

    int texture_index = surface_gl_sprite_texture_index(surface, sprite_id);

    surface_gl_buffer_textured_quad(
        surface, texture_array_id, texture_index, mask_colour, skin_colour,
        alpha, full_width, full_height, points, depth_top, depth_bottom);
}

void surface_gl_buffer_character(Surface *surface, char character, int x, int y,
                                 int colour, int font_id, int draw_shadow,
                                 float depth) {
    if (character == ' ') {
        return;
    }

    int8_t *font_data = game_fonts[font_id];
    int slot_size = surface_text_height(font_id);

    if (draw_shadow) {
        slot_size += 1;
    }

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

    GLfloat left_x = surface_gl_translate_x(surface, x);
    GLfloat right_x = surface_gl_translate_x(surface, x + width);
    GLfloat top_y = surface_gl_translate_y(surface, y);
    GLfloat bottom_y = surface_gl_translate_y(surface, y + height);

    GLfloat r = ((colour >> 16) & 0xff) / 255.0f;
    GLfloat g = ((colour >> 8) & 0xff) / 255.0f;
    GLfloat b = (colour & 0xff) / 255.0f;

    GLfloat texture_x =
        ((char_set_index % 10) * slot_size) / (float)FONT_TEXTURE_WIDTH;

    GLfloat texture_y =
        ((char_set_index / 10) * slot_size) / (float)FONT_TEXTURE_HEIGHT;

    float texture_width = width / (float)FONT_TEXTURE_WIDTH;
    float texture_height = height / (float)FONT_TEXTURE_HEIGHT;

    if (draw_shadow) {
        font_id += FONT_COUNT;
    }

    GLfloat char_quad[] = {
        /* top left / northwest */
        left_x, top_y, depth, //
        r, g, b, 1.0f,        //
        -1.0f, -1.0f, -1.0f,  //
        texture_x, texture_y, //
        font_id,              //

        /* top right / northeast */
        right_x, top_y, depth,                //
        r, g, b, 1.0f,                        //
        -1.0f, -1.0f, -1.0f,                  //
        texture_x + texture_width, texture_y, //
        font_id,                              //

        /* bottom right / southeast */
        right_x, bottom_y, depth,                              //
        r, g, b, 1.0f,                                         //
        -1.0f, -1.0f, -1.0f,                                   //
        texture_x + texture_width, texture_y + texture_height, //
        font_id,                                               //

        /* bottom left / southwest */
        left_x, bottom_y, depth,               //
        r, g, b, 1.0f,                         //
        -1.0f, -1.0f, -1.0f,                   //
        texture_x, texture_y + texture_height, //
        font_id                                //
    };

    surface_gl_buffer_flat_quad(surface, char_quad, surface->gl_font_textures);
}

void surface_gl_buffer_box(Surface *surface, int x, int y, int width,
                           int height, int colour, int alpha) {
    float red_f = ((colour >> 16) & 0xff) / 255.0f;
    float green_f = ((colour >> 8) & 0xff) / 255.0f;
    float blue_f = (colour & 0xff) / 255.0f;
    float alpha_f = alpha / 255.0f;

    GLfloat left_x = surface_gl_translate_x(surface, x);
    GLfloat right_x = surface_gl_translate_x(surface, x + width);
    GLfloat top_y = surface_gl_translate_y(surface, y);
    GLfloat bottom_y = surface_gl_translate_y(surface, y + height);

    GLfloat box_quad[] = {
        /* top left / northwest */
        left_x, top_y, 0,                //
        red_f, green_f, blue_f, alpha_f, //
        -1.0, -1.0, -1.0,                //
        0, 0, -1,                        //

        /* top right / northeast */
        right_x, top_y, 0,               //
        red_f, green_f, blue_f, alpha_f, //
        -1.0, -1.0, -1.0,                //
        0, 0, -1,                        //

        /* bottom right / southeast */
        right_x, bottom_y, 0,            //
        red_f, green_f, blue_f, alpha_f, //
        -1.0, -1.0, -1.0,                //
        0, 0, -1,                        //

        /* bottom left / southwest */
        left_x, bottom_y, 0,             //
        red_f, green_f, blue_f, alpha_f, //
        -1.0, -1.0, -1.0,                //
        0, 0, -1                         //
    };

    surface_gl_buffer_flat_quad(surface, box_quad, 0);
}

void surface_gl_buffer_circle(Surface *surface, int x, int y, int radius,
                              int colour, int alpha, float depth) {
    int diameter = radius * 2;

    x -= radius;
    y -= radius;

    GLfloat left_x = surface_gl_translate_x(surface, x);
    GLfloat right_x = surface_gl_translate_x(surface, x + diameter);
    GLfloat top_y = surface_gl_translate_y(surface, y);
    GLfloat bottom_y = surface_gl_translate_y(surface, y + diameter);

    float r = ((colour >> 16) & 0xff) / 255.0f;
    float g = ((colour >> 8) & 0xff) / 255.0f;
    float b = (colour & 0xff) / 255.0f;
    float a = alpha / 255.0f;

    int circle_index = surface->mud->sprite_item - surface->mud->sprite_media;

    GLfloat circle_quad[] = {
        /* top left / northwest */
        left_x, top_y, depth, //
        r, g, b, a,           //
        -1.0f, -1.0f, -1.0f,  //
        0, 0,                 //
        circle_index,         //

        /* top right / northeast */
        right_x, top_y, depth,                               //
        r, g, b, a,                                          //
        -1.0f, -1.0f, -1.0f,                                 //
        CIRCLE_TEXTURE_SIZE / (float)MEDIA_TEXTURE_WIDTH, 0, //
        circle_index,                                        //

        /* bottom right / southeast */
        right_x, bottom_y, depth,                          //
        r, g, b, a,                                        //
        -1.0f, -1.0f, -1.0f,                               //
        CIRCLE_TEXTURE_SIZE / (float)MEDIA_TEXTURE_WIDTH,  //
        CIRCLE_TEXTURE_SIZE / (float)MEDIA_TEXTURE_HEIGHT, //
        circle_index,                                      //

        /* bottom left / southwest */
        left_x, bottom_y, depth,                              //
        r, g, b, a,                                           //
        -1.0f, -1.0f, -1.0f,                                  //
        0, CIRCLE_TEXTURE_SIZE / (float)MEDIA_TEXTURE_HEIGHT, //
        circle_index                                          //
    };

    surface_gl_buffer_flat_quad(surface, circle_quad,
                                surface->gl_sprite_media_textures);
}

void surface_gl_create_framebuffer(Surface *surface) {
    free(surface->gl_screen_pixels_reversed);

    surface->gl_screen_pixels_reversed = calloc(
        surface->mud->game_width * surface->mud->game_height, sizeof(int32_t));

    free(surface->gl_screen_pixels);

    surface->gl_screen_pixels = calloc(
        surface->mud->game_width * surface->mud->game_height, sizeof(int32_t));

    surface->gl_last_screen_width = surface->mud->game_width;
    surface->gl_last_screen_height = surface->mud->game_height;

    surface_gl_create_texture_array(&surface->gl_framebuffer_textures,
                                    surface->mud->game_width,
                                    surface->mud->game_height, 1);
}

void surface_gl_update_framebuffer(Surface *surface) {
    if (surface->gl_last_screen_width != surface->mud->game_width ||
        surface->gl_last_screen_height != surface->mud->game_height) {
        if (surface->gl_framebuffer_textures != 0) {
            glDeleteTextures(1, &surface->gl_framebuffer_textures);
        }

        surface_gl_create_framebuffer(surface);
    }

    glReadPixels(0, 0, surface->mud->game_width, surface->mud->game_height,
                 GL_RGBA, GL_UNSIGNED_BYTE, surface->gl_screen_pixels_reversed);

    for (int x = 0; x < surface->mud->game_width; x++) {
        for (int y = 0; y < surface->mud->game_height; y++) {
            int colour = surface->gl_screen_pixels_reversed
                             [x + (surface->mud->game_height - y - 1) *
                                      surface->mud->game_width];

            // colour = y >= surface->mud->game_height / 2 ? 0xffff00ff :
            // 0xffff0000;
            surface->gl_screen_pixels[x + y * surface->mud->game_width] =
                colour;
        }
    }
}

void surface_gl_update_framebuffer_texture(Surface *surface) {
    glBindTexture(GL_TEXTURE_2D_ARRAY, surface->gl_framebuffer_textures);

    gl_update_texture_array(surface->gl_framebuffer_textures, 0,
                            surface->mud->game_width, surface->mud->game_height,
                            surface->gl_screen_pixels, 0);
}

void surface_gl_buffer_framebuffer_quad(Surface *surface) {
    int points[][2] = {
        {0, 0},                            //
        {surface->width, 0},               //
        {surface->width, surface->height}, //
        {0, surface->height}               //
    };

    surface_gl_buffer_textured_quad(surface, surface->gl_framebuffer_textures,
                                    0, 0, 0, 255, surface->mud->game_width,
                                    surface->mud->game_height, points, 0, 0);
}

float surface_gl_get_layer_depth(Surface *surface) {
    // return (-0.00875 * surface->mud->camera_zoom + 11.9375) / 100000.0;
    float min_depth = 0.00001f;
    float max_depth = 0.00008f;

    float zoom_scale = 1.0f - ((surface->mud->camera_zoom - ZOOM_MIN) /
                               //(float)(ZOOM_MAX - ZOOM_MIN));
                               800.0f);

    float depth = (zoom_scale * (max_depth - min_depth)) + min_depth;

    if (depth < 0.0f) {
        return min_depth / 2;
    }

    return depth;
}

void surface_gl_draw(Surface *surface, int use_depth) {
    glDisable(GL_CULL_FACE);

    if (!use_depth) {
        glDisable(GL_DEPTH_TEST);
    }

    shader_use(&surface->gl_flat_shader);

    shader_set_int(&surface->gl_flat_shader, "ui_scale",
                   use_depth ? 0 : mudclient_is_ui_scaled(surface->mud));

    glBindVertexArray(surface->gl_flat_vao);
    glActiveTexture(GL_TEXTURE0);

    int drawn_quads = 0;

    for (int i = 0; i < surface->gl_context_count; i++) {
        SurfaceGlContext *context = &surface->gl_contexts[i];

        int interlace = surface->interlace;

        shader_set_float(&surface->gl_flat_shader, "bounds_min_x",
                         (float)context->min_x);

        shader_set_float(&surface->gl_flat_shader, "bounds_max_x",
                         (float)context->max_x);

        shader_set_float(&surface->gl_flat_shader, "bounds_min_y",
                         (float)(surface->height - context->min_y));

        shader_set_float(&surface->gl_flat_shader, "bounds_max_y",
                         (float)(surface->height - context->max_y));

        GLuint texture_array_id = context->texture_id;

        if (texture_array_id == surface->gl_font_textures) {
            interlace = 0;
        }

        shader_set_int(&surface->gl_flat_shader, "interlace", interlace);

        if (texture_array_id != 0) {
            glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array_id);
        }

        int quad_count = context->quad_count;

        glDrawElements(GL_TRIANGLES, quad_count * 6, GL_UNSIGNED_INT,
                       (void *)(drawn_quads * 6 * sizeof(GLuint)));

        drawn_quads += quad_count;
    }

    if (surface->gl_fade_to_black) {
        surface_gl_update_framebuffer(surface);
        surface_fade_to_black_software(surface, surface->gl_screen_pixels, 1);
        surface_gl_update_framebuffer_texture(surface);
        surface->gl_has_faded = 1;
    }

    surface_gl_reset_context(surface);

    surface->gl_fade_to_black = 0;
}
#endif

#ifdef RENDER_3DS_GL
void surface_3ds_gl_buffer_flat_quad(Surface *surface, float *quad,
                                     int texture_id) {
    if (surface->_3ds_gl_flat_count >= 2) {
        return;
    }

    /*if (surface->gl_context_count >= FLAT_MAX_CONTEXTS) {
        fprintf(stderr, "too many context (texture/boundary) switches!\n");
        return;
    }*/

    int quad_vbo_size = (sizeof(float) * 12 * 4);

    memcpy(surface->_3ds_gl_flat_vbo +
               (surface->_3ds_gl_flat_count * quad_vbo_size),
           quad, quad_vbo_size);

    uint16_t index = surface->_3ds_gl_flat_count * 4;

    uint16_t indices[] = {index, index + 1, index + 2,
                          index, index + 2, index + 3};

    memcpy(surface->_3ds_gl_flat_ebo +
               (surface->_3ds_gl_flat_count * sizeof(indices)),
           indices, sizeof(indices));

    surface->_3ds_gl_flat_count++;

    /*int context_index = surface->_3ds_gl_context_count - 1;
    SurfaceGlContext *context = &surface->gl_contexts[context_index];

    if (context->min_x == surface->bounds_min_x &&
        context->max_x == surface->bounds_max_x &&
        context->min_y == surface->bounds_min_y &&
        context->max_y == surface->bounds_max_y &&
        (context->texture_id == texture_array_id ||
         texture_array_id == 0 &&
             context->texture_id != surface->gl_font_textures)) {
        context->quad_count++;
    } else {
        context = &surface->gl_contexts[context_index + 1];

        context->min_x = surface->bounds_min_x;
        context->max_x = surface->bounds_max_x;
        context->min_y = surface->bounds_min_y;
        context->max_y = surface->bounds_max_y;

        context->texture_id = texture_array_id;
        context->quad_count = 1;

        surface->gl_context_count++;
    }*/
}

void surface_3ds_gl_buffer_box(Surface *surface, int x, int y, int width,
                               int height, int colour, int alpha) {
    /*if (y != 0) {
        return;
    }*/

    float red_f = ((colour >> 16) & 0xff) / 255.0f;
    float green_f = ((colour >> 8) & 0xff) / 255.0f;
    float blue_f = (colour & 0xff) / 255.0f;
    float alpha_f = alpha / 255.0f;

    float left_x = x;
    float right_x = x + width;
    float top_y = (240 - y - height);
    float bottom_y = (240 - y);

    /*float left_x = surface_3ds_gl_translate_x(surface, x);
    float right_x = surface_3ds_gl_translate_x(surface, x + width);
    float top_y = surface_3ds_gl_translate_y(surface, y);
    float bottom_y = surface_3ds_gl_translate_y(surface, y + height);*/

    //printf("%d %d %f %f\n", x, y, top_y, bottom_y);

    //bottom_y = 240;
    //top_y = 240 - height;

    float box_quad[] = {
        /* top left / northwest */
        left_x, top_y, 0,                //
        red_f, green_f, blue_f, alpha_f, //
        -1.0, -1.0, -1.0,                //
        0, 0,                            //

        /* top right / northeast */
        right_x, top_y, 0,               //
        red_f, green_f, blue_f, alpha_f, //
        -1.0, -1.0, -1.0,                //
        0, 0,                            //

        /* bottom right / southeast */
        right_x, bottom_y, 0,            //
        red_f, green_f, blue_f, alpha_f, //
        -1.0, -1.0, -1.0,                //
        0, 0,                            //

        /* bottom left / southwest */
        left_x, bottom_y, 0,             //
        red_f, green_f, blue_f, alpha_f, //
        -1.0, -1.0, -1.0,                //
        0, 0,                            //
    };

    surface_3ds_gl_buffer_flat_quad(surface, box_quad, 0);
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
#endif

#ifdef _3DS
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
#endif

#ifdef RENDER_3DS_GL
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C3D_RenderTargetClear(mud->_3ds_gl_render_target, C3D_CLEAR_ALL, 0,
                          0);

    C3D_FrameDrawOn(mud->_3ds_gl_render_target);

    C3D_DrawElements(GPU_TRIANGLES, surface->_3ds_gl_flat_count * 6,
                     C3D_UNSIGNED_SHORT, surface->_3ds_gl_flat_ebo);

    C3D_FrameEnd(0);

    surface->_3ds_gl_flat_count = 0;
#endif

    // gspWaitForVBlank();
#endif

#if !defined(WII) && !defined(_3DS)
    if (mud->window != NULL) {
        SDL_BlitSurface(mud->pixel_surface, NULL, mud->screen, NULL);
        SDL_UpdateWindowSurface(mud->window);
    }
#endif

#ifdef RENDER_GL
    surface_gl_draw(surface, 0);
#endif
}

void surface_black_screen(Surface *surface) {
#ifdef RENDER_GL
    surface->gl_has_faded = 0;
    glClear(GL_COLOR_BUFFER_BIT);
#endif

#ifdef RENDER_SW
    int area = surface->width * surface->height;

    if (!surface->interlace) {
        memset(surface->pixels, 0, area * sizeof(int32_t));
        return;
    }

    int pixel_idx = 0;

    for (int y = -surface->height; y < 0; y += 2) {
        for (int x = -surface->width; x < 0; x++) {
            surface->pixels[pixel_idx++] = 0;
        }

        pixel_idx += surface->width;
    }
#endif
}

void surface_draw_circle_software(Surface *surface, int x, int y, int radius,
                                  int colour, int alpha) {
    int bg_alpha = 256 - alpha;
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
            int bg_red = ((pixel >> 16) & 0xff) * bg_alpha;
            int bg_green = ((pixel >> 8) & 0xff) * bg_alpha;
            int bg_blue = (pixel & 0xff) * bg_alpha;

            int new_colour = (((red + bg_red) >> 8) << 16) +
                             (((green + bg_green) >> 8) << 8) +
                             ((blue + bg_blue) >> 8);

            surface->pixels[index++] = new_colour;
        }
    }
}

void surface_draw_circle(Surface *surface, int x, int y, int radius, int colour,
                         int alpha, float depth) {
#ifdef RENDER_GL
    surface_gl_buffer_circle(surface, x, y, radius, colour, alpha, depth);
#endif

#ifdef RENDER_SW
    surface_draw_circle_software(surface, x, y, radius, colour, alpha);
#endif
}

void surface_draw_box_alpha(Surface *surface, int x, int y, int width,
                            int height, int colour, int alpha) {
#ifdef RENDER_GL
    surface_gl_buffer_box(surface, x, y, width, height, colour, alpha);
#endif

#ifdef RENDER_3DS_GL
    surface_3ds_gl_buffer_box(surface, x, y, width, height, colour, alpha);
#endif

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
#endif
}

void surface_draw_gradient(Surface *surface, int x, int y, int width,
                           int height, int top_colour, int bottom_colour) {
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

#ifdef RENDER_GL
    GLfloat left_x = surface_gl_translate_x(surface, x);
    GLfloat right_x = surface_gl_translate_x(surface, x + width);
    GLfloat top_y = surface_gl_translate_y(surface, y);
    GLfloat bottom_y = surface_gl_translate_y(surface, y + height);

    GLfloat gradient_quad[] = {
        /* top left / northwest */
        left_x, top_y, 0,   //
        top_red / 255.0f,   //
        top_green / 255.0f, //
        top_blue / 255.0f,  //
        1.0,                //
        -1.0, -1.0, -1.0,   //
        0, 0, -1,           //

        /* top right / northeast */
        right_x, top_y, 0,  //
        top_red / 255.0f,   //
        top_green / 255.0f, //
        top_blue / 255.0f,  //
        1.0,                //
        -1.0, -1.0, -1.0,   //
        0, 0, -1,           //

        /* bottom right / southeast */
        right_x, bottom_y, 0,  //
        bottom_red / 255.0f,   //
        bottom_green / 255.0f, //
        bottom_blue / 255.0f,  //
        1.0,                   //
        -1.0, -1.0, -1.0,      //
        0, 0, -1,              //

        /* bottom left / southwest */
        left_x, bottom_y, 0,   //
        bottom_red / 255.0f,   //
        bottom_green / 255.0f, //
        bottom_blue / 255.0f,  //
        1.0,                   //
        -1.0, -1.0, -1.0,      //
        0, 0, -1               //
    };

    surface_gl_buffer_flat_quad(surface, gradient_quad, 0);
#endif

#ifdef RENDER_SW
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
#endif
}

void surface_draw_box_software(Surface *surface, int x, int y, int width,
                               int height, int colour) {
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
}

void surface_draw_box(Surface *surface, int x, int y, int width, int height,
                      int colour) {
#ifdef RENDER_GL
    surface_gl_buffer_box(surface, x, y, width, height, colour, 255);
#endif

#ifdef RENDER_3DS_GL
    surface_3ds_gl_buffer_box(surface, x, y, width, height, colour, 255);
#endif

#ifdef RENDER_SW
    surface_draw_box_software(surface, x, y, width, height, colour);
#endif
}

void surface_draw_line_horizontal_software(Surface *surface, int x, int y,
                                           int width, int colour) {
#if defined(RENDER_GL) || defined(RENDER_SW)
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
#endif
}

void surface_draw_line_horizontal(Surface *surface, int x, int y, int width,
                                  int colour) {
#ifdef RENDER_GL
    surface_gl_buffer_box(surface, x, y, width, 1, colour, 255);
#endif

#ifdef RENDER_SW
    surface_draw_line_horizontal_software(surface, x, y, width, colour);
#endif
}

void surface_draw_line_vertical_software(Surface *surface, int x, int y,
                                         int height, int colour) {
#if defined(RENDER_GL) || defined(RENDER_SW)
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
#endif
}

void surface_draw_line_vertical(Surface *surface, int x, int y, int height,
                                int colour) {
#ifdef RENDER_GL
    surface_gl_buffer_box(surface, x, y, 1, height, colour, 255);
#endif

#ifdef RENDER_SW
    surface_draw_line_vertical_software(surface, x, y, height, colour);
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
#ifdef RENDER_GL
    // TODO this leaves some sort of residue. https://imgur.com/a/35sqk7v
    /*surface_gl_buffer_box(surface, 0, 0, surface->width, surface->height,
                          BLACK, 16);*/

    if (!surface->gl_has_faded) {
        surface_gl_update_framebuffer(surface);
        surface_fade_to_black_software(surface, surface->gl_screen_pixels, 1);
        surface_gl_update_framebuffer_texture(surface);
    }

    surface_gl_buffer_framebuffer_quad(surface);

    surface->gl_fade_to_black = 1;

    glClear(GL_COLOR_BUFFER_BIT);
#endif

#ifdef RENDER_SW
    surface_fade_to_black_software(surface, surface->pixels, 0);
#endif
}

void surface_draw_blur_software(Surface *surface, int32_t *dest, int j, int x,
                                int y, int width, int height, int add_alpha) {
    for (int xx = x; xx < x + width; xx++) {
        for (int yy = y; yy < y + height; yy++) {
            int r = 0;
            int g = 0;
            int b = 0;
            int a = 0;

            for (int i2 = xx; i2 <= xx; i2++) {
                if (i2 >= 0 && i2 < surface->width) {
                    for (int j2 = yy - j; j2 <= yy + j; j2++) {
                        if (j2 >= 0 && j2 < surface->height) {
                            int32_t pixel = dest[i2 + surface->width * j2];

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

void surface_draw_blur(Surface *surface, int j, int x, int y, int width,
                       int height) {
#ifdef RENDER_GL
    surface_gl_draw(surface, 0);

    surface_gl_update_framebuffer(surface);

    surface_draw_blur_software(surface, surface->gl_screen_pixels, j, x, y,
                               width, height, 1);

    surface_gl_update_framebuffer_texture(surface);
    surface_gl_buffer_framebuffer_quad(surface);
#endif

#ifdef RENDER_SW
    surface_draw_blur_software(surface, surface->pixels, j, x, y, width, height,
                               1);
#endif
}

void surface_apply_login_filter(Surface *surface, int background_height) {
    surface_fade_to_black(surface);

#ifdef RENDER_GL
    surface_gl_draw(surface, 0);
#endif

    surface_fade_to_black(surface);

    surface_draw_box(surface, 0, 0, surface->width, 6, BLACK);

    for (int i = 6; i >= 1; i--) {
        surface_draw_blur(surface, i, 0, i, surface->width, 8);
    }

    surface_draw_box(surface, 0, background_height - 6, surface->width, 20,
                     BLACK);

    for (int i = 6; i >= 1; i--) {
        surface_draw_blur(surface, i, 0, background_height - 6 - i,
                          surface->width, 8);
    }
}

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
    int index_offset = get_unsigned_short(sprite_data, 0);

    int full_width = get_unsigned_short(index_data, index_offset);
    index_offset += 2;

    int full_height = get_unsigned_short(index_data, index_offset);
    index_offset += 2;

    int colour_count = index_data[index_offset++] & 0xff;

    int32_t *colours = calloc(colour_count, sizeof(int32_t));
    colours[0] = MAGENTA;

    for (int i = 0; i < colour_count - 1; i++) {
        colours[i + 1] = ((index_data[index_offset] & 0xff) << 16) +
                         ((index_data[index_offset + 1] & 0xff) << 8) +
                         (index_data[index_offset + 2] & 0xff);

        index_offset += 3;
    }

    int sprite_offset = 2;

    for (int i = sprite_id; i < sprite_id + frame_count; i++) {
        surface->sprite_translate_x[i] = index_data[index_offset++] & 0xff;
        surface->sprite_translate_y[i] = index_data[index_offset++] & 0xff;

        surface->sprite_width[i] = get_unsigned_short(index_data, index_offset);
        index_offset += 2;

        surface->sprite_height[i] =
            get_unsigned_short(index_data, index_offset);

        index_offset += 2;

        int type = index_data[index_offset++] & 0xff;
        int area = surface->sprite_width[i] * surface->sprite_height[i];

        surface->sprite_colours[i] = calloc(area, sizeof(int8_t));
        surface->sprite_palette[i] = colours;
        surface->sprite_width_full[i] = full_width;
        surface->sprite_height_full[i] = full_height;

        free(surface->surface_pixels[i]);
        surface->surface_pixels[i] = NULL;

        surface->sprite_translate[i] = 0;

        if (surface->sprite_translate_x[i] != 0 ||
            surface->sprite_translate_y[i] != 0) {
            surface->sprite_translate[i] = 1;
        }

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
    }

    free(sprite_data);

#ifdef RENDER_GL
    for (int i = sprite_id; i < sprite_id + frame_count; i++) {
        GLuint texture_array_id =
            surface_gl_sprite_texture_array_id(surface, i);

        if (texture_array_id == 0) {
            continue;
        }

        int32_t *pixels = surface_palette_sprite_to_raster(surface, i, 1);

        int sprite_width = surface->sprite_width[i];
        int sprite_height = surface->sprite_height[i];
        int translate_x = surface->sprite_translate_x[i];
        int translate_y = surface->sprite_translate_y[i];

        int texture_index = surface_gl_sprite_texture_index(surface, i);

        int texture_width =
            surface_gl_sprite_texture_width(surface, texture_array_id);

        int texture_height =
            surface_gl_sprite_texture_height(surface, texture_array_id);

        int32_t *texture_pixels =
            calloc(texture_width * texture_height, sizeof(int32_t));

        for (int x = 0; x < sprite_width; x++) {
            for (int y = 0; y < sprite_height; y++) {
                texture_pixels[(x + translate_x) +
                               (y + translate_y) * texture_width] =
                    pixels[x + y * sprite_width];
            }
        }

        gl_update_texture_array(texture_array_id, texture_index, texture_width,
                                texture_height, texture_pixels, 1);

        free(pixels);
        free(texture_pixels);
    }
#endif
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
    int pixel_offset = 0;

    for (pixel_offset = 0; pixel_offset < SLEEP_WIDTH;) {
        int length = sprite_data[packet_offset++] & 0xff;

        for (int i = 0; i < length; i++) {
            pixels[pixel_offset++] = colour;
        }

        /* alternate between black and white */
        colour = WHITE - colour;
    }

    for (int y = 1; y < SLEEP_HEIGHT; y++) {
        for (int x = 0; x < SLEEP_WIDTH;) {
            int length = sprite_data[packet_offset++] & 0xff;

            for (int i = 0; i < length; i++) {
                pixels[pixel_offset] = pixels[pixel_offset - SLEEP_WIDTH];
                pixel_offset++;
                x++;
            }

            if (x < SLEEP_WIDTH) {
                pixels[pixel_offset] =
                    WHITE - pixels[pixel_offset - SLEEP_WIDTH];

                pixel_offset++;
                x++;
            }
        }
    }

#ifdef RENDER_GL
    GLuint texture_array_id =
        surface_gl_sprite_texture_array_id(surface, sprite_id);

    int texture_index = surface_gl_sprite_texture_index(surface, sprite_id);

    int texture_width =
        surface_gl_sprite_texture_width(surface, texture_array_id);

    int texture_height =
        surface_gl_sprite_texture_height(surface, texture_array_id);

    int32_t *texture_pixels =
        calloc(texture_width * texture_height, sizeof(int32_t));

    for (int x = 0; x < SLEEP_WIDTH; x++) {
        for (int y = 0; y < SLEEP_HEIGHT; y++) {
            texture_pixels[x + y * texture_width] =
                pixels[x + y * SLEEP_WIDTH] + 0xff000000;
        }
    }

    gl_update_texture_array(texture_array_id, texture_index, texture_width,
                            texture_height, texture_pixels, 1);

    free(texture_pixels);
#endif
}

void surface_screen_raster_to_palette_sprite(Surface *surface, int sprite_id) {
#ifdef RENDER_GL
    if (sprite_id == surface->mud->sprite_logo ||
        sprite_id == surface->mud->sprite_logo + 1 ||
        sprite_id == surface->mud->sprite_media + 10) {
        surface_gl_update_framebuffer(surface);

        int32_t *texture_pixels =
            calloc(MEDIA_TEXTURE_WIDTH * MEDIA_TEXTURE_HEIGHT, sizeof(int32_t));

        for (int x = 0; x < MEDIA_TEXTURE_WIDTH; x++) {
            for (int y = 0; y < MEDIA_TEXTURE_HEIGHT; y++) {
                texture_pixels[x + y * MEDIA_TEXTURE_WIDTH] =
                    surface->gl_screen_pixels[x + y * surface->width];
            }
        }

        int texture_index = surface_gl_sprite_texture_index(surface, sprite_id);

        gl_update_texture_array(surface->gl_sprite_media_textures,
                                texture_index, MEDIA_TEXTURE_WIDTH,
                                MEDIA_TEXTURE_HEIGHT, texture_pixels, 0);

        free(texture_pixels);
    }
#endif

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
    surface->surface_pixels[sprite_id] =
        surface_palette_sprite_to_raster(surface, sprite_id, 0);

    free(surface->sprite_colours[sprite_id]);
    surface->sprite_colours[sprite_id] = NULL;
}

void surface_screen_raster_to_sprite(Surface *surface, int sprite_id, int x,
                                     int y, int width, int height) {
#if defined(RENDER_GL) || defined(RENDER_SW)
    surface->sprite_width[sprite_id] = width;
    surface->sprite_height[sprite_id] = height;
    surface->sprite_translate[sprite_id] = 0;
    surface->sprite_translate_x[sprite_id] = 0;
    surface->sprite_translate_y[sprite_id] = 0;
    surface->sprite_width_full[sprite_id] = width;
    surface->sprite_height_full[sprite_id] = height;

    free(surface->surface_pixels[sprite_id]);

    surface->surface_pixels[sprite_id] =
        calloc(width * height, sizeof(int32_t));

    int pixel = 0;

    for (int yy = y; yy < y + height; yy++) {
        for (int xx = x; xx < x + width; xx++) {
            surface->surface_pixels[sprite_id][pixel++] =
                surface->pixels[xx + yy * surface->width];
        }
    }

    free(surface->sprite_colours[sprite_id]);
    surface->sprite_colours[sprite_id] = NULL;

    free(surface->sprite_palette[sprite_id]);
    surface->sprite_palette[sprite_id] = NULL;
#endif
}

// TODO not draw - load from raster reversed
void surface_draw_sprite_reversed(Surface *surface, int sprite_id, int x, int y,
                                  int width, int height) {
#if defined(RENDER_GL) || defined(RENDER_SW)
    surface->sprite_width[sprite_id] = width;
    surface->sprite_height[sprite_id] = height;
    surface->sprite_translate[sprite_id] = 0;
    surface->sprite_translate_x[sprite_id] = 0;
    surface->sprite_translate_y[sprite_id] = 0;
    surface->sprite_width_full[sprite_id] = width;
    surface->sprite_height_full[sprite_id] = height;

    free(surface->surface_pixels[sprite_id]);

    surface->surface_pixels[sprite_id] =
        calloc(width * height, sizeof(int32_t));

#ifdef RENDER_GL
    int32_t *texture_pixels = calloc(width * height, sizeof(int32_t));
#endif

    int index = 0;

    for (int xx = x; xx < x + width; xx++) {
        for (int yy = y; yy < y + height; yy++) {
            int colour = surface->pixels[xx + yy * surface->width];

#ifdef RENDER_GL
            texture_pixels[index] = colour + 0xff000000;
#endif

            surface->surface_pixels[sprite_id][index++] = colour;
        }
    }

    free(surface->sprite_colours[sprite_id]);
    surface->sprite_colours[sprite_id] = NULL;

    free(surface->sprite_palette[sprite_id]);
    surface->sprite_palette[sprite_id] = NULL;

#ifdef RENDER_GL
    GLuint texture_array_id =
        surface_gl_sprite_texture_array_id(surface, sprite_id);

    if (texture_array_id == 0) {
        return;
    }

    gl_update_texture_array(texture_array_id, 0, width, height, texture_pixels,
                            1);

    free(texture_pixels);
#endif
#endif
}

/* used for texture loading on GL rendering */
void surface_draw_sprite_from3_software(Surface *surface, int x, int y,
                                        int sprite_id) {
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
}

void surface_draw_sprite_from3(Surface *surface, int x, int y, int sprite_id) {
#ifdef RENDER_GL
    surface_gl_buffer_sprite(surface, sprite_id, x, y, -1, -1, 0, 0, 0, 255, 0,
                             0, 0, 0);
#endif

#ifdef RENDER_SW
    surface_draw_sprite_from3_software(surface, x, y, sprite_id);
#endif
}

void surface_draw_sprite_from3_depth(Surface *surface, int x, int y,
                                     int sprite_id, float depth_top,
                                     float depth_bottom) {
#ifdef RENDER_GL
    surface_gl_buffer_sprite(surface, sprite_id, x, y, -1, -1, 0, 0, 0, 255, 0,
                             0, depth_top, depth_bottom);
#endif

#ifdef RENDER_SW
    surface_draw_sprite_from3_software(surface, x, y, sprite_id);
#endif
}

void surface_draw_sprite_scaled(Surface *surface, int x, int y, int width,
                                int height, int sprite_id, float depth) {
#ifdef RENDER_GL
    surface_gl_buffer_sprite(surface, sprite_id, x, y, width, height, 0, 0, 0,
                             255, 0, 0, depth, depth);
#endif

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
#endif
}

void surface_draw_entity_sprite(Surface *surface, int x, int y, int width,
                                int height, int sprite_id, int tx, int ty,
                                float depth_top, float depth_bottom) {
    if (sprite_id >= 50000) {
#ifdef RENDER_GL
        float depth = ((depth_top + depth_bottom) / 2) -
                      ANIMATION_COUNT * surface_gl_get_layer_depth(surface);
#else
        float depth = 0.0f;
#endif

        mudclient_draw_teleport_bubble(surface->mud, x, y, width, height,
                                       sprite_id - 50000, depth);

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

    surface_draw_sprite_scaled(surface, x, y, width, height, sprite_id,
                               (depth_top + depth_bottom) / 2);
}

void surface_draw_sprite_alpha_from4(Surface *surface, int x, int y,
                                     int sprite_id, int alpha) {
#ifdef RENDER_GL
    surface_gl_buffer_sprite(surface, sprite_id, x, y, -1, -1, 0, 0, 0, alpha,
                             0, 0, 0, 0);
#endif

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
#endif
}

void surface_draw_action_bubble(Surface *surface, int x, int y, int scale_x,
                                int scale_y, int sprite_id, int alpha) {
#ifdef RENDER_GL
    surface_gl_buffer_sprite(surface, sprite_id, x, y, scale_x, scale_y, 0, 0,
                             0, alpha, 0, 0, 0, 0);
#endif

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
#endif
}

// surface_draw_sprite_scale_mask
/* only works with palette sprites */
void surface_sprite_clipping_from6(Surface *surface, int x, int y, int width,
                                   int height, int sprite_id, int colour) {
#ifdef RENDER_GL
    surface_gl_buffer_sprite(surface, sprite_id, x, y, width, height, 0, colour,
                             0, 255, 0, 0, 0, 0);
#endif

#ifdef RENDER_SW
    int k1 = surface->sprite_width[sprite_id];
    int l1 = surface->sprite_height[sprite_id];
    int i2 = 0;
    int j2 = 0;
    int k2 = (k1 << 16) / width;
    int l2 = (l1 << 16) / height;

    if (surface->sprite_translate[sprite_id]) {
        int i3 = surface->sprite_width_full[sprite_id];
        int k3 = surface->sprite_height_full[sprite_id];

        k2 = (i3 << 16) / width;
        l2 = (k3 << 16) / height;

        x += (surface->sprite_translate_x[sprite_id] * width + i3 - 1) / i3;
        y += (surface->sprite_translate_y[sprite_id] * height + k3 - 1) / k3;

        if ((surface->sprite_translate_x[sprite_id] * width) % i3 != 0) {
            i2 = ((i3 - ((surface->sprite_translate_x[sprite_id] * width) % i3))
                  << 16) /
                 width;
        }

        if ((surface->sprite_translate_y[sprite_id] * height) % k3 != 0) {
            j2 =
                ((k3 - ((surface->sprite_translate_y[sprite_id] * height) % k3))
                 << 16) /
                height;
        }

        width = (width * (surface->sprite_width[sprite_id] - (i2 >> 16))) / i3;

        height =
            (height * (surface->sprite_height[sprite_id] - (j2 >> 16))) / k3;
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

    surface_plot_scale_from14(surface->pixels,
                              surface->surface_pixels[sprite_id], i2, j2, j3,
                              l3, width, height, k2, l2, k1, y_inc, colour);
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

#ifdef RENDER_GL
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

void surface_sprite_clipping_from9_software(Surface *surface, int x, int y,
                                            int draw_width, int draw_height,
                                            int sprite_id, int mask_colour,
                                            int skin_colour, int skew_x,
                                            int flip) {
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

// surface_draw_sprite_transform_mask
/* applies scale, both grey and skin colour masks, skew/shear and flip. used for
 * entity sprites */
void surface_sprite_clipping_from9(Surface *surface, int x, int y,
                                   int draw_width, int draw_height,
                                   int sprite_id, int mask_colour,
                                   int skin_colour, int skew_x, int flip) {
#ifdef RENDER_GL
    surface_gl_buffer_sprite(surface, sprite_id, x, y, draw_width, draw_height,
                             skew_x, mask_colour, skin_colour, 255, flip, 0, 0,
                             0);
#endif

#ifdef RENDER_SW
    surface_sprite_clipping_from9_software(surface, x, y, draw_width,
                                           draw_height, sprite_id, mask_colour,
                                           skin_colour, skew_x, flip);
#endif
}

void surface_sprite_clipping_from9_depth(Surface *surface, int x, int y,
                                         int draw_width, int draw_height,
                                         int sprite_id, int mask_colour,
                                         int skin_colour, int skew_x, int flip,
                                         float depth_top, float depth_bottom) {
#ifdef RENDER_GL
    surface_gl_buffer_sprite(surface, sprite_id, x, y, draw_width, draw_height,
                             skew_x, mask_colour, skin_colour, 255, flip, 0,
                             depth_top, depth_bottom);
#endif

#ifdef RENDER_SW
    surface_sprite_clipping_from9_software(surface, x, y, draw_width,
                                           draw_height, sprite_id, mask_colour,
                                           skin_colour, skew_x, flip);
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

                    int j3 = (colour >> 16) & 0xff;
                    int k3 = (colour >> 8) & 0xff;
                    int l3 = colour & 0xff;

                    if (j3 == k3 && k3 == l3) {
                        dest[k6 + l] = (((j3 * mask_r) >> 8) << 16) +
                                       (((k3 * mask_g) >> 8) << 8) +
                                       ((l3 * mask_b) >> 8);
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

void surface_draw_character(Surface *surface, int character_offset, int x,
                            int y, int colour, int8_t *font_data) {
#ifdef RENDER_SW
    /* baseline and kerning offsets */
    int draw_x = x + font_data[character_offset + 5];
    int draw_y = y - font_data[character_offset + 6];

    int width = font_data[character_offset + 3];
    int height = font_data[character_offset + 4];

    /* position of pixel data for the font (on/off) */
    int font_pos = font_data[character_offset] * 16384 +
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
#endif
}

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

void surface_draw_string_depth(Surface *surface, char *text, int x, int y,
                               int font, int colour, float depth) {
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

            if (strcmp(sliced, "red") == 0) {
                colour = STRING_RED;
            } else if (strcmp(sliced, "lre") == 0) {
                colour = STRING_LRE;
            } else if (strcmp(sliced, "yel") == 0) {
                colour = STRING_YEL;
            } else if (strcmp(sliced, "gre") == 0) {
                colour = STRING_GRE;
            } else if (strcmp(sliced, "blu") == 0) {
                colour = STRING_BLU;
            } else if (strcmp(sliced, "cya") == 0) {
                colour = STRING_CYA;
            } else if (strcmp(sliced, "mag") == 0) {
                colour = STRING_MAG;
            } else if (strcmp(sliced, "whi") == 0) {
                colour = STRING_WHI;
            } else if (strcmp(sliced, "bla") == 0) {
                colour = STRING_BLA;
            } else if (strcmp(sliced, "dre") == 0) {
                colour = STRING_DRE;
            } else if (strcmp(sliced, "ora") == 0) {
                colour = STRING_ORA;
            } else if (strcmp(sliced, "ran") == 0) {
                colour =
                    (int)(((float)rand() / (float)RAND_MAX) * (float)WHITE);
            } else if (strcmp(sliced, "or1") == 0) {
                colour = STRING_OR1;
            } else if (strcmp(sliced, "or2") == 0) {
                colour = STRING_OR2;
            } else if (strcmp(sliced, "or3") == 0) {
                colour = STRING_OR3;
            } else if (strcmp(sliced, "gr1") == 0) {
                colour = STRING_GR1;
            } else if (strcmp(sliced, "gr2") == 0) {
                colour = STRING_GR2;
            } else if (strcmp(sliced, "gr3") == 0) {
                colour = STRING_GR3;
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

#ifdef RENDER_GL
            surface_gl_buffer_character(surface, text[i], x, y, colour, font,
                                        draw_shadow, depth);
#endif

#ifdef RENDER_SW
            if (draw_shadow) {
                surface_draw_character(surface, character_offset, x + 1, y,
                                       BLACK, font_data);

                surface_draw_character(surface, character_offset, x, y + 1,
                                       BLACK, font_data);
            }

            surface_draw_character(surface, character_offset, x, y, colour,
                                   font_data);
#endif

            x += font_data[character_offset + 7];
        }
    }
}

void surface_draw_string(Surface *surface, char *text, int x, int y, int font,
                         int colour) {
    surface_draw_string_depth(surface, text, x, y, font, colour, 0);
}

void surface_draw_string_right(Surface *surface, char *text, int x, int y,
                               int font, int colour) {
    surface_draw_string(surface, text, x - surface_text_width(text, font), y,
                        font, colour);
}

void surface_draw_string_centre_depth(Surface *surface, char *text, int x,
                                      int y, int font, int colour,
                                      float depth) {
    surface_draw_string_depth(surface, text,
                              x - (int)(surface_text_width(text, font) / 2), y,
                              font, colour, depth);
}

void surface_draw_string_centre(Surface *surface, char *text, int x, int y,
                                int font, int colour) {
    surface_draw_string(surface, text,
                        x - (int)(surface_text_width(text, font) / 2), y, font,
                        colour);
}

void surface_draw_paragraph(Surface *surface, char *text, int x, int y,
                            int font, int colour, int max) {
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

int surface_text_height(int font_id) {
    switch (font_id) {
    case 0:
        return 12;
    case 1:
        return 14;
    case 2:
        return 14;
    case 3:
        return 15;
    case 4:
        return 15;
    case 5:
        return 19;
    case 6:
        return 24;
    case 7:
        return 29;
    default:
        return surface_text_height_font(font_id);
    }
}

int surface_text_height_font(int font_id) {
    if (font_id == 0) {
        return game_fonts[font_id][8] - 2;
    }

    return game_fonts[font_id][8] - 1;
}

int surface_text_width(char *text, int font_id) {
    int total = 0;
    int8_t *font = game_fonts[font_id];
    int text_length = strlen(text);

    for (int i = 0; i < text_length; i++) {
        if (text[i] == '@' && i + 4 < text_length && text[i + 4] == '@') {
            i += 4;
        } else if (text[i] == '~' && i + 4 < text_length &&
                   text[i + 4] == '~') {
            i += 4;
        } else {
            total += font[character_width[(unsigned)text[i]] + 7];
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
    surface_sprite_clipping_from9(
        surface, x, y, slot_width - 1, slot_height - 2,
        surface->mud->sprite_item + game_data_item_sprite[item_id],
        game_data_item_mask[item_id], 0, 0, 0);
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
                } else if (game_data_item_stackable[item_id] == 0) {
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
    surface_draw_sprite_from3(surface, x + 1, y + 1, surface->mud->sprite_util);

    /* down arrow */
    surface_draw_sprite_from3(surface, x + 1, y + height - 12,
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

void surface_draw_status_bar(Surface *surface, int min, int max, int current,
                             char *label, int x, int y, int width, int height,
                             int background_colour, int foreground_colour) {
    int current_width = (current / (float)max) * width;

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
