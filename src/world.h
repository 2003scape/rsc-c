#ifndef _H_WORLD
#define _H_WORLD

#include <stdint.h>
#include <stdio.h>

/* TODO remove */
#include <stdlib.h>

typedef struct World World;

#include "game-data.h"
#include "game-model.h"
#include "scene.h"
#include "surface.h"

#define COLOUR_TRANSPARENT 12345678

#define REGION_WIDTH 96
#define REGION_HEIGHT 96

#define TILE_SIZE 128
#define PLANE_COUNT 4
#define TILE_COUNT 2304
#define TERRAIN_COUNT 64
#define TERRAIN_COLOUR_COUNT 256
#define LOCAL_COUNT 18432
#define REGION_SIZE 48
#define PLANE_HEIGHT 80000

/* length of the portion of the roof hanging over the building */
#define ROOF_SLOPE 16

/* https://github.com/2003scape/rsc-config/blob/master/res/types.json#L14 */
#define FLOOR_TILE_TYPE 2
#define LIQUID_TILE_TYPE 3
#define BRIDGE_TILE_TYPE 4
#define HOLE_TILE_TYPE 5

/* https://github.com/2003scape/rsc-config/blob/master/config-json/tiles.json */
#define BRIDGE_TILE_DECORATION 12

extern int terrain_colours[TERRAIN_COLOUR_COUNT];

void init_world_global();

typedef struct World {
    Scene *scene;
    Surface *surface;
    int player_alive;
    int base_media_sprite;
    int8_t *landscape_pack;
    int8_t *map_pack;
    int8_t *member_landscape_pack;
    int8_t *member_map_pack;
    GameModel *parent_model;
    int object_adjacency[REGION_WIDTH][REGION_HEIGHT];
    int route_via[REGION_WIDTH][REGION_HEIGHT];
    int terrain_height_local[REGION_WIDTH][REGION_HEIGHT];
    int walls_diagonal[PLANE_COUNT][TILE_COUNT];
    GameModel *terrain_models[TERRAIN_COUNT];
    int8_t terrain_colour[PLANE_COUNT][TILE_COUNT];
    int8_t terrain_height[PLANE_COUNT][TILE_COUNT];
    int8_t tile_decoration[PLANE_COUNT][TILE_COUNT];
    int8_t tile_direction[PLANE_COUNT][TILE_COUNT];
    GameModel *wall_models[PLANE_COUNT][TILE_COUNT];
    int8_t walls_east_west[PLANE_COUNT][TILE_COUNT];
    int8_t walls_north_south[PLANE_COUNT][TILE_COUNT];
    GameModel *roof_models[PLANE_COUNT][TILE_COUNT];
    int8_t walls_roof[PLANE_COUNT][TILE_COUNT];
    int local_x[LOCAL_COUNT];
    int local_y[LOCAL_COUNT];
} World;

void world_new(World *world, Scene *scene, Surface *surface);
int get_byte_plane_coord(int8_t plane_array[PLANE_COUNT][TILE_COUNT], int x,
                         int y);
int get_int_plane_coord(int plane_array[PLANE_COUNT][TILE_COUNT], int x, int y);
int world_get_wall_east_west(World *world, int x, int y);
void world_set_terrain_ambience(World *world, int x, int y, int x2, int y2,
                                int ambience);
int world_get_wall_roof(World *world, int x, int y);
int world_get_elevation(World *world, int x, int y);
int world_get_wall_diagonal(World *world, int x, int y);
void world_remove_object2(World *world, int x, int y, int id);
void world_remove_wall_object(World *world, int x, int y, int k, int id);
void world_draw_map_tile(World *world, int i, int j, int k, int l,
                         int texture_id_2);
void world_load_section_from4i(World *world, int x, int y, int plane,
                               int chunk);
void world_method404(World *world, int x, int y, int width, int height);
int world_get_object_adjacency(World *world, int x, int y);
int world_has_roof(World *world, int x, int y);
void world_method407(World *world, int i, int j, int k);
int world_get_terrain_colour(World *world, int x, int y);
void world_reset(World *world, int dispose);
void world_set_tiles(World *world);
int world_get_wall_north_south(World *world, int x, int y);
int world_get_tile_direction(World *world, int x, int y);
int world_get_tile_decoration(World *world, int x, int y);
int world_get_tile_decoration_from4(World *world, int x, int y, int unused,
                                    int def);
int world_get_tile_decoration(World *world, int x, int y);
void world_set_tile_decoration(World *world, int x, int y, int decoration);
int world_route(World *world, int start_x, int start_y, int end_x1, int end_y1,
                int end_x2, int end_y2, int *route_x, int *route_y,
                int objects);
void world_set_object_adjacency_from4(World *world, int x, int y, int dir,
                                      int id);
void world_load_section_from4(World *world, int x, int y, int plane,
                              int is_current_plane);
void world_set_object_adjacency_from3(World *world, int i, int j, int k);
int world_get_tile_type(World *world, int i, int j);
void world_add_models(World *world, GameModel **models);
void world_create_wall(World *world, GameModel *game_model, int wall_object_id,
                       int x1, int y1, int x2, int y2);
int world_get_terrain_height(World *world, int x, int y);
void world_load_section_from3(World *world, int x, int y, int plane);
void world_method425(World *world, int i, int j, int k);
void world_remove_object(World *world, int x, int y, int id);
int world_has_neighbouring_roof(World *world, int x, int y);
void world_raise_wall_object(World *world, int wall_object_id, int x1, int y1,
                             int x2, int y2);

#endif
