#ifndef _H_WORLD
#define _H_WORLD

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "game-data.h"
#include "game-model.h"
#include "scene.h"
#include "surface.h"

#define COLOUR_TRANSPARENT 12345678
#define REGION_WIDTH 96
#define REGION_HEIGHT 96
#define AN_INT_585 128

#define PLANE_COUNT 4
#define TILE_COUNT 2304
#define TERRAIN_COUNT 64
#define TERRAIN_COLOUR_COUNT 256
#define LOCAL_COUNT 18432
#define REGION_SIZE 48

extern int terrain_colours[TERRAIN_COLOUR_COUNT];

void init_world_global();

typedef struct World {
    Scene *scene;
    Surface *surface;
    int world_initialised;
    int8_t *landscape_pack;
    int8_t *map_pack;
    int8_t *member_landscape_pack;
    int8_t *member_map_pack;
    GameModel *parent_model;
    int object_adjacency[PLANE_COUNT][TILE_COUNT];
    int route_via[PLANE_COUNT][TILE_COUNT];
    int terrain_height_local[PLANE_COUNT][TILE_COUNT];
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
    int player_alive;
    int a_boolean592;
    int base_media_sprite;
} World;

void world_new(World *world, Scene *scene, Surface *surface);
int get_byte_plane_coord(int8_t plane_array[PLANE_COUNT][TILE_COUNT], int x, int y);
int get_int_plane_coord(int plane_array[PLANE_COUNT][TILE_COUNT], int x, int y);
int world_get_wall_east_west(World *world, int x, int y);
void world_set_terrain_ambience(World *world, int x, int y, int x2, int y2,
                                int ambience);
int world_get_wall_roof(World *world, int x, int y);
int world_get_elevation(World *world, int x, int y);
int world_get_wall_diagonal(World *world, int x, int y);
void world_remove_object2(World *world, int x, int y, int id);
void world_remove_wall_object(World *world, int x, int y, int k, int id);
void world_method402(World *world, int i, int j, int k, int l, int i1);
void world_load_section_from4i(World *world, int x, int y, int plane,
                               int chunk);
void world_method404(World *world, int x, int y, int k, int l);
int world_get_object_adjacency(World *world, int x, int y);
int world_has_roof(World *world, int x, int y);
void world_method407(World *world, int i, int j, int k);
int world_get_terrain_colour(World *world, int x, int y);
void world_reset(World *world);
void world_set_tiles(World *world);
int world_get_wall_north_south(World *world, int x, int y);
int world_get_tile_direction(World *world, int x, int y);
int world_get_tile_decoration(World *world, int x, int y);
int world_get_tile_decoration_from4(World *world, int x, int y, int unused,
                                    int def);
int world_get_tile_decoration(World *world, int x, int y);
void world_set_tile_decoration(World *world, int x, int y, int decoration);
int world_route(World *world, int start_x, int start_y, int end_x1, int end_y1,
                int end_x2, int end_y2, int *route_x, int *route_y, int objects,
                int size);
void world_set_object_adjacency_from4(World *world, int x, int y, int dir,
                                      int id);
void world_load_section_from4(World *world, int x, int y, int plane, int flag);
void world_set_object_adjacency_from3(World *world, int i, int j, int k);
int world_get_tile_type(World *world, int i, int j, int k);
void world_add_models(World *world, GameModel **models);
void world_method422(World *world, GameModel *game_model, int i, int j, int k,
                     int l, int i1);
int world_get_terrain_height(World *world, int x, int y);
void world_load_section_from3(World *world, int x, int y, int plane);
void world_method425(World *world, int i, int j, int k);
void world_remove_object(World *world, int x, int y, int id);
int world_method427(World *world, int i, int j);
void world_method428(World *world, int i, int j, int k, int l, int i1);

#endif
