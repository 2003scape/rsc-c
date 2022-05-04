#ifndef _H_SCENE
#define _H_SCENE

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>

typedef struct Scene Scene;

#include "game-model.h"
#include "polygon.h"
#include "scanline.h"
#include "surface.h"
#include "utility.h"

#define VERTEX_COUNT 40
#define RAMP_COUNT 50
#define RAMP_SIZE 256

/* originally Scene had a wide_band property and a second gradient_scanline
 * function that was unused */
#define RAMP_WIDE 0

#define MOUSE_PICKED_MAX 100

#define COLOUR_TRANSPARENT 12345678

/* width and height of scrollable textures */
#define SCROLL_TEXTURE_SIZE 64
#define SCROLL_TEXTURE_AREA (SCROLL_TEXTURE_SIZE * SCROLL_TEXTURE_SIZE)

extern int scene_frustum_max_x;
extern int scene_frustum_min_x;
extern int scene_frustum_max_y;
extern int scene_frustum_min_y;
extern int scene_frustum_far_z;
extern int scene_frustum_near_z;

extern int64_t scene_texture_count_loaded;

typedef struct Scene {
    int max_sprite_count;
    int last_visible_polygons_count;
    int clip_near;
    int clip_far_3d;
    int clip_far_2d;
    int fog_z_falloff;
    int fog_z_distance;
    int model_count;
    int max_model_count;
    GameModel **models;
    GameModel *view;
    int32_t *raster;
    int gradient_base[RAMP_COUNT];
    int gradient_ramps[RAMP_COUNT][RAMP_SIZE];
    int32_t *gradient_ramp;
    int texture_count;
    int8_t **texture_colours_used;
    int32_t **texture_colour_list;
    int8_t *texture_dimension;
    int64_t *texture_loaded_number;
    int32_t **texture_pixels;
    int8_t *texture_back_transparent;
    int32_t **texture_colours_64;
    int length_64;
    int32_t **texture_colours_128;
    int length_128;
    Surface *surface;
    Scanline **scanlines;
    int min_y;
    int max_y;
    int plane_x[VERTEX_COUNT];
    int plane_y[VERTEX_COUNT];
    int vertex_shade[VERTEX_COUNT];
    int vertex_x[VERTEX_COUNT];
    int vertex_y[VERTEX_COUNT];
    int vertex_z[VERTEX_COUNT];
    int interlace;
    int new_start;
    int new_end;
    int mouse_picking_active;
    int mouse_x;
    int mouse_y;
    int mouse_picked_count;
    GameModel *mouse_picked_models[MOUSE_PICKED_MAX];
    int mouse_picked_faces[MOUSE_PICKED_MAX];
    int width;
    int clip_x;
    int clip_y;
    int base_x;
    int base_y;
    int view_distance;
    int normal_magnitude;
    int camera_x;
    int camera_y;
    int camera_z;
    int camera_yaw;
    int camera_pitch;
    int camera_roll;
    int visible_polygons_count;
    GamePolygon **visible_polygons;
    int sprite_count;
    int *sprite_id;
    int *sprite_x;
    int *sprite_z;
    int *sprite_y;
    int *sprite_width;
    int *sprite_height;
    int *sprite_translate_x;
} Scene;

int scene_polygon_depth_compare(const void *a, const void *b);

void scene_new(Scene *scene, Surface *surface, int model_count,
               int polygon_count, int max_sprite_count);
void scene_texture_scanline(int32_t *raster, int32_t *texture_pixels, int k,
                            int l, int i1, int j1, int k1, int l1, int i2,
                            int j2, int k2, int l2);
void scene_texture_back_translucent_scanline(int32_t *raster,
                                             int32_t *texture_pixels, int l,
                                             int i1, int j1, int k1, int l1,
                                             int i2, int j2, int k2, int l2,
                                             int i3);
void scene_texture_scanline2(int32_t *raster, int32_t *texture_pixels, int k,
                             int l, int i1, int j1, int k1, int l1, int i2,
                             int j2, int k2, int l2);
void scene_texture_back_translucent_scanline2(int32_t *raster,
                                              int32_t *texture_pixels, int l,
                                              int i1, int j1, int k1, int l1,
                                              int i2, int j2, int k2, int l2,
                                              int i3);
void scene_gradient_translucent_scanline(int32_t *raster, int i, int j,
                                         int32_t *ramp, int l, int i1);
void scene_gradient_scanline(int32_t *raster, int i, int raster_idx,
                             int32_t *ramp, int ramp_index, int ramp_inc);
int rgb(int i, int j, int k);
void scene_add_model(Scene *scene, GameModel *model);
void scene_remove_model(Scene *scene, GameModel *model);
void scene_null_model(Scene *scene, GameModel *model);
void scene_dispose(Scene *scene);
void scene_clear(Scene *scene);
void scene_reduce_sprites(Scene *scene, int i);
int scene_add_sprite(Scene *scene, int n, int x, int z, int y, int w, int h,
                     int tag);
void scene_set_local_player(Scene *scene, int i);
void scene_set_sprite_translate_x(Scene *scene, int i, int n);
void scene_set_mouse_loc(Scene *scene, int x, int y);
void scene_set_bounds(Scene *scene, int base_x, int base_y, int clip_x,
                      int clip_y, int width, int view_distance);
void scene_polygons_intersect_sort(Scene *scene, int step, GamePolygon **polygons,
                                   int count);
int scene_polygons_order(Scene *scene, GamePolygon **polygons, int start, int end);
void scene_set_frustum(Scene *scene, int x, int y, int z);
void scene_render(Scene *scene);
void scene_generate_scanlines(Scene *scene, int plane, int32_t *plane_x,
                              int32_t *plane_y, int32_t *vertex_shade,
                              GameModel *game_model, int face);
void scene_rasterize(Scene *scene, int num_vertices, int32_t *vertices_x,
                     int32_t *vertices_y, int32_t *vertices_z, int face_fill,
                     GameModel *game_model);
void scene_set_camera(Scene *scene, int x, int z, int y, int pitch, int yaw,
                      int roll, int distance);
void scene_initialise_polygon_3d(Scene *scene, int polygon_index);
void scene_initialise_polygon_2d(Scene *scene, int polygon_index);
int scene_separate_polygon(GamePolygon *polygon_a, GamePolygon *polygon_b);
int scene_heuristic_polygon(GamePolygon *polygon_a, GamePolygon *polygon_b);
void scene_allocate_textures(Scene *scene, int count, int length_64,
                             int length_128);
void scene_define_texture(Scene *scene, int id, int8_t *colour_idx,
                          int32_t *colours, int wide128);
void scene_prepare_texture(Scene *scene, int id);
void scene_set_texture_pixels(Scene *scene, int id);
void scene_scroll_texture(Scene *scene, int id);
int scene_get_fill_colour(Scene *scene, int face_fill);
void scene_set_light_from3(Scene *scene, int x, int y, int z);
void scene_set_light_from5(Scene *scene, int ambience, int diffuse, int x,
                           int y, int z);
int scene_method306(int i, int j, int k, int l, int i1);
int scene_method307(int i, int j, int k, int l, int flag);
int scene_method308(int i, int j, int k, int flag);
int scene_intersect(int *vertex_view_x_a, int *vertex_view_y_a,
                    int *vertex_view_x_b, int *vertex_view_y_b, int length_a,
                    int length_b);

#endif
