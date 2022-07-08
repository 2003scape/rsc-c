#ifndef _H_SURFACE
#define _H_SURFACE

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef RENDER_GL
#include <GL/glew.h>
#include <GL/glu.h>
#include <cglm/cglm.h>

#include "shader.h"

#define FLAT_QUAD_COUNT 2048

#define ITEM_TEXTURE_WIDTH 48
#define ITEM_TEXTURE_HEIGHT 32

#define MEDIA_TEXTURE_WIDTH 512
#define MEDIA_TEXTURE_HEIGHT 256

#define ENTITY_TEXTURE_WIDTH 256
#define ENTITY_TEXTURE_HEIGHT 256

/* TODO combine font and map? */
#define FONT_TEXTURE_WIDTH 286
#define FONT_TEXTURE_HEIGHT 286

#define MAP_TEXTURE_WIDTH 285
#define MAP_TEXTURE_HEIGHT 285

#define CIRCLE_TEXTURE_SIZE 256
#endif

#ifdef WII
#include <gccore.h>

#define CLIP(X) ((X) > 255 ? 255 : (X) < 0 ? 0 : X)
#define RGB2Y(R, G, B) CLIP(((66 * (R) + 129 * (G) + 25 * (B) + 128) >> 8) + 16)
#define RGB2U(R, G, B) CLIP(((-38 * (R)-74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(((112 * (R)-94 * (G)-18 * (B) + 128) >> 8) + 128)
#endif

#ifdef _3DS
#include <3ds.h>
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

#ifdef RENDER_GL
typedef struct SurfaceGlContext {
    GLuint texture_id;
    int quad_count;

    /* boundaries for minimap drawing */
    int min_x;
    int max_x;
    int min_y;
    int max_y;
} SurfaceGlContext;
#endif

typedef struct Surface {
    int limit;
    int width;
    int height;
    int32_t *pixels;
    int32_t **surface_pixels; // TODO rename
    int8_t **sprite_colours_used;
    int32_t **sprite_colour_list;
    int *sprite_width;
    int *sprite_height;
    int *sprite_width_full;
    int *sprite_height_full;
    int8_t *sprite_translate;
    int *sprite_translate_x;
    int *sprite_translate_y;

    int8_t interlace;
    int8_t logged_in;

    /* arrays used for minimap sprite rotation */
    int *rotations_0;
    int *rotations_1;
    int *rotations_2;
    int *rotations_3;
    int *rotations_4;
    int *rotations_5;
    int rotations_length;

    /* used to prevent the minimap from drawing outside of the map UI, and to
     * prevent software rendering from writing outside of the raster array */
    int bounds_min_y;
    int bounds_max_y;
    int bounds_min_x;
    int bounds_max_x;

    mudclient *mud;

#ifdef RENDER_GL
    int8_t gl_fade_to_black;
    int8_t gl_has_faded;

    Shader gl_flat_shader;
    GLuint gl_flat_vao;
    GLuint gl_flat_vbo;
    GLuint gl_flat_ebo;
    int gl_flat_count;

    /* texture arrays */
    GLuint gl_sprite_item_textures;
    GLuint gl_sprite_media_textures;
    GLuint gl_sprite_entity_textures;
    GLuint gl_map_textures;
    GLuint gl_font_textures;
    GLuint gl_framebuffer_textures;

    /* used for texture array and boundary changes */
    SurfaceGlContext gl_contexts[256];

    int gl_context_count;

    int gl_entity_sprite_indices[2000];

    int32_t *gl_screen_pixels_reversed;
    int32_t *gl_screen_pixels;
#endif
} Surface;

void create_font(int8_t *buffer, int id);

void surface_new(Surface *surface, int width, int height, int limit,
                 mudclient *mud);

#ifdef RENDER_GL
void surface_gl_create_texture_array(GLuint *texture_array_id, int width,
                                     int height, int length);
void surface_gl_create_font_texture(int32_t *dest, int font_id,
                                    int draw_shadow);
void surface_gl_create_font_textures(Surface *surface);
void surface_gl_create_circle_texture(Surface *surface);
void surface_gl_reset_context(Surface *surface);
void surface_gl_buffer_flat_quad(Surface *surface, GLfloat *quad,
                                 GLuint texture_array_id);
int surface_gl_sprite_texture_array_id(Surface *surface, int sprite_id);
int surface_gl_sprite_texture_width(Surface *surface, GLuint texture_array_id);
int surface_gl_sprite_texture_height(Surface *surface, GLuint texture_array_id);
int surface_gl_sprite_texture_index(Surface *surface, int sprite_id);
void surface_gl_buffer_textured_quad(Surface *surface, GLuint texture_array_id,
                                     int texture_index, int mask_colour,
                                     int skin_colour, int alpha, int width,
                                     int height, int (*points)[2], float depth_top,
                                     float depth_bottom);
void surface_gl_buffer_sprite(Surface *surface, int sprite_id, int x, int y,
                              int draw_width, int draw_height, int skew_x,
                              int mask_colour, int skin_colour, int alpha,
                              int flip, int rotation, float depth_top, float depth_bottom);
void surface_gl_buffer_character(Surface *surface, char character, int x, int y,
                                 int colour, int font_id, int draw_shadow);
void surface_gl_buffer_box(Surface *surface, int x, int y, int width,
                           int height, int colour, int alpha);
void surface_gl_buffer_circle(Surface *surface, int x, int y, int radius,
                              int colour, int alpha);
void surface_gl_update_framebuffer(Surface *surface);
void surface_gl_update_framebuffer_texture(Surface *surface);
void surface_gl_buffer_framebuffer_quad(Surface *surface);
void surface_gl_draw(Surface *surface, int use_depth);
#endif

void surface_set_bounds(Surface *surface, int min_x, int min_y, int max_x, int max_y);
void surface_reset_bounds(Surface *surface);
void surface_reset_draw(Surface *surface);
void surface_draw(Surface *surface);
void surface_black_screen(Surface *surface);
void surface_draw_circle_software(Surface *surface, int x, int y, int radius,
                                  int colour, int alpha);
void surface_draw_circle(Surface *surface, int x, int y, int radius, int colour,
                         int alpha);
void surface_draw_box_alpha(Surface *surface, int x, int y, int width,
                            int height, int colour, int alpha);
void surface_draw_gradient(Surface *surface, int x, int y, int width,
                           int height, int colour_top, int colour_bottom);
void surface_draw_box_software(Surface *surface, int x, int y, int width,
                               int height, int colour);
void surface_draw_box(Surface *surface, int x, int y, int width, int height,
                      int colour);
void surface_draw_line_horizontal_software(Surface *surface, int x, int y,
                                           int width, int colour);
void surface_draw_line_horizontal(Surface *surface, int x, int y, int width,
                                  int colour);
void surface_draw_line_vertical_software(Surface *surface, int x, int y, int height, int colour);
void surface_draw_line_vertical(Surface *surface, int x, int y, int height,
                                int colour);
void surface_draw_box_edge(Surface *surface, int x, int y, int width,
                           int height, int colour);
void surface_set_pixel(Surface *surface, int x, int y, int colour);
void surface_fade_to_black_software(Surface *surface, int32_t *dest,
                                    int add_alpha);
void surface_fade_to_black(Surface *surface);
void surface_draw_blur_software(Surface *surface, int32_t *dest, int j, int x,
                                int y, int width, int height, int add_alpha);
void surface_draw_blur(Surface *surface, int j, int x, int y, int width,
                       int height);
void surface_apply_login_filter(Surface *surface);
void surface_clear(Surface *surface);
void surface_parse_sprite(Surface *surface, int sprite_id, int8_t *sprite_data,
                          int8_t *index_data, int frame_count);
void surface_read_sleep_word(Surface *surface, int sprite_id,
                             int8_t *sprite_data);
void surface_screen_raster_to_sprite(Surface *surface, int sprite_id);
int32_t *surface_palette_sprite_to_raster(Surface *surface, int sprite_id,
                                          int add_alpha);
void surface_load_sprite(Surface *surface, int sprite_id);
void surface_draw_sprite_from5(Surface *surface, int sprite_id, int x, int y,
                               int width, int height);
void surface_draw_sprite_reversed(Surface *surface, int sprite_id, int x, int y,
                                  int width, int height);
void surface_draw_sprite_from3_software(Surface *surface, int x, int y,
                                        int sprite_id);
void surface_draw_sprite_from3(Surface *surface, int x, int y, int sprite_id);
void surface_sprite_clipping_from5(Surface *surface, int x, int y, int width,
                                   int height, int sprite_id);
void surface_draw_entity_sprite(Surface *surface, int x, int y, int width,
                                int height, int sprite_id, int tx, int ty,
                                float depth_top, float depth_bottom);
void surface_draw_sprite_alpha_from4(Surface *surface, int x, int y,
                                     int sprite_id, int alpha);
void surface_draw_action_bubble(Surface *surface, int x, int y, int scale_x,
                                int scale_y, int sprite_id, int alpha);
void surface_sprite_clipping_from6(Surface *surface, int x, int y, int width,
                                   int height, int sprite_id, int colour);
void surface_plot_sprite(int32_t *dest, int32_t *src, int src_pos, int dest_pos,
                         int width, int height, int dest_offset, int src_offset,
                         int y_inc);
void surface_plot_palette_sprite(int32_t *dest, int8_t *colours,
                                 int32_t *palette, int src_pos, int dest_pos,
                                 int width, int height, int dest_offset,
                                 int src_offset, int y_inc);
void surface_plot_scale_from13(int32_t *dest, int32_t *src, int j, int k,
                               int dest_pos, int i1, int j1, int k1, int l1,
                               int i2, int j2, int k2);
void surface_draw_sprite_alpha_from11(int32_t *dest, int32_t *src, int src_pos,
                                      int size, int width, int height,
                                      int extra_x_spcae, int k1, int y_inc,
                                      int alpha);
void surface_draw_sprite_alpha_from11a(int32_t *dest, int8_t *colour_idx,
                                       int32_t *colours, int list_pos, int size,
                                       int width, int height, int extra_x_space,
                                       int j1, int y_inc, int alpha);
void surface_transparent_scale(int32_t *dest, int32_t *src, int j, int k,
                               int dest_pos, int i1, int j1, int k1, int l1,
                               int i2, int j2, int y_inc, int alpha);
void surface_plot_scale_from14(int32_t *dest, int32_t *src, int j, int k, int l,
                               int i1, int width, int height, int l1, int i2,
                               int j2, int y_inc, int colour);
void surface_draw_minimap_sprite(Surface *surface, int x, int y, int sprite_id,
                                 int rotation, int scale);
void surface_draw_minimap(int32_t *dest, int32_t *src, int j, int k, int l,
                          int i1, int j1, int k1, int l1);
void surface_draw_minimap_translate(int32_t *dest, int32_t *src, int j, int k,
                                    int l, int i1, int j1, int k1, int l1);
void surface_sprite_clipping_from9_software(Surface *surface, int x, int y,
                                   int draw_width, int draw_height,
                                   int sprite_id, int mask_colour,
                                   int skin_colour, int skew_x, int flip);
void surface_sprite_clipping_from9_depth(Surface *surface, int x, int y,
                                         int draw_width, int draw_height,
                                         int sprite_id, int mask_colour,
                                         int skin_colour, int skew_x, int flip,
                                         float depth_top, float depth_bottom);
void surface_sprite_clipping_from9(Surface *surface, int x, int y, int w, int h,
                                   int sprite_id, int colour1, int colour2,
                                   int skew_x, int flag);
void surface_transparent_sprite_plot_from15(Surface *surface, int32_t *dest,
                                            int32_t *src, int j, int k,
                                            int dest_pos, int i1, int j1,
                                            int k1, int l1, int i2, int j2,
                                            int k2, int l2, int i3);
void surface_transparent_sprite_plot_from16(Surface *surface, int32_t *dest,
                                            int32_t *src, int j, int k,
                                            int dest_pos, int i1, int j1,
                                            int k1, int l1, int i2, int j2,
                                            int k2, int l2, int i3, int j3);
void surface_transparent_sprite_plot_from16a(Surface *surface, int32_t *dest,
                                             int8_t *colour_idx,
                                             int32_t *colours, int j, int k,
                                             int l, int i1, int j1, int k1,
                                             int l1, int i2, int j2, int k2,
                                             int l2, int i3);
void surface_transparent_sprite_plot_from17(Surface *surface, int32_t *dest,
                                            int8_t *colour_idx,
                                            int32_t *colours, int j, int k,
                                            int l, int i1, int j1, int k1,
                                            int l1, int i2, int j2, int k2,
                                            int l2, int i3, int j3);
void surface_draw_string_right(Surface *surface, char *text, int x, int y,
                               int font, int colour);
void surface_draw_string_centre(Surface *surface, char *text, int x, int y,
                                int font, int colour);
void surface_draw_paragraph(Surface *surface, char *text, int x, int y,
                            int font, int colour, int max);
void surface_draw_string(Surface *surface, char *text, int x, int y, int font,
                         int colour);
void surface_draw_character(Surface *surface, int font_offset, int x, int y,
                            int colour, int8_t *font_data);
void surface_plot_letter(int32_t *dest, int8_t *font_data, int colour,
                         int font_pos, int dest_pos, int width, int height,
                         int dest_offset, int font_data_offset);
int surface_text_height(int font_id);
int surface_text_height_font(int font_id);
int surface_text_width(char *text, int font_id);
void surface_draw_tabs(Surface *surface, int x, int y, int width, int height,
                       char **tabs, int tabs_length, int selected);

#endif
