#include "surface.h"

int an_int_346 = 0;
int an_int_347 = 0;
int an_int_348 = 0;

int8_t *game_fonts[50];
int character_width[256];

int32_t *surface_texture_pixels;

void init_surface_global() {
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

int surface_rgb_to_int(int r, int g, int b) { return (r << 16) + (g << 8) + b; }

void create_font(int8_t *buffer, int id) { game_fonts[id] = buffer; }

void surface_new(Surface *surface, int width, int height, int limit,
                 mudclient *mud) {
    memset(surface, 0, sizeof(Surface));

    surface->limit = limit;
    surface->bounds_bottom_y = height;
    surface->bounds_bottom_x = width;
    surface->width1 = width;
    surface->width2 = width;
    surface->height1 = height;
    surface->height2 = height;
    surface->area = width * height;

#ifdef WII
    surface->pixels = calloc(width * height, sizeof(int32_t));
#endif

#ifdef _3DS
    surface->pixels = calloc(width * height, sizeof(int32_t));
#endif

#if !defined(WII) && !defined(_3DS)
    surface->pixels = mud->pixel_surface->pixels;
#endif

    surface->surface_pixels = calloc(limit, sizeof(int32_t *));
    surface->sprite_colours_used = calloc(limit, sizeof(int8_t *));
    surface->sprite_colour_list = calloc(limit, sizeof(int32_t *));
    surface->sprite_width = calloc(limit, sizeof(int));
    surface->sprite_height = calloc(limit, sizeof(int));
    surface->sprite_width_full = calloc(limit, sizeof(int));
    surface->sprite_height_full = calloc(limit, sizeof(int));
    surface->sprite_translate = calloc(limit, sizeof(int8_t));
    surface->sprite_translate_x = calloc(limit, sizeof(int));
    surface->sprite_translate_y = calloc(limit, sizeof(int));

    surface->logged_in = 0;
    surface->interlace = 0;

    surface->mud = mud;

#ifdef RENDER_GL
    shader_new(&surface->flat_shader, "./flat.vs", "./flat.fs");

    glGenVertexArrays(1, &surface->flat_vao);
    glBindVertexArray(surface->flat_vao);

    glGenBuffers(1, &surface->flat_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, surface->flat_vbo);

    /* 256 quads */
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 256 * 4, NULL,
                 GL_DYNAMIC_DRAW);

    /* vertices { x, y } */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);

    glEnableVertexAttribArray(0);

    /* colours { r, g, b, a } */
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(2 * sizeof(float)));

    glEnableVertexAttribArray(1);

    glGenBuffers(1, &surface->flat_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface->flat_ebo);

    /* two triangles per quad (6 vertex indices) */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6 * 256, NULL,
                 GL_DYNAMIC_DRAW);
#endif
}

#ifdef RENDER_GL
void surface_buffer_flat_quad(Surface *surface, GLfloat *quad) {
    glBindBuffer(GL_ARRAY_BUFFER, surface->flat_vbo);

    glBufferSubData(GL_ARRAY_BUFFER,
                    sizeof(GLfloat) * surface->flat_count * 6 * 4,
                    sizeof(GLfloat) * 6 * 4, quad);

    GLuint index = surface->flat_count * 4;

    GLuint indices[] = {index, index + 1, index + 2,
                        index, index + 2, index + 3};

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface->flat_ebo);

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(indices) * surface->flat_count, sizeof(indices),
                    indices);

    surface->flat_count += 1;
}
#endif

void surface_set_bounds(Surface *surface, int x1, int y1, int x2, int y2) {
    if (x1 < 0) {
        x1 = 0;
    }

    if (y1 < 0) {
        y1 = 0;
    }

    if (x2 > surface->width2) {
        x2 = surface->width2;
    }

    if (y2 > surface->height2) {
        y2 = surface->height2;
    }

    surface->bounds_top_x = x1;
    surface->bounds_top_y = y1;
    surface->bounds_bottom_x = x2;
    surface->bounds_bottom_y = y2;
}

void surface_reset_bounds(Surface *surface) {
    surface->bounds_top_x = 0;
    surface->bounds_top_y = 0;
    surface->bounds_bottom_x = surface->width2;
    surface->bounds_bottom_y = surface->height2;
}

void surface_draw(Surface *surface) {
    mudclient *mud = surface->mud;

#ifdef WII
    uint8_t *fb = mud->framebuffer;
    uint8_t *pixels = (uint8_t *)surface->pixels;
    int index = 0;

    // 64,54

    for (int y = 0; y < surface->height1; y++) {
        for (int x = 0; x < surface->width1; x += 2) {
            // int index = ((y  * surface->width1) + x) * 4;

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

    // VIDEO_WaitVSync();
#endif

#ifdef _3DS
    uint8_t *pixels = (uint8_t *)surface->pixels;

    if (mud->r_down) {
        for (int y = 0; y < 240; y++) {
            for (int x = 0; x < 400; x++) {
                int fb_index = ((x * 240) + (240 - y)) * 3;

                int pixel_index =
                    (((y + mud->zoom_offset_y) * surface->width1) +
                     (x + mud->zoom_offset_x)) *
                    4;

                memcpy(mud->framebuffer_top + fb_index, pixels + pixel_index,
                       3);
            }
        }

        // gspWaitForVBlank();
    }

    uint8_t *fb = NULL;
    int offset_x = 0;
    int offset_y = 0;

    if (mud->keyboard_open) {
        if (mud->r_down) {
            return;
        }

        fb = mud->framebuffer_top;
        offset_x = 72 + 1;
        offset_y = (57 * 2) + 1;
    } else {
        fb = mud->framebuffer_bottom;
        offset_x = 32;
        offset_y = (6 * 2) + 1;
    }

    int index = 0;

    for (int y = 0; y < surface->height1; y++) {
        for (int x = 0; x < surface->width1 / 2; x++) {
            if (mud->interlace) {
                if (y % 2 == 1) {
                    index += 8;
                    continue;
                }

                int fb_index =
                    (((x + offset_x) * 240) + (240 - ((y + offset_y) / 2))) * 3;

                memcpy(fb + fb_index, pixels + index, 3);

                index += 8;
            } else {
                int b1 = pixels[index];
                int g1 = pixels[index + 1];
                int r1 = pixels[index + 2];

                index += 4;

                int b2 = pixels[index];
                int g2 = pixels[index + 1];
                int r2 = pixels[index + 2];

                index += 4;

                int fb_index =
                    (((x + offset_x) * 240) + (240 - ((y + offset_y) / 2))) * 3;

                if (y % 2 == 1) {
                    fb[fb_index] = (fb[fb_index] + b1 + b2) / 3;
                    fb[fb_index + 1] = (fb[fb_index + 1] + g1 + g2) / 3;
                    fb[fb_index + 2] = (fb[fb_index + 2] + r1 + r2) / 3;
                } else {
                    fb[fb_index] = (b1 + b2) / 2;
                    fb[fb_index + 1] = (g1 + g2) / 2;
                    fb[fb_index + 2] = (r1 + r2) / 2;
                }
            }
        }
    }

    // gspWaitForVBlank();
#endif

#if !defined(WII) && !defined(_3DS)
    if (mud->window == NULL) {
        return;
    }

    SDL_BlitSurface(mud->pixel_surface, NULL, mud->screen, NULL);
    SDL_UpdateWindowSurface(mud->window);
#endif

#ifdef RENDER_GL
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_use(&surface->flat_shader);

    glBindVertexArray(surface->flat_vao);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, surface->flat_count * 6, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(mud->gl_window);
#endif

    surface->flat_count = 0;
}

void surface_black_screen(Surface *surface) {
    int area = surface->width2 * surface->height2;

    if (!surface->interlace) {
        for (int i = 0; i < area; i++) {
            surface->pixels[i] = 0;
        }

        return;
    }

    int pixel_idx = 0;

    for (int y = -surface->height2; y < 0; y += 2) {
        for (int x = -surface->width2; x < 0; x++) {
            surface->pixels[pixel_idx++] = 0;
        }

        pixel_idx += surface->width2;
    }
}

void surface_draw_circle(Surface *surface, int x, int y, int radius, int colour,
                         int alpha) {
    int bg_alpha = 256 - alpha;
    int red = ((colour >> 16) & 0xff) * alpha;
    int green = ((colour >> 8) & 0xff) * alpha;
    int blue = (colour & 0xff) * alpha;

    int top = y - radius;

    if (top < 0) {
        top = 0;
    }

    int bottom = y + radius;

    if (bottom >= surface->height2) {
        bottom = surface->height2 - 1;
    }

    int8_t vert_inc = 1;

    if (surface->interlace) {
        vert_inc = 2;

        if ((top & 1) != 0) {
            top++;
        }
    }

    for (int yy = top; yy <= bottom; yy += vert_inc) {
        int l3 = yy - y;
        int i4 = (int)sqrt(radius * radius - l3 * l3);
        int j4 = x - i4;

        if (j4 < 0) {
            j4 = 0;
        }

        int k4 = x + i4;

        if (k4 >= surface->width2) {
            k4 = surface->width2 - 1;
        }

        int pixel_idx = j4 + yy * surface->width2;

        for (int i = j4; i <= k4; i++) {
            int pixel = surface->pixels[pixel_idx];
            int bg_red = ((pixel >> 16) & 0xff) * bg_alpha;
            int bg_green = ((pixel >> 8) & 0xff) * bg_alpha;
            int bg_blue = (pixel & 0xff) * bg_alpha;

            int new_colour = (((red + bg_red) >> 8) << 16) +
                             (((green + bg_green) >> 8) << 8) +
                             ((blue + bg_blue) >> 8);

            surface->pixels[pixel_idx++] = new_colour;
        }
    }
}

void surface_draw_box_alpha(Surface *surface, int x, int y, int width,
                            int height, int colour, int alpha) {
    if (x < surface->bounds_top_x) {
        width -= surface->bounds_top_x - x;
        x = surface->bounds_top_x;
    }

    if (y < surface->bounds_top_y) {
        height -= surface->bounds_top_y - y;
        y = surface->bounds_top_y;
    }

    if (x + width > surface->bounds_bottom_x) {
        width = surface->bounds_bottom_x - x;
    }

    if (y + height > surface->bounds_bottom_y) {
        height = surface->bounds_bottom_y - y;
    }

#ifdef RENDER_GL
    float red_f = ((colour >> 16) & 0xff) / 255.0f;
    float green_f = ((colour >> 8) & 0xff) / 255.0f;
    float blue_f = (colour & 0xff) / 255.0f;
    float alpha_f = alpha / 255.0f;

    GLfloat left_x = translate_gl_x(x, surface->width2);
    GLfloat right_x = translate_gl_x(x + width, surface->width2);
    GLfloat top_y = translate_gl_y(y, surface->height2);
    GLfloat bottom_y = translate_gl_y(y + height, surface->height2);

    GLfloat box_quad[] = {
        /* top left / northwest */
        left_x, top_y,                   //
        red_f, green_f, blue_f, alpha_f, //

        /* top right / northeast */
        right_x, top_y,                  //
        red_f, green_f, blue_f, alpha_f, //

        /* bottom right / southeast */
        right_x, bottom_y,               //
        red_f, green_f, blue_f, alpha_f, //

        /* bottom left / southwest */
        left_x, bottom_y,               //
        red_f, green_f, blue_f, alpha_f //
    };

    surface_buffer_flat_quad(surface, box_quad);
#endif

#ifdef RENDER_SW
    int red = ((colour >> 16) & 0xff) * alpha;
    int green = ((colour >> 8) & 0xff) * alpha;
    int blue = (colour & 0xff) * alpha;
    int background_alpha = 256 - alpha;

    int offset_x = surface->width2 - width;
    int8_t vert_inc = 1;

    if (surface->interlace) {
        vert_inc = 2;
        offset_x += surface->width2;

        if ((y & 1) != 0) {
            y++;
            height--;
        }
    }

    int pixel_idx = x + y * surface->width2;

    for (int i = 0; i < height; i += vert_inc) {
        for (int j = -width; j < 0; j++) {
            int pixel = surface->pixels[pixel_idx];
            int background_red = ((pixel >> 16) & 0xff) * background_alpha;
            int background_green = ((pixel >> 8) & 0xff) * background_alpha;
            int background_blue = (pixel & 0xff) * background_alpha;

            int new_colour = (((red + background_red) >> 8) << 16) +
                             (((green + background_green) >> 8) << 8) +
                             ((blue + background_blue) >> 8);

            surface->pixels[pixel_idx++] = new_colour;
        }

        pixel_idx += offset_x;
    }
#endif
}

void surface_draw_gradient(Surface *surface, int x, int y, int width,
                           int height, int colour_top, int colour_bottom) {
    if (x < surface->bounds_top_x) {
        width -= surface->bounds_top_x - x;
        x = surface->bounds_top_x;
    }

    if (x + width > surface->bounds_bottom_x) {
        width = surface->bounds_bottom_x - x;
    }

    int bottom_red = (colour_bottom >> 16) & 0xff;
    int bottom_green = (colour_bottom >> 8) & 0xff;
    int bottom_blue = colour_bottom & 0xff;
    int top_red = (colour_top >> 16) & 0xff;
    int top_green = (colour_top >> 8) & 0xff;
    int top_blue = colour_top & 0xff;

#ifdef RENDER_GL
    GLfloat left_x = translate_gl_x(x, surface->width2);
    GLfloat right_x = translate_gl_x(x + width, surface->width2);
    GLfloat top_y = translate_gl_y(y, surface->height2);
    GLfloat bottom_y = translate_gl_y(y + height, surface->height2);

    GLfloat gradient_quad[] = {
        /* top left / northwest */
        left_x, top_y,      //
        top_red / 255.0f,   //
        top_green / 255.0f, //
        top_blue / 255.0f,  //
        1.0,                //

        /* top right / northeast */
        right_x, top_y,     //
        top_red / 255.0f,   //
        top_green / 255.0f, //
        top_blue / 255.0f,  //
        1.0,                //

        /* bottom right / southeast */
        right_x, bottom_y,     //
        bottom_red / 255.0f,   //
        bottom_green / 255.0f, //
        bottom_blue / 255.0f,  //
        1.0,                   //

        /* bottom left / southwest */
        left_x, bottom_y,      //
        bottom_red / 255.0f,   //
        bottom_green / 255.0f, //
        bottom_blue / 255.0f,  //
        1.0                    //
    };

    surface_buffer_flat_quad(surface, gradient_quad);
#endif

#ifdef RENDER_SW
    int i3 = surface->width2 - width; // TODO offset_x
    int8_t vert_inc = 1;

    if (surface->interlace) {
        vert_inc = 2;
        i3 += surface->width2;

        if ((y & 1) != 0) {
            y++;
            height--;
        }
    }

    int pixel_idx = x + y * surface->width2;

    for (int i = 0; i < height; i += vert_inc) {
        if (i + y >= surface->bounds_top_y &&
            i + y < surface->bounds_bottom_y) {
            int new_colour =
                (((bottom_red * i + top_red * (height - i)) / height) << 16) +
                (((bottom_green * i + top_green * (height - i)) / height)
                 << 8) +
                (((bottom_blue * i + top_blue * (height - i)) / height));

            for (int j = -width; j < 0; j++) {
                surface->pixels[pixel_idx++] = new_colour;
            }

            pixel_idx += i3;
        } else {
            pixel_idx += surface->width2;
        }
    }
#endif
}

void surface_draw_box(Surface *surface, int x, int y, int width, int height,
                      int colour) {
#ifdef RENDER_GL
    surface_draw_box_alpha(surface, x, y, width, height, colour, 255);
#endif

#ifdef RENDER_SW
    if (x < surface->bounds_top_x) {
        width -= surface->bounds_top_x - x;
        x = surface->bounds_top_x;
    }

    if (y < surface->bounds_top_y) {
        height -= surface->bounds_top_y - y;
        y = surface->bounds_top_y;
    }

    if (x + width > surface->bounds_bottom_x) {
        width = surface->bounds_bottom_x - x;
    }

    if (y + height > surface->bounds_bottom_y) {
        height = surface->bounds_bottom_y - y;
    }

    int j1 = surface->width2 - width;
    int8_t vert_inc = 1;

    if (surface->interlace) {
        vert_inc = 2;
        j1 += surface->width2;

        if ((y & 1) != 0) {
            y++;
            height--;
        }
    }

    int pixel_idx = x + y * surface->width2;

    for (int i = -height; i < 0; i += vert_inc) {
        for (int j = -width; j < 0; j++) {
            surface->pixels[pixel_idx++] = colour;
        }

        pixel_idx += j1;
    }
#endif
}

void surface_draw_line_horizontal(Surface *surface, int x, int y, int width,
                                  int colour) {
    if (y < surface->bounds_top_y || y >= surface->bounds_bottom_y) {
        return;
    }

    if (x < surface->bounds_top_x) {
        width -= surface->bounds_top_x - x;
        x = surface->bounds_top_x;
    }

    if (x + width > surface->bounds_bottom_x) {
        width = surface->bounds_bottom_x - x;
    }

#ifdef RENDER_GL
    GLfloat left_x = translate_gl_x(x, surface->width2);
    GLfloat right_x = translate_gl_x(x + width, surface->width2);
    GLfloat top_y = translate_gl_y(y, surface->height2);
    GLfloat bottom_y = translate_gl_y(y + 1, surface->height2);

    float red = ((colour >> 16) & 0xff) / 255.0f;
    float green = ((colour >> 8) & 0xff) / 255.0f;
    float blue = (colour & 0xff) / 255.0f;

    GLfloat box_quad[] = {
        /* top left / northwest */
        left_x, top_y,         //
        red, green, blue, 1.0, //

        /* top right / northeast */
        right_x, top_y,        //
        red, green, blue, 1.0, //

        /* bottom right / southeast */
        right_x, bottom_y,     //
        red, green, blue, 1.0, //

        /* bottom left / southwest */
        left_x, bottom_y,     //
        red, green, blue, 1.0 //
    };

    surface_buffer_flat_quad(surface, box_quad);
#endif

#ifdef RENDER_SW
    int start = x + y * surface->width2;

    for (int i = 0; i < width; i++) {
        surface->pixels[start + i] = colour;
    }
#endif
}

void surface_draw_line_vertical(Surface *surface, int x, int y, int height,
                                int colour) {
    if (x < surface->bounds_top_x || x >= surface->bounds_bottom_x) {
        return;
    }

    if (y < surface->bounds_top_y) {
        height -= surface->bounds_top_y - y;
        y = surface->bounds_top_y;
    }

    if (y + height > surface->bounds_bottom_x) {
        height = surface->bounds_bottom_y - y;
    }

#ifdef RENDER_GL
    GLfloat left_x = translate_gl_x(x, surface->width2);
    GLfloat right_x = translate_gl_x(x + 1, surface->width2);
    GLfloat top_y = translate_gl_y(y, surface->height2);
    GLfloat bottom_y = translate_gl_y(y + height, surface->height2);

    float red = ((colour >> 16) & 0xff) / 255.0f;
    float green = ((colour >> 8) & 0xff) / 255.0f;
    float blue = (colour & 0xff) / 255.0f;

    GLfloat box_quad[] = {
        /* top left / northwest */
        left_x, top_y,         //
        red, green, blue, 1.0, //

        /* top right / northeast */
        right_x, top_y,        //
        red, green, blue, 1.0, //

        /* bottom right / southeast */
        right_x, bottom_y,     //
        red, green, blue, 1.0, //

        /* bottom left / southwest */
        left_x, bottom_y,     //
        red, green, blue, 1.0 //
    };

    surface_buffer_flat_quad(surface, box_quad);
#endif

#ifdef RENDER_SW
    int start = x + y * surface->width2;

    for (int i = 0; i < height; i++) {
        surface->pixels[start + i * surface->width2] = colour;
    }
#endif
}

void surface_draw_box_edge(Surface *surface, int x, int y, int width,
                           int height, int colour) {
    surface_draw_line_horizontal(surface, x, y, width, colour);
    surface_draw_line_horizontal(surface, x, y + height - 1, width, colour);
    surface_draw_line_vertical(surface, x, y, height, colour);
    surface_draw_line_vertical(surface, x + width - 1, y, height, colour);
}

void surface_set_pixel(Surface *surface, int x, int y, int colour) {
    if (x < surface->bounds_top_x || y < surface->bounds_top_y ||
        x >= surface->bounds_bottom_x || y >= surface->bounds_bottom_y) {
        return;
    }

    surface->pixels[x + y * surface->width2] = colour;
}

void surface_fade_to_black(Surface *surface) {
    int area = surface->width2 * surface->height2;

    for (int i = 0; i < area; i++) {
        int32_t pixel = surface->pixels[i] & 0xffffff;

        surface->pixels[i] =
            ((pixel >> 1) & 0x7f7f7f) + ((pixel >> 2) & 0x3f3f3f) +
            ((pixel >> 3) & 0x1f1f1f) + ((pixel >> 4) & 0xf0f0f);
    }
}

void surface_draw_line_alpha(Surface *surface, int j, int x, int y, int width,
                             int height) {
    for (int xx = x; xx < x + width; xx++) {
        for (int yy = y; yy < y + height; yy++) {
            int r = 0;
            int g = 0;
            int b = 0;
            int a = 0;

            for (int i2 = xx; i2 <= xx; i2++) {
                if (i2 >= 0 && i2 < surface->width2) {
                    for (int j2 = yy - j; j2 <= yy + j; j2++) {
                        if (j2 >= 0 && j2 < surface->height2) {
                            int32_t pixel =
                                surface->pixels[i2 + surface->width2 * j2];

                            r += (pixel >> 16) & 0xff;
                            g += (pixel >> 8) & 0xff;
                            b += pixel & 0xff;
                            a++;
                        }
                    }
                }
            }

            surface->pixels[xx + surface->width2 * yy] =
                ((r / a) << 16) + ((g / a) << 8) + (b / a);
        }
    }
}

void surface_clear(Surface *surface) {
    for (int i = 0; i < surface->limit; i++) {
        free(surface->surface_pixels[i]);
        surface->surface_pixels[i] = NULL;
        surface->sprite_width[i] = 0;
        surface->sprite_height[i] = 0;
        // free(surface->sprite_colours_used[i]);
        // free(surface->sprite_colour_list[i]);
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
    colours[0] = 0xff00ff;

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

        int unknown = index_data[index_offset++] & 0xff;

        int area = surface->sprite_width[i] * surface->sprite_height[i];

        surface->sprite_colours_used[i] = calloc(area, sizeof(int8_t));
        surface->sprite_colour_list[i] = colours;
        surface->sprite_width_full[i] = full_width;
        surface->sprite_height_full[i] = full_height;
        free(surface->surface_pixels[i]);
        surface->surface_pixels[i] = NULL;
        surface->sprite_translate[i] = 0;

        if (surface->sprite_translate_x[i] != 0 ||
            surface->sprite_translate_y[i] != 0) {
            surface->sprite_translate[i] = 1;
        }

        if (unknown == 0) {
            for (int pixel = 0; pixel < area; pixel++) {
                surface->sprite_colours_used[i][pixel] =
                    sprite_data[sprite_offset++];

                if (surface->sprite_colours_used[i][pixel] == 0) {
                    surface->sprite_translate[i] = 1;
                }
            }
        } else if (unknown == 1) {
            for (int x = 0; x < surface->sprite_width[i]; x++) {
                for (int y = 0; y < surface->sprite_height[i]; y++) {
                    surface
                        ->sprite_colours_used[i][x +
                                                 y * surface->sprite_width[i]] =
                        sprite_data[sprite_offset++];

                    if (surface->sprite_colours_used
                            [i][x + y * surface->sprite_width[i]] == 0) {
                        surface->sprite_translate[i] = 1;
                    }
                }
            }
        }
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
}

void surface_draw_world(Surface *surface, int sprite_id) {
    int sprite_size =
        surface->sprite_width[sprite_id] * surface->sprite_height[sprite_id];

    int32_t *sprite_pixels = surface->surface_pixels[sprite_id];

    for (int i = 0; i < sprite_size; i++) {
        int colour = sprite_pixels[i];

        surface_texture_pixels[((colour & 0xf80000) >> 9) +
                               ((colour & 0xf800) >> 6) +
                               ((colour & 0xf8) >> 3)]++;
    }

    int32_t *colour_list = calloc(256, sizeof(int32_t));
    colour_list[0] = 0xff00ff;

    int32_t ai3[256];
    memset(ai3, 0, 256 * sizeof(int32_t));

    for (int i = 0; i < 32768; i++) {
        int32_t pixel = surface_texture_pixels[i];

        if (pixel > ai3[255]) {
            for (int k1 = 1; k1 < 256; k1++) {
                if (pixel <= ai3[k1]) {
                    continue;
                }

                for (int i2 = 255; i2 > k1; i2--) {
                    colour_list[i2] = colour_list[i2 - 1];
                    ai3[i2] = ai3[i2 - 1];
                }

                colour_list[k1] = ((i & 0x7c00) << 9) + ((i & 0x3e0) << 6) +
                                  ((i & 0x1f) << 3) + 0x40404;

                ai3[k1] = pixel;
                break;
            }
        }

        surface_texture_pixels[i] = -1;
    }

    int8_t *colours_used = calloc(sprite_size, sizeof(int8_t));

    for (int i = 0; i < sprite_size; i++) {
        int j2 = sprite_pixels[i];

        int pixel_index =
            ((j2 & 0xf80000) >> 9) + ((j2 & 0xf800) >> 6) + ((j2 & 0xf8) >> 3);

        int pixel = surface_texture_pixels[pixel_index];

        if (pixel == -1) {
            int i3 = 999999999;
            int j3 = (j2 >> 16) & 0xff;
            int k3 = (j2 >> 8) & 0xff;
            int l3 = j2 & 0xff;

            for (int i4 = 0; i4 < 256; i4++) {
                int j4 = colour_list[i4];
                int k4 = (j4 >> 16) & 0xff;
                int l4 = (j4 >> 8) & 0xff;
                int i5 = j4 & 0xff;

                int j5 = (j3 - k4) * (j3 - k4) + (k3 - l4) * (k3 - l4) +
                         (l3 - i5) * (l3 - i5);

                if (j5 < i3) {
                    i3 = j5;
                    pixel = i4;
                }
            }

            surface_texture_pixels[pixel_index] = pixel;
        }

        colours_used[i] = pixel & 0xff;
    }

    surface->sprite_colours_used[sprite_id] = colours_used;
    surface->sprite_colour_list[sprite_id] = colour_list;

    free(surface->surface_pixels[sprite_id]);
    surface->surface_pixels[sprite_id] = NULL;
}

void surface_load_sprite(Surface *surface, int sprite_id) {
    int8_t *idx = surface->sprite_colours_used[sprite_id];

    if (idx == NULL) {
        return;
    }

    int area =
        surface->sprite_width[sprite_id] * surface->sprite_height[sprite_id];

    int32_t *cols = surface->sprite_colour_list[sprite_id];
    int32_t *pixels = malloc(area * sizeof(int32_t));

    for (int i = 0; i < area; i++) {
        int32_t colour = cols[idx[i] & 0xff];

        if (colour == 0) {
            colour = 1;
        } else if (colour == 0xff00ff) {
            colour = 0;
        }

        pixels[i] = colour;
    }

    surface->surface_pixels[sprite_id] = pixels;

    free(surface->sprite_colours_used[sprite_id]);
    surface->sprite_colours_used[sprite_id] = NULL;
    // surface->sprite_colour_list[sprite_id] = NULL;
    // free(surface->sprite_colour_list[sprite_id]);
}

/* TODO not actually draw_sprite - also allocates ? */
/* allocates a sprite buffer to draw the landscape onto with draw_world */
void surface_draw_sprite_from5(Surface *surface, int sprite_id, int x, int y,
                               int width, int height) {
    surface->sprite_width[sprite_id] = width;
    surface->sprite_height[sprite_id] = height;
    surface->sprite_translate[sprite_id] = 0;
    surface->sprite_translate_x[sprite_id] = 0;
    surface->sprite_translate_y[sprite_id] = 0;
    surface->sprite_width_full[sprite_id] = width;
    surface->sprite_height_full[sprite_id] = height;

    free(surface->surface_pixels[sprite_id]);

    surface->surface_pixels[sprite_id] =
        malloc(width * height * sizeof(int32_t));

    int pixel = 0;

    for (int yy = y; yy < y + height; yy++) {
        for (int xx = x; xx < x + width; xx++) {
            surface->surface_pixels[sprite_id][pixel++] =
                surface->pixels[xx + yy * surface->width2];
        }
    }

    free(surface->sprite_colours_used[sprite_id]);
    surface->sprite_colours_used[sprite_id] = NULL;

    free(surface->sprite_colour_list[sprite_id]);
    surface->sprite_colour_list[sprite_id] = NULL;
}

void surface_draw_sprite_reversed(Surface *surface, int sprite_id, int x, int y,
                                  int width, int height) {
    surface->sprite_width[sprite_id] = width;
    surface->sprite_height[sprite_id] = height;
    surface->sprite_translate[sprite_id] = 0;
    surface->sprite_translate_x[sprite_id] = 0;
    surface->sprite_translate_y[sprite_id] = 0;
    surface->sprite_width_full[sprite_id] = width;
    surface->sprite_height_full[sprite_id] = height;

    free(surface->surface_pixels[sprite_id]);

    surface->surface_pixels[sprite_id] =
        malloc(width * height * sizeof(int32_t));

    int pixel = 0;

    for (int xx = x; xx < x + width; xx++) {
        for (int yy = y; yy < y + height; yy++) {
            surface->surface_pixels[sprite_id][pixel++] =
                surface->pixels[xx + yy * surface->width2];
        }
    }

    free(surface->sprite_colours_used[sprite_id]);
    surface->sprite_colours_used[sprite_id] = NULL;

    free(surface->sprite_colour_list[sprite_id]);
    surface->sprite_colour_list[sprite_id] = NULL;
}

void surface_draw_sprite_from3(Surface *surface, int x, int y, int sprite_id) {
    if (surface->sprite_translate[sprite_id] != 0) {
        x += surface->sprite_translate_x[sprite_id];
        y += surface->sprite_translate_y[sprite_id];
    }

    int r_y = x + y * surface->width2;
    int r_x = 0;
    int height = surface->sprite_height[sprite_id];
    int width = surface->sprite_width[sprite_id];
    int w2 = surface->width2 - width;
    int h2 = 0;

    if (y < surface->bounds_top_y) {
        int j2 = surface->bounds_top_y - y;
        height -= j2;
        y = surface->bounds_top_y;
        r_x += j2 * width;
        r_y += j2 * surface->width2;
    }

    if (y + height >= surface->bounds_bottom_y) {
        height -= y + height - surface->bounds_bottom_y + 1;
    }

    if (x < surface->bounds_top_x) {
        int k2 = surface->bounds_top_x - x;
        width -= k2;
        x = surface->bounds_top_x;
        r_x += k2;
        r_y += k2;
        h2 += k2;
        w2 += k2;
    }

    if (x + width >= surface->bounds_bottom_x) {
        int l2 = x + width - surface->bounds_bottom_x + 1;
        width -= l2;
        h2 += l2;
        w2 += l2;
    }

    if (width <= 0 || height <= 0) {
        return;
    }

    int8_t inc = 1;

    if (surface->interlace) {
        inc = 2;
        w2 += surface->width2;
        h2 += surface->sprite_width[sprite_id];

        if ((y & 1) != 0) {
            r_y += surface->width2;
            height--;
        }
    }

    if (surface->surface_pixels[sprite_id] == NULL) {
        surface_draw_sprite_from10a(surface->pixels,
                                    surface->sprite_colours_used[sprite_id],
                                    surface->sprite_colour_list[sprite_id], r_x,
                                    r_y, width, height, w2, h2, inc);
    } else {
        surface_draw_sprite_from10(surface->pixels,
                                   surface->surface_pixels[sprite_id], r_x, r_y,
                                   width, height, w2, h2, inc);
    }
}

void surface_sprite_clipping_from5(Surface *surface, int x, int y, int width,
                                   int height, int sprite_id) {
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

    int i3 = x + y * surface->width2;
    int k3 = surface->width2 - width;

    if (y < surface->bounds_top_y) {
        int l3 = surface->bounds_top_y - y;
        height -= l3;
        y = 0;
        i3 += l3 * surface->width2;
        i2 += k2 * l3;
    }

    if (y + height >= surface->bounds_bottom_y) {
        height -= y + height - surface->bounds_bottom_y + 1;
    }

    if (x < surface->bounds_top_x) {
        int i4 = surface->bounds_top_x - x;
        width -= i4;
        x = 0;
        i3 += i4;
        l1 += j2 * i4;
        k3 += i4;
    }

    if (x + width >= surface->bounds_bottom_x) {
        int j4 = x + width - surface->bounds_bottom_x + 1;
        width -= j4;
        k3 += j4;
    }

    int8_t y_inc = 1;

    if (surface->interlace) {
        y_inc = 2;
        k3 += surface->width2;
        k2 += k2;

        if ((y & 1) != 0) {
            i3 += surface->width2;
            height--;
        }
    }

    surface_plot_scale_from13(surface->pixels,
                              surface->surface_pixels[sprite_id], 0, l1, i2, i3,
                              k3, width, height, j2, k2, sprite_width, y_inc);
}

void surface_sprite_clipping_from7(Surface *surface, int x, int y, int width,
                                   int height, int sprite_id, int tx, int ty) {
    if (sprite_id >= 50000) {
        mudclient_draw_teleport_bubble(surface->mud, x, y, width, height,
                                       sprite_id - 50000);

        return;
    }

    if (sprite_id >= 40000) {
        mudclient_draw_item(surface->mud, x, y, width, height,
                            sprite_id - 40000);
        return;
    }

    if (sprite_id >= 20000) {
        mudclient_draw_npc(surface->mud, x, y, width, height, sprite_id - 20000,
                           tx, ty);
        return;
    }

    if (sprite_id >= 5000) {
        mudclient_draw_player(surface->mud, x, y, width, height,
                              sprite_id - 5000, tx, ty);
        return;
    }

    surface_sprite_clipping_from5(surface, x, y, width, height, sprite_id);
}

void surface_draw_sprite_alpha_from4(Surface *surface, int x, int y,
                                     int sprite_id, int alpha) {
    return;

    if (surface->sprite_translate[sprite_id]) {
        x += surface->sprite_translate_x[sprite_id];
        y += surface->sprite_translate_y[sprite_id];
    }

    int size = x + y * surface->width2;
    int src_pos = 0;
    int height = surface->sprite_height[sprite_id];
    int width = surface->sprite_width[sprite_id];
    int extra_x_space = surface->width2 - width;
    int j2 = 0;

    if (y < surface->bounds_top_y) {
        int k2 = surface->bounds_top_y - y;
        height -= k2;
        y = surface->bounds_top_y;
        src_pos += k2 * width;
        size += k2 * surface->width2;
    }

    if (y + height >= surface->bounds_bottom_y) {
        height -= y + height - surface->bounds_bottom_y + 1;
    }

    if (x < surface->bounds_top_x) {
        int l2 = surface->bounds_top_x - x;
        width -= l2;
        x = surface->bounds_top_x;
        src_pos += l2;
        size += l2;
        j2 += l2;
        extra_x_space += l2;
    }

    if (x + width >= surface->bounds_bottom_x) {
        int i3 = x + width - surface->bounds_bottom_x + 1;
        width -= i3;
        j2 += i3;
        extra_x_space += i3;
    }

    if (width <= 0 || height <= 0) {
        return;
    }

    int8_t y_inc = 1;

    if (surface->interlace) {
        y_inc = 2;
        extra_x_space += surface->width2;
        j2 += surface->sprite_width[sprite_id];

        if ((y & 1) != 0) {
            size += surface->width2;
            height--;
        }
    }

    if (surface->surface_pixels[sprite_id] == NULL) {
        surface_draw_sprite_alpha_from11a(
            surface->pixels, surface->sprite_colours_used[sprite_id],
            surface->sprite_colour_list[sprite_id], src_pos, size, width,
            height, extra_x_space, j2, y_inc, alpha);
    } else {
        surface_draw_sprite_alpha_from11(
            surface->pixels, surface->surface_pixels[sprite_id], 0, src_pos,
            size, width, height, extra_x_space, j2, y_inc, alpha);
    }
}

void surface_draw_action_bubble(Surface *surface, int x, int y, int scale_x,
                                int scale_y, int sprite_id, int alpha) {
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

    int j3 = x + y * surface->width2;
    int l3 = surface->width2 - scale_x;

    if (y < surface->bounds_top_y) {
        int i4 = surface->bounds_top_y - y;
        scale_y -= i4;
        y = 0;
        j3 += i4 * surface->width2;
        j2 += l2 * i4;
    }

    if (y + scale_y >= surface->bounds_bottom_y)
        scale_y -= y + scale_y - surface->bounds_bottom_y + 1;

    if (x < surface->bounds_top_x) {
        int j4 = surface->bounds_top_x - x;
        scale_x -= j4;
        x = 0;
        j3 += j4;
        i2 += k2 * j4;
        l3 += j4;
    }

    if (x + scale_x >= surface->bounds_bottom_x) {
        int k4 = x + scale_x - surface->bounds_bottom_x + 1;
        scale_x -= k4;
        l3 += k4;
    }

    int8_t y_inc = 1;

    if (surface->interlace) {
        y_inc = 2;
        l3 += surface->width2;
        l2 += l2;

        if ((y & 1) != 0) {
            j3 += surface->width2;
            scale_y--;
        }
    }

    surface_transparent_scale(
        surface->pixels, surface->surface_pixels[sprite_id], 0, i2, j2, j3, l3,
        scale_x, scale_y, k2, l2, sprite_width, y_inc, alpha);
}

void surface_sprite_clipping_from6(Surface *surface, int x, int y, int width,
                                   int height, int sprite_id, int colour) {
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

    int j3 = x + y * surface->width2;
    int l3 = surface->width2 - width;

    if (y < surface->bounds_top_y) {
        int i4 = surface->bounds_top_y - y;
        height -= i4;
        y = 0;
        j3 += i4 * surface->width2;
        j2 += l2 * i4;
    }

    if (y + height >= surface->bounds_bottom_y) {
        height -= y + height - surface->bounds_bottom_y + 1;
    }

    if (x < surface->bounds_top_x) {
        int j4 = surface->bounds_top_x - x;
        width -= j4;
        x = 0;
        j3 += j4;
        i2 += k2 * j4;
        l3 += j4;
    }

    if (x + width >= surface->bounds_bottom_x) {
        int k4 = x + width - surface->bounds_bottom_x + 1;
        width -= k4;
        l3 += k4;
    }

    int8_t y_inc = 1;

    if (surface->interlace) {
        y_inc = 2;
        l3 += surface->width2;
        l2 += l2;

        if ((y & 1) != 0) {
            j3 += surface->width2;
            height--;
        }
    }

    surface_plot_scale_from14(surface->pixels,
                              surface->surface_pixels[sprite_id], 0, i2, j2, j3,
                              l3, width, height, k2, l2, k1, y_inc, colour);
}

void surface_draw_sprite_from10(int32_t *dest, int32_t *src, int src_pos,
                                int dest_pos, int width, int height, int j1,
                                int k1, int y_inc) {
    int colour = 0;
    int i2 = -(width >> 2);
    width = -(width & 3);

    for (int y = -height; y < 0; y += y_inc) {
        for (int x = i2; x < 0; x++) {
            colour = src[src_pos++];

            if (colour != 0) {
                dest[dest_pos++] = colour;
            } else {
                dest_pos++;
            }

            colour = src[src_pos++];

            if (colour != 0) {
                dest[dest_pos++] = colour;
            } else {
                dest_pos++;
            }

            colour = src[src_pos++];

            if (colour != 0) {
                dest[dest_pos++] = colour;
            } else {
                dest_pos++;
            }

            colour = src[src_pos++];

            if (colour != 0) {
                dest[dest_pos++] = colour;
            } else {
                dest_pos++;
            }
        }

        for (int l2 = width; l2 < 0; l2++) {
            colour = src[src_pos++];

            if (colour != 0) {
                dest[dest_pos++] = colour;
            } else {
                dest_pos++;
            }
        }

        dest_pos += j1;
        src_pos += k1;
    }
}

void surface_draw_sprite_from10a(int32_t *dest, int8_t *colour_idx,
                                 int32_t *colours, int src_pos, int dest_pos,
                                 int width, int height, int w2, int h2,
                                 int y_inc) {
    int l1 = -(width / 4);
    width = -(width & 3);

    for (int y = -height; y < 0; y += y_inc) {
        for (int x = l1; x < 0; x++) {
            int8_t byte0 = colour_idx[src_pos++];

            if (byte0 != 0) {
                dest[dest_pos++] = colours[byte0 & 0xff];
            } else {
                dest_pos++;
            }

            byte0 = colour_idx[src_pos++];

            if (byte0 != 0) {
                dest[dest_pos++] = colours[byte0 & 0xff];
            } else {
                dest_pos++;
            }

            byte0 = colour_idx[src_pos++];

            if (byte0 != 0) {
                dest[dest_pos++] = colours[byte0 & 0xff];
            } else {
                dest_pos++;
            }

            byte0 = colour_idx[src_pos++];

            if (byte0 != 0) {
                dest[dest_pos++] = colours[byte0 & 0xff];
            } else {
                dest_pos++;
            }
        }

        for (int x = width; x < 0; x++) {
            int8_t byte1 = colour_idx[src_pos++];

            if (byte1 != 0) {
                dest[dest_pos++] = colours[byte1 & 0xff];
            } else {
                dest_pos++;
            }
        }

        dest_pos += w2;
        src_pos += h2;
    }
}

void surface_plot_scale_from13(int32_t *dest, int32_t *src, int i, int j, int k,
                               int dest_pos, int i1, int j1, int k1, int l1,
                               int i2, int j2, int k2) {
    int l2 = j;

    for (int i3 = -k1; i3 < 0; i3 += k2) {
        int j3 = (k >> 16) * j2;

        for (int k3 = -j1; k3 < 0; k3++) {
            i = src[(j >> 16) + j3];

            if (i != 0) {
                dest[dest_pos++] = i;
            } else {
                dest_pos++;
            }

            j += l1;
        }

        k += i2;
        j = l2;
        dest_pos += i1;
    }
}

void surface_draw_sprite_alpha_from11(int32_t *dest, int32_t *src, int i,
                                      int src_pos, int size, int width,
                                      int height, int extra_x_space, int k1,
                                      int y_inc, int alpha) {
    int j2 = 256 - alpha;

    for (int k2 = -height; k2 < 0; k2 += y_inc) {
        for (int l2 = -width; l2 < 0; l2++) {
            i = src[src_pos++];

            if (i != 0) {
                int i3 = dest[size];

                dest[size++] =
                    ((((i & 0xff00ff) * alpha + (i3 & 0xff00ff) * j2) &
                      -0xff0100) +
                     (((i & 0xff00) * alpha + (i3 & 0xff00) * j2) &
                      0xff0000)) >>
                    8;
            } else {
                size++;
            }
        }

        size += extra_x_space;
        src_pos += k1; /* k1 = x_inc? */
    }
}

void surface_draw_sprite_alpha_from11a(int32_t *dest, int8_t *colour_idx,
                                       int32_t *colours, int list_pos, int size,
                                       int width, int height, int extra_x_space,
                                       int j1, int y_inc, int alpha) {
    int i2 = 256 - alpha;

    for (int j2 = -height; j2 < 0; j2 += y_inc) {
        for (int k2 = -width; k2 < 0; k2++) {
            int l2 = colour_idx[list_pos++];

            if (l2 != 0) {
                l2 = colours[l2 & 0xff];

                int i3 = dest[size];

                dest[size++] =
                    ((((l2 & 0xff00ff) * alpha + (i3 & 0xff00ff) * i2) &
                      -0xff0100) +
                     (((l2 & 0xff00) * alpha + (i3 & 0xff00) * i2) &
                      0xff0000)) >>
                    8;
            } else {
                size++;
            }
        }

        size += extra_x_space;
        list_pos += j1;
    }
}

void surface_transparent_scale(int32_t *dest, int32_t *src, int i, int j, int k,
                               int dest_pos, int i1, int j1, int k1, int l1,
                               int i2, int j2, int y_inc, int alpha) {
    int i3 = 256 - alpha;
    int j3 = j;

    for (int k3 = -k1; k3 < 0; k3 += y_inc) {
        int l3 = (k >> 16) * j2;

        for (int i4 = -j1; i4 < 0; i4++) {
            i = src[(j >> 16) + l3];

            if (i != 0) {
                int j4 = dest[dest_pos];

                dest[dest_pos++] =
                    ((((i & 0xff00ff) * alpha + (j4 & 0xff00ff) * i3) &
                      -0xff0100) +
                     (((i & 0xff00) * alpha + (j4 & 0xff00) * i3) &
                      0xff0000)) >>
                    8;
            } else {
                dest_pos++;
            }

            j += l1;
        }

        k += i2;
        j = j3;
        dest_pos += i1;
    }
}

void surface_plot_scale_from14(int32_t *dest, int32_t *src, int i, int j, int k,
                               int l, int i1, int width, int height, int l1,
                               int i2, int j2, int y_inc, int colour) {
    int r = (colour >> 16) & 0xff;
    int g = (colour >> 8) & 0xff;
    int b = colour & 0xff;
    int a = j;

    for (int i4 = -height; i4 < 0; i4 += y_inc) {
        int j4 = (k >> 16) * j2;

        for (int k4 = -width; k4 < 0; k4++) {
            i = src[(j >> 16) + j4];

            if (i != 0) {
                int l4 = (i >> 16) & 0xff;
                int i5 = (i >> 8) & 0xff;
                int j5 = i & 0xff;

                if (l4 == i5 && i5 == j5) {
                    dest[l++] = (((l4 * r) >> 8) << 16) +
                                (((i5 * g) >> 8) << 8) + ((j5 * b) >> 8);
                } else {
                    dest[l++] = i;
                }
            } else {
                l++;
            }

            j += l1;
        }

        k += i2;
        j = a;
        l += i1;
    }
}

void surface_draw_minimap_sprite(Surface *surface, int x, int y, int sprite_id,
                                 int rotation, int scale) {
    int j1 = surface->width2;
    int k1 = surface->height2;
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

    rotation &= 0xff;

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

    if (k6 < surface->bounds_top_y) {
        k6 = surface->bounds_top_y;
    }

    if (l6 > surface->bounds_bottom_y) {
        l6 = surface->bounds_bottom_y;
    }

    if (surface->an_int_array_340 == NULL ||
        surface->an_int_array_340_length != k1 + 1) {
        free(surface->an_int_array_340);
        free(surface->an_int_array_341);
        free(surface->an_int_array_342);
        free(surface->an_int_array_343);
        free(surface->an_int_array_344);
        free(surface->an_int_array_345);

        surface->an_int_array_340 = calloc((k1 + 1), sizeof(int));
        surface->an_int_array_341 = calloc((k1 + 1), sizeof(int));
        surface->an_int_array_342 = calloc((k1 + 1), sizeof(int));
        surface->an_int_array_343 = calloc((k1 + 1), sizeof(int));
        surface->an_int_array_344 = calloc((k1 + 1), sizeof(int));
        surface->an_int_array_345 = calloc((k1 + 1), sizeof(int));

        surface->an_int_array_340_length = k1 + 1;
    }

    for (int i7 = k6; i7 <= l6; i7++) {
        surface->an_int_array_340[i7] = 99999999;
        surface->an_int_array_341[i7] = -99999999;
    }

    int i8 = 0;
    int k8 = 0;
    int i9 = 0;
    int j9 = surface->sprite_width[sprite_id];
    int k9 = surface->sprite_height[sprite_id];

    i2 = 0;
    j2 = 0;
    i3 = j9 - 1;
    j3 = 0;
    k2 = j9 - 1;
    l2 = k9 - 1;
    k3 = 0;
    l3 = k9 - 1;

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
        surface->an_int_array_340[i] = l7;
        surface->an_int_array_341[i] = l7;

        l7 += i8;

        surface->an_int_array_342[i] = 0;
        surface->an_int_array_343[i] = 0;
        surface->an_int_array_344[i] = l8;
        surface->an_int_array_345[i] = l8;

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
        if (l7 < surface->an_int_array_340[i]) {
            surface->an_int_array_340[i] = l7;
            surface->an_int_array_342[i] = j8;
            surface->an_int_array_344[i] = 0;
        }

        if (l7 > surface->an_int_array_341[i]) {
            surface->an_int_array_341[i] = l7;
            surface->an_int_array_343[i] = j8;
            surface->an_int_array_345[i] = 0;
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
        if (l7 < surface->an_int_array_340[i]) {
            surface->an_int_array_340[i] = l7;
            surface->an_int_array_342[i] = j8;
            surface->an_int_array_344[i] = l8;
        }

        if (l7 > surface->an_int_array_341[i]) {
            surface->an_int_array_341[i] = l7;
            surface->an_int_array_343[i] = j8;
            surface->an_int_array_345[i] = l8;
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
        if (l7 < surface->an_int_array_340[i]) {
            surface->an_int_array_340[i] = l7;
            surface->an_int_array_342[i] = j8;
            surface->an_int_array_344[i] = l8;
        }

        if (l7 > surface->an_int_array_341[i]) {
            surface->an_int_array_341[i] = l7;
            surface->an_int_array_343[i] = j8;
            surface->an_int_array_345[i] = l8;
        }

        l7 += i8;
        j8 += k8;
    }

    int l10 = k6 * j1;
    int32_t *ai = surface->surface_pixels[sprite_id];

    for (int i = k6; i < l6; i++) {
        int j11 = surface->an_int_array_340[i] >> 8;
        int k11 = surface->an_int_array_341[i] >> 8;

        if (k11 - j11 <= 0) {
            l10 += j1;
        } else {
            int l11 = surface->an_int_array_342[i] << 9;
            int i12 = ((surface->an_int_array_343[i] << 9) - l11) / (k11 - j11);
            int j12 = surface->an_int_array_344[i] << 9;
            int k12 = ((surface->an_int_array_345[i] << 9) - j12) / (k11 - j11);

            if (j11 < surface->bounds_top_x) {
                l11 += (surface->bounds_top_x - j11) * i12;
                j12 += (surface->bounds_top_x - j11) * k12;
                j11 = surface->bounds_top_x;
            }

            if (k11 > surface->bounds_bottom_x) {
                k11 = surface->bounds_bottom_x;
            }

            if (!surface->interlace || (i & 1) == 0) {
                if (!surface->sprite_translate[sprite_id]) {
                    surface_draw_minimap(surface->pixels, ai, 0, l10 + j11, l11,
                                         j12, i12, k12, j11 - k11, j9);
                } else {
                    // minimap
                    surface_draw_minimap_translate(surface->pixels, ai, 0,
                                                   l10 + j11, l11, j12, i12,
                                                   k12, j11 - k11, j9);
                }
            }

            l10 += j1;
        }
    }
}

void surface_draw_minimap(int32_t *dest, int32_t *src, int i, int j, int k,
                          int l, int i1, int j1, int k1, int l1) {
    // k = x
    for (i = k1; i < 0; i++) {
        dest[j++] = src[(k >> 17) + (l >> 17) * l1];
        k += i1;
        l += j1;
    }
}

void surface_draw_minimap_translate(int32_t *dest, int32_t *src, int i, int j,
                                    int k, int l, int i1, int j1, int k1,
                                    int l1) {
    for (int i2 = k1; i2 < 0; i2++) {
        i = src[(k >> 17) + (l >> 17) * l1];

        if (i != 0) {
            dest[j++] = i;
        } else {
            j++;
        }

        k += i1;
        l += j1;
    }
}

/* this might be draw_sprite_scaled */
void surface_sprite_clipping_from9(Surface *surface, int x, int y, int w, int h,
                                   int sprite_id, int colour1, int colour2,
                                   int tx, int flag) {
    if (colour1 == 0) {
        colour1 = WHITE;
    }

    if (colour2 == 0) {
        colour2 = WHITE;
    }

    int width = surface->sprite_width[sprite_id];
    int height = surface->sprite_height[sprite_id];
    int k2 = 0;
    int l2 = 0;
    int i3 = tx << 16;
    int j3 = (width << 16) / w;
    int k3 = (height << 16) / h;
    int l3 = -((tx << 16) / h);

    if (surface->sprite_translate[sprite_id]) {
        int full_width = surface->sprite_width_full[sprite_id];
        int full_height = surface->sprite_height_full[sprite_id];

        j3 = (full_width << 16) / w;
        k3 = (full_height << 16) / h;

        int j5 = surface->sprite_translate_x[sprite_id];
        int k5 = surface->sprite_translate_y[sprite_id];

        if (flag) {
            j5 = full_width - surface->sprite_width[sprite_id] - j5;
        }

        x += (j5 * w + full_width - 1) / full_width;

        int l5 = (k5 * h + full_height - 1) / full_height;

        y += l5;
        i3 += l5 * l3;

        if ((j5 * w) % full_width != 0) {
            k2 = ((full_width - ((j5 * w) % full_width)) << 16) / w;
        }

        if ((k5 * h) % full_height != 0) {
            l2 = ((full_height - ((k5 * h) % full_height)) << 16) / h;
        }

        w = ((surface->sprite_width[sprite_id] << 16) - k2 + j3 - 1) / j3;
        h = ((surface->sprite_height[sprite_id] << 16) - l2 + k3 - 1) / k3;
    }

    int j4 = y * surface->width2;
    i3 += x << 16;

    if (y < surface->bounds_top_y) {
        int l4 = surface->bounds_top_y - y;
        h -= l4;
        y = surface->bounds_top_y;
        j4 += l4 * surface->width2;
        l2 += k3 * l4;
        i3 += l3 * l4;
    }

    if (y + h >= surface->bounds_bottom_y) {
        h -= y + h - surface->bounds_bottom_y + 1;
    }

    int i5 = (j4 / surface->width2) & 1;

    if (!surface->interlace) {
        i5 = 2;
    }

    if (colour2 == WHITE) {
        if (surface->surface_pixels[sprite_id] != NULL) {
            if (!flag) {
                surface_transparent_sprite_plot_from15(
                    surface, surface->pixels,
                    surface->surface_pixels[sprite_id], 0, k2, l2, j4, w, h, j3,
                    k3, width, colour1, i3, l3, i5);

                return;
            } else {
                surface_transparent_sprite_plot_from15(
                    surface, surface->pixels,
                    surface->surface_pixels[sprite_id], 0,
                    (surface->sprite_width[sprite_id] << 16) - k2 - 1, l2, j4,
                    w, h, -j3, k3, width, colour1, i3, l3, i5);

                return;
            }
        }

        if (!flag) {
            surface_transparent_sprite_plot_from16a(
                surface, surface->pixels,
                surface->sprite_colours_used[sprite_id],
                surface->sprite_colour_list[sprite_id], 0, k2, l2, j4, w, h, j3,
                k3, width, colour1, i3, l3, i5);

            return;
        } else {
            surface_transparent_sprite_plot_from16a(
                surface, surface->pixels,
                surface->sprite_colours_used[sprite_id],
                surface->sprite_colour_list[sprite_id], 0,
                (surface->sprite_width[sprite_id] << 16) - k2 - 1, l2, j4, w, h,
                -j3, k3, width, colour1, i3, l3, i5);

            return;
        }
    }

    if (surface->surface_pixels[sprite_id] != NULL) {
        if (!flag) {
            surface_transparent_sprite_plot_from16(
                surface, surface->pixels, surface->surface_pixels[sprite_id], 0,
                k2, l2, j4, w, h, j3, k3, width, colour1, colour2, i3, l3, i5);

            return;
        } else {
            surface_transparent_sprite_plot_from16(
                surface, surface->pixels, surface->surface_pixels[sprite_id], 0,
                (surface->sprite_width[sprite_id] << 16) - k2 - 1, l2, j4, w, h,
                -j3, k3, width, colour1, colour2, i3, l3, i5);

            return;
        }
    }

    /* flag might be flip */

    if (!flag) {
        surface_transparent_sprite_plot_from17(
            surface, surface->pixels, surface->sprite_colours_used[sprite_id],
            surface->sprite_colour_list[sprite_id], 0, k2, l2, j4, w, h, j3, k3,
            width, colour1, colour2, i3, l3, i5);
    } else {
        surface_transparent_sprite_plot_from17(
            surface, surface->pixels, surface->sprite_colours_used[sprite_id],
            surface->sprite_colour_list[sprite_id], 0,
            (surface->sprite_width[sprite_id] << 16) - k2 - 1, l2, j4, w, h,
            -j3, k3, width, colour1, colour2, i3, l3, i5);
    }
}

void surface_transparent_sprite_plot_from15(Surface *surface, int32_t *dest,
                                            int32_t *src, int i, int j, int k,
                                            int dest_pos, int i1, int j1,
                                            int k1, int l1, int i2, int j2,
                                            int k2, int l2, int i3) {
    int i4 = (j2 >> 16) & 0xff;
    int j4 = (j2 >> 8) & 0xff;
    int k4 = j2 & 0xff;
    int l4 = j;

    for (int i5 = -j1; i5 < 0; i5++) {
        int j5 = (k >> 16) * i2;
        int k5 = k2 >> 16;
        int l5 = i1;

        if (k5 < surface->bounds_top_x) {
            int i6 = surface->bounds_top_x - k5;

            l5 -= i6;
            k5 = surface->bounds_top_x;
            j += k1 * i6;
        }

        if (k5 + l5 >= surface->bounds_bottom_x) {
            int j6 = k5 + l5 - surface->bounds_bottom_x;

            l5 -= j6;
        }

        i3 = 1 - i3;

        if (i3 != 0) {
            for (int k6 = k5; k6 < k5 + l5; k6++) {
                i = src[(j >> 16) + j5];

                if (i != 0) {
                    int j3 = (i >> 16) & 0xff;
                    int k3 = (i >> 8) & 0xff;
                    int l3 = i & 0xff;

                    if (j3 == k3 && k3 == l3) {
                        dest[k6 + dest_pos] = (((j3 * i4) >> 8) << 16) +
                                              (((k3 * j4) >> 8) << 8) +
                                              ((l3 * k4) >> 8);
                    } else {
                        dest[k6 + dest_pos] = i;
                    }
                }

                j += k1;
            }
        }

        k += l1;
        j = l4;
        dest_pos += surface->width2;
        k2 += l2;
    }
}

void surface_transparent_sprite_plot_from16(Surface *surface, int32_t *dest,
                                            int32_t *src, int i, int j, int k,
                                            int dest_pos, int i1, int j1,
                                            int k1, int l1, int i2, int j2,
                                            int k2, int l2, int i3, int j3) {
    int j4 = (j2 >> 16) & 0xff;
    int k4 = (j2 >> 8) & 0xff;
    int l4 = j2 & 0xff;
    int i5 = (k2 >> 16) & 0xff;
    int j5 = (k2 >> 8) & 0xff;
    int k5 = k2 & 0xff;
    int l5 = j;

    for (int i6 = -j1; i6 < 0; i6++) {
        int j6 = (k >> 16) * i2;
        int k6 = l2 >> 16;
        int l6 = i1;

        if (k6 < surface->bounds_top_x) {
            int i7 = surface->bounds_top_x - k6;
            l6 -= i7;
            k6 = surface->bounds_top_x;
            j += k1 * i7;
        }

        if (k6 + l6 >= surface->bounds_bottom_x) {
            int j7 = k6 + l6 - surface->bounds_bottom_x;
            l6 -= j7;
        }

        j3 = 1 - j3;

        if (j3 != 0) {
            for (int k7 = k6; k7 < k6 + l6; k7++) {
                i = src[(j >> 16) + j6];

                if (i != 0) {
                    int k3 = (i >> 16) & 0xff;
                    int l3 = (i >> 8) & 0xff;
                    int i4 = i & 0xff;

                    if (k3 == l3 && l3 == i4) {
                        dest[k7 + dest_pos] = (((k3 * j4) >> 8) << 16) +
                                              (((l3 * k4) >> 8) << 8) +
                                              ((i4 * l4) >> 8);
                    } else if (k3 == 255 && l3 == i4) {
                        dest[k7 + dest_pos] = (((k3 * i5) >> 8) << 16) +
                                              (((l3 * j5) >> 8) << 8) +
                                              ((i4 * k5) >> 8);
                    } else {
                        dest[k7 + dest_pos] = i;
                    }
                }

                j += k1;
            }
        }

        k += l1;
        j = l5;
        dest_pos += surface->width2;
        l2 += i3;
    }
}

void surface_transparent_sprite_plot_from16a(Surface *surface, int32_t *dest,
                                             int8_t *colour_idx,
                                             int32_t *colours, int i, int j,
                                             int k, int l, int i1, int j1,
                                             int k1, int l1, int i2, int j2,
                                             int k2, int l2, int i3) {
    int i4 = (j2 >> 16) & 0xff;
    int j4 = (j2 >> 8) & 0xff;
    int k4 = j2 & 0xff;
    int l4 = j;

    for (int i5 = -j1; i5 < 0; i5++) {
        int j5 = (k >> 16) * i2;
        int k5 = k2 >> 16;
        int l5 = i1;

        if (k5 < surface->bounds_top_x) {
            int i6 = surface->bounds_top_x - k5;
            l5 -= i6;
            k5 = surface->bounds_top_x;
            j += k1 * i6;
        }

        if (k5 + l5 >= surface->bounds_bottom_x) {
            int j6 = k5 + l5 - surface->bounds_bottom_x;
            l5 -= j6;
        }

        i3 = 1 - i3;

        if (i3 != 0) {
            for (int k6 = k5; k6 < k5 + l5; k6++) {
                i = colour_idx[(j >> 16) + j5] & 0xff;

                if (i != 0) {
                    i = colours[i];

                    int j3 = (i >> 16) & 0xff;
                    int k3 = (i >> 8) & 0xff;
                    int l3 = i & 0xff;

                    if (j3 == k3 && k3 == l3) {
                        dest[k6 + l] = (((j3 * i4) >> 8) << 16) +
                                       (((k3 * j4) >> 8) << 8) +
                                       ((l3 * k4) >> 8);
                    } else {
                        dest[k6 + l] = i;
                    }
                }

                j += k1;
            }
        }

        k += l1;
        j = l4;
        l += surface->width2;
        k2 += l2;
    }
}

void surface_transparent_sprite_plot_from17(Surface *surface, int32_t *dest,
                                            int8_t *colour_idx,
                                            int32_t *colours, int i, int j,
                                            int k, int l, int i1, int j1,
                                            int k1, int l1, int i2, int j2,
                                            int k2, int l2, int i3, int j3) {
    int j4 = (j2 >> 16) & 0xff;
    int k4 = (j2 >> 8) & 0xff;
    int l4 = j2 & 0xff;

    int i5 = (k2 >> 16) & 0xff;
    int j5 = (k2 >> 8) & 0xff;
    int k5 = k2 & 0xff;

    int l5 = j;

    for (int i6 = -j1; i6 < 0; i6++) {
        int j6 = (k >> 16) * i2;
        int k6 = l2 >> 16;
        int l6 = i1;

        if (k6 < surface->bounds_top_x) {
            int i7 = surface->bounds_top_x - k6;
            l6 -= i7;
            k6 = surface->bounds_top_x;
            j += k1 * i7;
        }

        if (k6 + l6 >= surface->bounds_bottom_x) {
            int j7 = k6 + l6 - surface->bounds_bottom_x;
            l6 -= j7;
        }

        j3 = 1 - j3;

        if (j3 != 0) {
            for (int k7 = k6; k7 < k6 + l6; k7++) {
                i = colour_idx[(j >> 16) + j6] & 0xff;

                if (i != 0) {
                    i = colours[i];
                    int k3 = (i >> 16) & 0xff;
                    int l3 = (i >> 8) & 0xff;
                    int i4 = i & 0xff;

                    if (k3 == l3 && l3 == i4) {
                        dest[k7 + l] = (((k3 * j4) >> 8) << 16) +
                                       (((l3 * k4) >> 8) << 8) +
                                       ((i4 * l4) >> 8);
                    } else if (k3 == 255 && l3 == i4) {
                        dest[k7 + l] = (((k3 * i5) >> 8) << 16) +
                                       (((l3 * j5) >> 8) << 8) +
                                       ((i4 * k5) >> 8);
                    } else {
                        dest[k7 + l] = i;
                    }
                }

                j += k1;
            }
        }

        k += l1;
        j = l5;
        l += surface->width2;
        l2 += i3;
    }
}

void surface_draw_string_right(Surface *surface, char *text, int x, int y,
                               int font, int colour) {
    surface_draw_string(surface, text, x - surface_text_width(text, font), y,
                        font, colour);
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
            width += font_data[character_width[(unsigned)text[i]] + 7];
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

void surface_draw_string(Surface *surface, char *text, int x, int y, int font,
                         int colour) {
    int8_t *font_data = game_fonts[font];
    int text_length = strlen(text);

    for (int i = 0; i < text_length; i++) {
        if (text[i] == '@' && i + 4 < text_length && text[i + 4] == '@') {
            int start = i + 1;
            int end = i + 4;

            char sliced[(end - start) + 1];
            memset(sliced, '\0', (end - start) + 1);

            strncpy(sliced, text + start, end - start);

            {
                int j = 0;

                while (sliced[j]) {
                    sliced[j] = tolower(sliced[j]);
                    j++;
                }
            }

            if (strcmp(sliced, "red") == 0) {
                colour = 0xff0000;
            } else if (strcmp(sliced, "lre") == 0) {
                colour = 0xff9040;
            } else if (strcmp(sliced, "yel") == 0) {
                colour = 0xffff00;
            } else if (strcmp(sliced, "gre") == 0) {
                colour = 0x00ff00;
            } else if (strcmp(sliced, "blu") == 0) {
                colour = 0x0000ff;
            } else if (strcmp(sliced, "cya") == 0) {
                colour = 0x00ffff;
            } else if (strcmp(sliced, "mag") == 0) {
                colour = 0xff00ff;
            } else if (strcmp(sliced, "whi") == 0) {
                colour = 0xffffff;
            } else if (strcmp(sliced, "bla") == 0) {
                colour = 0;
            } else if (strcmp(sliced, "dre") == 0) {
                colour = 0xc00000;
            } else if (strcmp(sliced, "ora") == 0) {
                colour = 0xff9040;
            } else if (strcmp(sliced, "ran") == 0) {
                float r = (float)rand() / (float)RAND_MAX;
                colour = (int)(r * (float)0xffffff);
            } else if (strcmp(sliced, "or1") == 0) {
                colour = 0xffb000;
            } else if (strcmp(sliced, "or2") == 0) {
                colour = 0xff7000;
            } else if (strcmp(sliced, "or3") == 0) {
                colour = 0xff3000;
            } else if (strcmp(sliced, "gr1") == 0) {
                colour = 0xc0ff00;
            } else if (strcmp(sliced, "gr2") == 0) {
                colour = 0x80ff00;
            } else if (strcmp(sliced, "gr3") == 0) {
                colour = 0x40ff00;
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
        } else {
            int width = character_width[(unsigned)text[i]];

            if (surface->logged_in && colour != 0) {
                surface_draw_character(surface, width, x + 1, y, 0, font_data);
                surface_draw_character(surface, width, x, y + 1, 0, font_data);
            }

            surface_draw_character(surface, width, x, y, colour, font_data);

            x += font_data[width + 7];
        }
    }
}

void surface_draw_character(Surface *surface, int font_offset, int x, int y,
                            int colour, int8_t *font_data) {
    /* baseline and kerning offsets */
    int draw_x = x + font_data[font_offset + 5];
    int draw_y = y - font_data[font_offset + 6];

    int width = font_data[font_offset + 3];
    int height = font_data[font_offset + 4];

    /* position of pixel data for the font (on/off) */
    int font_pos = font_data[font_offset] * 16384 +
                   font_data[font_offset + 1] * 128 +
                   font_data[font_offset + 2];

    int dest_pos = draw_x + draw_y * surface->width2;
    int dest_offset = surface->width2 - width;
    int font_data_offset = 0;

    if (draw_y < surface->bounds_top_y) {
        int clip_y = surface->bounds_top_y - draw_y;
        height -= clip_y;
        draw_y = surface->bounds_top_y;
        font_pos += clip_y * width;
        dest_pos += clip_y * surface->width2;
    }

    if (draw_y + height >= surface->bounds_bottom_y) {
        height -= draw_y + height - surface->bounds_bottom_y + 1;
    }

    if (draw_x < surface->bounds_top_x) {
        int clip_x = surface->bounds_top_x - draw_x;
        width -= clip_x;
        draw_x = surface->bounds_top_x;
        font_pos += clip_x;
        dest_pos += clip_x;
        font_data_offset += clip_x;
        dest_offset += clip_x;
    }

    if (draw_x + width >= surface->bounds_bottom_x) {
        int clip_x = draw_x + width - surface->bounds_bottom_x + 1;
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

void surface_plot_letter(int32_t *dest, int8_t *font_data, int colour,
                         int font_pos, int dest_pos, int width, int height,
                         int dest_offset, int font_data_offset) {
    for (int y = 0 - height; y < 0; y++) {
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
    int tab_width = width / tabs_length;
    int offset_x = 0;

    for (int i = 0; i < tabs_length; i++) {
        int tab_colour = selected == i ? LIGHT_GREY : TAB_DARK_GREY;

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
