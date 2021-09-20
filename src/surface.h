#ifndef _H_SURFACE
#define _H_SURFACE

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef WII
#include <gccore.h>

#define CLIP(X) ((X) > 255 ? 255 : (X) < 0 ? 0 : X)
#define RGB2Y(R, G, B) CLIP(((66 * (R) + 129 * (G) + 25 * (B) + 128) >> 8) + 16)
#define RGB2U(R, G, B) CLIP(((-38 * (R)-74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(((112 * (R)-94 * (G)-18 * (B) + 128) >> 8) + 128)
#endif

typedef struct Surface Surface;

#include "colours.h"
#include "mudclient.h"
#include "utility.h"

#define SLEEP_WIDTH 255
#define SLEEP_HEIGHT 40

extern int an_int_346;
extern int an_int_347;
extern int an_int_348;
extern int8_t *game_fonts[50];
extern int character_width[256];
extern int32_t *surface_texture_pixels;

void init_surface_global();

typedef struct Surface {
    int limit;
    int width2;
    int height2;
    int area;
    int width1;
    int height1;
    int32_t *pixels;
    int32_t **surface_pixels;
    int8_t **sprite_colours_used;
    int32_t **sprite_colour_list;
    int *sprite_width;
    int *sprite_height;
    int *sprite_width_full;
    int *sprite_height_full;
    int8_t *sprite_translate;
    int *sprite_translate_x;
    int *sprite_translate_y;
    int interlace;
    int logged_in;
    int *an_int_array_340;
    int *an_int_array_341;
    int *an_int_array_342;
    int *an_int_array_343;
    int *an_int_array_344;
    int *an_int_array_345;
    int an_int_array_340_length;
    int bounds_top_y;
    int bounds_bottom_y;
    int bounds_top_x;
    int bounds_bottom_x;
    mudclient *mud;
} Surface;

int surface_rgb_to_int(int r, int g, int b);
void create_font(int8_t *buffer, int id);

void surface_new(Surface *surface, int width, int height, int limit,
                 mudclient *mud);
void surface_set_bounds(Surface *surface, int x1, int y1, int x2, int y2);
void surface_reset_bounds(Surface *surface);
void surface_reset_draw(Surface *surface);
void surface_draw(Surface *surface);
void surface_black_screen(Surface *surface);
void surface_draw_circle(Surface *surface, int x, int y, int radius, int colour,
                         int alpha);
void surface_draw_box_alpha(Surface *surface, int x, int y, int width,
                            int height, int colour, int alpha);
void surface_draw_gradient(Surface *surface, int x, int y, int width,
                           int height, int colour_top, int colour_bottom);
void surface_draw_box(Surface *surface, int x, int y, int width, int height,
                      int colour);
void surface_draw_line_horiz(Surface *surface, int x, int y, int width,
                             int colour);
void surface_draw_line_vert(Surface *surface, int x, int y, int height,
                            int colour);
void surface_draw_box_edge(Surface *surface, int x, int y, int width,
                           int height, int colour);
void surface_set_pixel(Surface *surface, int x, int y, int colour);
void surface_fade_to_black(Surface *surface);
void surface_draw_line_alpha(Surface *surface, int i, int j, int x, int y,
                             int width, int height);
void surface_clear(Surface *surface);
void surface_parse_sprite(Surface *surface, int sprite_id, int8_t *sprite_data,
                          int8_t *index_data, int frame_count);
void surface_read_sleep_word(Surface *surface, int sprite_id,
                             int8_t *sprite_data);
void surface_draw_world(Surface *surface, int sprite_id);
void surface_load_sprite(Surface *surface, int sprite_id);
void surface_draw_sprite_from5(Surface *surface, int sprite_id, int x, int y,
                               int width, int height);
void surface_draw_sprite_from3(Surface *surface, int x, int y, int sprite_id);
void surface_sprite_clipping_from5(Surface *surface, int x, int y, int width,
                                   int height, int sprite_id);
void surface_sprite_clipping_from7(Surface *surface, int x, int y, int width,
                                   int height, int sprite_id, int tx, int ty);
void surface_draw_sprite_alpha_from4(Surface *surface, int x, int y,
                                     int sprite_id, int alpha);
void surface_draw_action_bubble(Surface *surface, int x, int y, int scale_x,
                                int scale_y, int sprite_id, int alpha);
void surface_sprite_clipping_from6(Surface *surface, int x, int y, int width,
                                   int height, int sprite_id, int colour);
void surface_draw_sprite_from10(int32_t *dest, int32_t *src, int i,
                                int src_pos, int dest_pos, int width,
                                int height, int j1, int k1, int y_inc);
void surface_draw_sprite_from10a(int32_t *dest, int8_t *colour_idx,
                                 int32_t *colours, int src_pos, int dest_pos,
                                 int width, int height, int w2, int h2,
                                 int y_inc);
void surface_plot_scale_from13(int32_t *dest, int32_t *src, int i, int j,
                               int k, int dest_pos, int i1, int j1, int k1,
                               int l1, int i2, int j2, int k2);
void surface_draw_sprite_alpha_from11(int32_t *dest, int32_t *src, int i,
                                      int src_pos, int size, int width,
                                      int height, int extra_x_spcae, int k1,
                                      int y_inc, int alpha);
void surface_draw_sprite_alpha_from11a(int32_t *dest, int8_t *colour_idx,
                                       int32_t *colours, int list_pos,
                                       int size, int width, int height,
                                       int extra_x_space, int j1, int y_inc,
                                       int alpha);
void surface_transparent_scale(int32_t *dest, int32_t *src, int i, int j,
                               int k, int dest_pos, int i1, int j1, int k1,
                               int l1, int i2, int j2, int y_inc, int alpha);
void surface_plot_scale_from14(int32_t *dest, int32_t *src, int i, int j,
                               int k, int l, int i1, int width, int height,
                               int l1, int i2, int j2, int y_inc, int colour);
void surface_draw_minimap_sprite(Surface *surface, int x, int y, int sprite_id,
                                 int rotation, int scale);
void surface_draw_minimap(int32_t *dest, int32_t *src, int i, int j, int k,
                          int l, int i1, int j1, int k1, int l1);
void surface_draw_minimap_translate(int32_t *dest, int32_t *src, int i, int j,
                                    int k, int l, int i1, int j1, int k1,
                                    int l1);
void surface_transparent_sprite_plot_from15(Surface *surface, int32_t *dest,
                                            int32_t *src, int i, int j, int k,
                                            int dest_pos, int i1, int j1,
                                            int k1, int l1, int i2, int j2,
                                            int k2, int l2, int i3);
void surface_transparent_sprite_plot_from16(Surface *surface, int32_t *dest,
                                            int32_t *src, int i, int j, int k,
                                            int dest_pos, int i1, int j1,
                                            int k1, int l1, int i2, int j2,
                                            int k2, int l2, int i3, int j3);
void surface_transparent_sprite_plot_from16a(Surface *surface, int32_t *dest,
                                             int8_t *colour_idx,
                                             int32_t *colours, int i, int j,
                                             int k, int l, int i1, int j1,
                                             int k1, int l1, int i2, int j2,
                                             int k2, int l2, int i3);
void surface_transparent_sprite_plot_from17(Surface *surface, int32_t *dest,
                                            int8_t *colour_idx,
                                            int32_t *colours, int i, int j,
                                            int k, int l, int i1, int j1,
                                            int k1, int l1, int i2, int j2,
                                            int k2, int l2, int i3, int j3);
void surface_draw_string_right(Surface *surface, char *text, int x, int y,
                               int font, int colour);
void surface_draw_string_centre(Surface *surface, char *text, int x, int y,
                                int font, int colour);
void surface_draw_paragraph(Surface *surface, char *text, int x, int y,
                            int font, int colour, int max);
void surface_draw_string(Surface *surface, char *text, int x, int y, int font,
                         int colour);
void surface_draw_character(Surface *surface, int width, int x, int y,
                            int colour, int8_t *font);
void surface_plot_letter(int32_t *dest, int8_t *font, int i, int j, int k,
                         int l, int i1, int j1, int k1);
int surface_text_height(int font_id);
int surface_text_height_font(int font_id);
int surface_text_width(char *text, int font_id);
void surface_draw_tabs(Surface *surface, int x, int y, int width, int height,
                       char **tabs, int tabs_length, int selected);
void surface_free_colours(Surface *surface);

#endif
