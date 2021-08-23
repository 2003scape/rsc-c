#ifndef _H_SCENE
#define _H_SCENE

#include <stdint.h>

#include "game-model.h"
#include "polygon.h"
#include "scanline.h"
#include "surface.h"
#include "utility.h"

#define VERTEX_COUNT 40
#define RAMP_COUNT 50
#define MOUSE_PICKED_MAX 100

#define COLOUR_TRANSPARENT 12345678

extern int scene_frustum_max_x;
extern int scene_frustum_min_x;
extern int scene_frustum_max_y;
extern int scene_frustum_min_y;
extern int scene_frustum_far_z;
extern int scene_frustum_near_z;

static uint64_t texture_count_loaded;

typedef struct Scene {
    int last_visible_polygons_count;
    int clip_near;
    int clip_far_3d;
    int clip_far_2d;
    int fog_z_falloff;
    int fog_z_distance;
    int wide_band;
    int model_count;
    int max_model_count;
    GameModel *models;
    GameModel *view;
    uint32_t *raster;
    int gradient_base[RAMP_COUNT];
    int gradient_ramps[RAMP_COUNT][256];
    int *an_int_array_377;
    int texture_count;
    int8_t **texture_colours_used;
    uint32_t **texture_colour_list;
    int *texture_dimension;
    int64_t *texture_loaded_number;
    uint32_t **texture_pixels;
    int8_t *texture_back_transparent;
    uint32_t **texture_colours_64;
    uint32_t **texture_colours_128;
    Surface *surface;
    Scanline *scanlines;
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
    Polygon **visible_polygons;
    int sprite_count;
    int *sprite_id;
    int *sprite_x;
    int *sprite_z;
    int *sprite_y;
    int *sprite_width;
    int *sprite_height;
    int *sprite_translate_x;
} Scene;

void scene_new(Scene *scene, Surface *surface, int model_count,
               int polygon_count, int sprite_count);
void scene_texture_scanline(uint32_t *ai, uint32_t *ai1, int i, int j, int k,
                            int l, int i1, int j1, int k1, int l1, int i2,
                            int j2, int k2, int l2);
void scene_texture_translucent_scanline(uint32_t *ai, uint32_t *ai1, int i,
                                        int j, int k, int l, int i1, int j1,
                                        int k1, int l1, int i2, int j2, int k2,
                                        int l2);
void scene_texture_back_translucent_scanline(uint32_t *ai, int i, int j, int k,
                                             uint32_t *ai1, int l, int i1,
                                             int j1, int k1, int l1, int i2,
                                             int j2, int k2, int l2, int i3);
void scene_texture_scanline2(uint32_t *ai, uint32_t *ai1, int i, int j, int k,
                             int l, int i1, int j1, int k1, int l1, int i2,
                             int j2, int k2, int l2);
void scene_texture_translucent_scanline2(uint32_t *ai, uint32_t *ai1, int i,
                                         int j, int k, int l, int i1, int j1,
                                         int k1, int l1, int i2, int j2, int k2,
                                         int l2);
void scene_texture_back_translucent_scanline2(uint32_t *ai, int i, int j, int k,
                                              uint32_t *ai1, int l, int i1,
                                              int j1, int k1, int l1, int i2,
                                              int j2, int k2, int l2, int i3);
void scene_gradient_scanline(uint32_t *ai, int i, int j, int k, uint32_t *ai1,
                             int l, int i1);
void scene_texture_gradient_scanline(uint32_t *ai, int i, int j, int k,
                                     uint32_t *ai1, int l, int i1);
void scene_gradient_scanline2(uint32_t *ai, int i, int j, int k, uint32_t *ai1,
                              int l, int j1);
int rgb(int i, int j, int k);

#endif
