#ifndef _H_SURFACE
#define _H_SURFACE

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "utility.h"

#ifdef WII
#include <gccore.h>

#define CLIP(X) ((X) > 255 ? 255 : (X) < 0 ? 0 : X)
#define RGB2Y(R, G, B) CLIP(((66 * (R) + 129 * (G) + 25 * (B) + 128) >> 8) + 16)
#define RGB2U(R, G, B) CLIP(((-38 * (R)-74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(((112 * (R)-94 * (G)-18 * (B) + 128) >> 8) + 128)
#elif defined(_3DS)
#include <3ds.h>

#ifdef RENDER_3DS_GL
#include <citro3d.h>
#include <tex3ds.h>

#include "flat_shbin.h"

#include "sprites_t3x.h"

#include "entities_0_t3x.h"
#include "entities_1_t3x.h"
#include "entities_2_t3x.h"
#include "entities_3_t3x.h"
#include "entities_4_t3x.h"
#endif
#endif

#ifdef RENDER_GL
#ifdef GLAD
#include <glad/glad.h>
#else
#include <GL/glew.h>
#include <GL/glu.h>
#endif
#endif

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
#include <cglm/cglm.h>

#include "gl/shader.h"
#include "gl/vertex-buffer.h"

typedef struct gl_quad_vertex {
    float x, y, z;
    float r, g, b, a; /* mask colour */
    float u, v;       /* greyscale texture that is multiplied by mask colour */
    float base_u, base_v; /* non grey-pixel portion that is added to coloured */
} gl_quad_vertex;

typedef struct gl_quad {
#ifdef RENDER_GL
    gl_quad_vertex bottom_left, bottom_right, top_right, top_left;
#elif defined(RENDER_3DS_GL)
    gl_quad_vertex top_right, top_left, bottom_left, bottom_right;
#endif
} gl_quad;

/* atlas positions in ./textures/ to generate UVs */
typedef struct gl_atlas_position {
    float left_u, right_u;
    float top_v, bottom_v;
} gl_atlas_position;

#include "gl/textures/entities.h"
#include "gl/textures/fonts.h"
#include "gl/textures/media.h"

#define GL_MAX_QUADS 2048

typedef struct SurfaceGlContext {
#ifdef RENDER_GL
    GLuint texture;
    GLuint base_texture;
#elif defined(RENDER_3DS_GL)
    C3D_Tex *texture;
    C3D_Tex *base_texture;
#endif

    int quad_count;

    /* boundaries for minimap drawing */
    int min_x;
    int max_x;
    int min_y;
    int max_y;

    int use_depth;
} SurfaceGlContext;

extern gl_atlas_position gl_white_atlas_position;
extern gl_atlas_position gl_transparent_atlas_position;
#endif

#define SURFACE_STRING_MAX 256

#define SLEEP_WIDTH 255
#define SLEEP_HEIGHT 40

#define ITEM_GRID_SLOT_WIDTH 49
#define ITEM_GRID_SLOT_HEIGHT 34

/* colour of scrollbar background gradients */
#define SCROLLBAR_TOP_COLOUR 0x7272b0
#define SCROLLBAR_BOTTOM_COLOUR 0x0e0e3e

/* colour of the interactive scrub/handle in the scrollbar */
#define SCRUB_LEFT_COLOUR 0xc8d0e8
#define SCRUB_MIDDLE_COLOUR 0x6081b8
#define SCRUB_RIGHT_COLOUR 0x355f73

#define MINIMAP_SPRITE_WIDTH 285
#define MINIMAP_SPRITE_HEIGHT MINIMAP_SPRITE_WIDTH

typedef enum {
    FONT_REGULAR_11 = 0,
    FONT_BOLD_12 = 1,
    FONT_REGULAR_12 = 2,
    FONT_BOLD_13 = 3,
    FONT_BOLD_14 = 4,
    FONT_BOLD_16 = 5,
    FONT_BOLD_20 = 6,
    FONT_BOLD_24 = 7
} FontStyle;

typedef struct Surface Surface;

#include "mudclient.h"

extern int an_int_346;
extern int an_int_347;
extern int an_int_348;
extern int8_t *game_fonts[50];
extern int character_width[256];
extern int32_t *surface_texture_pixels;

void init_surface_global(void);

struct Surface {
    int limit;
    int width;
    int height;
    int32_t *pixels;
    int32_t **surface_pixels; // TODO rename
    int8_t **sprite_colours;
    int32_t **sprite_palette;
    int16_t *sprite_width;
    int16_t *sprite_height;
    int16_t *sprite_width_full;
    int16_t *sprite_height_full;
    int8_t *sprite_translate;
    int16_t *sprite_translate_x;
    int16_t *sprite_translate_y;

    int8_t interlace;
    int8_t draw_string_shadow;

    /* used to prevent the minimap from drawing outside of the map UI, and to
     * prevent software rendering from writing outside of the raster array */
    int bounds_min_y;
    int bounds_max_y;
    int bounds_min_x;
    int bounds_max_x;

    mudclient *mud;

#ifdef RENDER_SW
    /* arrays used for minimap sprite rotation */
    int *rotations_0;
    int *rotations_1;
    int *rotations_2;
    int *rotations_3;
    int *rotations_4;
    int *rotations_5;
    int rotations_length;
#elif defined(RENDER_GL)
    Shader gl_flat_shader;

    GLuint gl_sprite_texture;
    GLuint gl_entity_textures[ENTITY_TEXTURE_LENGTH];

    uint8_t *gl_dynamic_texture_buffer;
    GLuint gl_dynamic_texture;
    // GLuint gl_framebuffer_texture;

    // int32_t *gl_screen_pixels_reversed;
    int32_t *gl_screen_pixels;
#elif defined(RENDER_3DS_GL)
    DVLB_s *_3ds_gl_flat_shader_dvlb;
    shaderProgram_s _3ds_gl_flat_shader;

    int _3ds_gl_projection_uniform;

    C3D_Mtx _3ds_gl_projection;

    C3D_Tex gl_sprite_texture;
    C3D_Tex gl_entity_textures[ENTITY_TEXTURE_LENGTH];
#endif

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
    gl_vertex_buffer gl_flat_buffer;
    int gl_flat_count;

    /* used for texture array and boundary changes */
    SurfaceGlContext gl_contexts[GL_MAX_QUADS];
    int gl_context_count;
#endif
};

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
typedef enum {
    GL_DEPTH_BOTH = 0,
    GL_DEPTH_DISABLED = 1,
    GL_DEPTH_ENABLED = 2
} GL_DEPTH_MODE;
#endif

void create_font(int8_t *buffer, size_t id);

void surface_new(Surface *surface, int width, int height, int limit,
                 mudclient *mud);

#if defined(RENDER_GL) || defined(RENDER_3DS_GL)
float surface_gl_translate_x(Surface *surface, int x);
float surface_gl_translate_y(Surface *surface, int y);
void surface_gl_reset_context(Surface *surface);
void surface_gl_quad_apply_atlas(gl_quad *quad,
                                 gl_atlas_position atlas_position, int flip);
void surface_gl_quad_apply_base_atlas(gl_quad *quad,
                                      gl_atlas_position atlas_position,
                                      int flip);
void surface_gl_vertex_apply_depth(gl_quad_vertex *vertices, int length,
                                   float depth);
void surface_gl_vertex_apply_rotation(gl_quad_vertex *vertex, float centre_x,
                                      float centre_y, float angle);
#ifdef RENDER_GL
void surface_gl_buffer_quad(Surface *surface, gl_quad *quad, GLuint texture,
                            GLuint base_texture);
#elif defined(RENDER_3DS_GL)
void surface_gl_buffer_quad(Surface *surface, gl_quad *quad, C3D_Tex *texture,
                            C3D_Tex *base_texture);
#endif
void surface_gl_vertex_apply_colour(gl_quad_vertex *vertices, int length,
                                    int colour, int alpha);
void surface_gl_buffer_box(Surface *surface, int x, int y, int width,
                           int height, int colour, int alpha);
void surface_gl_buffer_character(Surface *surface, char character, int x, int y,
                                 int colour, int font_id, int draw_shadow,
                                 float depth);
void surface_gl_buffer_sprite(Surface *surface, int sprite_id, int x, int y,
                              int draw_width, int draw_height, int skew_x,
                              int mask_colour, int skin_colour, int alpha,
                              int flip, int rotation, float depth_top,
                              float depth_bottom);
void surface_gl_buffer_circle(Surface *surface, int x, int y, int radius,
                              int colour, int alpha, float depth);
void surface_gl_buffer_gradient(Surface *surface, int x, int y, int width,
                                int height, int top_colour, int bottom_colour);
void surface_gl_draw(Surface *surface, GL_DEPTH_MODE depth_mode);
void surface_gl_blur_texture(Surface *surface, int sprite_id, int blur_height,
                             int x, int y, int height);
void surface_gl_apply_login_filter(Surface *surface, int sprite_id);
#endif

void surface_set_bounds(Surface *surface, int min_x, int min_y, int max_x,
                        int max_y);
void surface_reset_bounds(Surface *surface);
void surface_reset_draw(Surface *surface);
void surface_draw(Surface *surface);
void surface_black_screen(Surface *surface);
void surface_draw_circle(Surface *surface, int x, int y, int radius, int colour,
                         int alpha, float depth);
void surface_draw_box_alpha(Surface *surface, int x, int y, int width,
                            int height, int colour, int alpha);
void surface_draw_gradient(Surface *surface, int x, int y, int width,
                           int height, int top_colour, int bottom_colour);
void surface_draw_box(Surface *surface, int x, int y, int width, int height,
                      int colour);
void surface_draw_line_horizontal(Surface *surface, int x, int y, int width,
                                  int colour);
void surface_draw_line_vertical(Surface *surface, int x, int y, int height,
                                int colour);
void surface_draw_border(Surface *surface, int x, int y, int width, int height,
                         int colour);
void surface_set_pixel(Surface *surface, int x, int y, int colour);
void surface_fade_to_black_software(Surface *surface, int32_t *dest,
                                    int add_alpha);
void surface_fade_to_black(Surface *surface);
void surface_apply_login_filter(Surface *surface, int background_height);
void surface_clear(Surface *surface);
void surface_parse_sprite_tga(Surface *surface, int sprite_id,
                              int8_t *sprite_data, size_t len, int columns,
                              int rows);
void surface_parse_sprite(Surface *surface, int sprite_id, int8_t *sprite_data,
                          int8_t *index_data, int frame_count);
void surface_read_sleep_word(Surface *surface, int sprite_id,
                             int8_t *sprite_data);
int32_t *surface_palette_sprite_to_raster(Surface *surface, int sprite_id,
                                          int add_alpha);
void surface_load_sprite(Surface *surface, int sprite_id);
void surface_screen_raster_to_sprite(Surface *surface, int sprite_id, int x,
                                     int y, int width, int height);
void surface_draw_sprite_reversed(Surface *surface, int sprite_id, int x, int y,
                                  int width, int height);
void surface_draw_sprite(Surface *surface, int x, int y, int sprite_id);
void surface_draw_sprite_depth(Surface *surface, int x, int y, int sprite_id,
                               float depth_top, float depth_bottom);
void surface_draw_sprite_scale(Surface *surface, int x, int y, int width,
                               int height, int sprite_id, float depth);
void surface_draw_entity_sprite(Surface *surface, int x, int y, int width,
                                int height, int sprite_id, int tx, int ty,
                                float depth_top, float depth_bottom);
void surface_draw_sprite_alpha(Surface *surface, int x, int y, int sprite_id,
                               int alpha);
void surface_draw_sprite_scale_alpha(Surface *surface, int x, int y,
                                     int scale_x, int scale_y, int sprite_id,
                                     int alpha);
void surface_draw_sprite_scale_mask(Surface *surface, int x, int y, int width,
                                    int height, int sprite_id, int colour);
void surface_transparent_scale(int32_t *dest, int32_t *src, int j, int k,
                               int dest_pos, int i1, int j1, int k1, int l1,
                               int i2, int j2, int y_inc, int alpha);
void surface_draw_minimap_sprite(Surface *surface, int x, int y, int sprite_id,
                                 int rotation, int scale);
void surface_draw_minimap(int32_t *dest, int32_t *src, int j, int k, int l,
                          int i1, int j1, int k1, int l1);
void surface_draw_minimap_translate(int32_t *dest, int32_t *src, int j, int k,
                                    int l, int i1, int j1, int k1, int l1);
void surface_draw_sprite_transform_mask(Surface *surface, int x, int y,
                                        int width, int height, int sprite_id,
                                        int mask_colour, int skin_colour,
                                        int skew_x, int flip);
void surface_draw_sprite_transform_mask_depth(Surface *surface, int x, int y,
                                              int draw_width, int draw_height,
                                              int sprite_id, int mask_colour,
                                              int skin_colour, int skew_x,
                                              int flip, float depth_top,
                                              float depth_bottom);
#ifdef RENDER_SW
void surface_screen_raster_to_palette_sprite(Surface *surface, int sprite_id);
void surface_draw_character(Surface *surface, int font_offset, int x, int y,
                            int colour, int8_t *font_data);
void surface_draw_blur(Surface *surface, int blur_height, int x, int y,
                       int width, int height);
#endif
void surface_draw_string_depth(Surface *surface, const char *text, int x, int y,
                               FontStyle font, int colour, float depth);
void surface_draw_string(Surface *surface, const char *text, int x, int y,
                         FontStyle font, int colour);
void surface_draw_string_right(Surface *surface, const char *text, int x, int y,
                               FontStyle font, int colour);
void surface_draw_string_centre_depth(Surface *surface, const char *text, int x,
                                      int y, FontStyle font, int colour,
                                      float depth);
void surface_draw_string_centre(Surface *surface, const char *text, int x,
                                int y, FontStyle font, int colour);
void surface_draw_stringf(Surface *surface, int x, int y, FontStyle font,
                          int colour, const char *text, ...);
void surface_draw_stringf_centre(Surface *surface, int x, int y, FontStyle font,
                                 int colour, const char *text, ...);
void surface_draw_stringf_right(Surface *surface, int x, int y, FontStyle font,
                                int colour, const char *text, ...);
int surface_paragraph_height(Surface *surface, const char *text, FontStyle font,
                             int max, int max_height);
void surface_draw_paragraph(Surface *surface, const char *text, int x, int y,
                            FontStyle font, int colour, int max);
int surface_text_height_font(FontStyle font);
int surface_text_height(FontStyle font);
int surface_text_width(const char *text, FontStyle font);
void surface_draw_tabs(Surface *surface, int x, int y, int width, int height,
                       const char **tabs, int tabs_length, int selected);
void surface_draw_item(Surface *surface, int x, int y, int slot_width,
                       int slot_height, int item_id);
void surface_draw_item_grid(Surface *surface, int x, int y, int rows,
                            int columns, int slot_width, int slot_height,
                            int *items, int *items_count, int items_length,
                            int selected, int show_inventory_count);
void surface_draw_scrollbar(Surface *surface, int x, int y, int width,
                            int height, int scrub_y, int scrub_height);
void surface_draw_status_bar(Surface *surface, int max, int current,
                             char *label, int x, int y, int width, int height,
                             int background_colour, int foreground_colour,
                             int is_percentage);
#ifdef RENDER_GL
void surface_gl_create_framebuffer(Surface *surface);
void surface_gl_update_dynamic_texture(Surface *surface);
#endif
#ifdef RENDER_3DS_GL
int surface_3ds_gl_get_sprite_texture_offsets(Surface *surface, int sprite_id,
                                              int *offset_x, int *offset_y);
#endif
#endif
