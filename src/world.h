#ifndef _H_WORLD
#define _H_WORLD

#include <stdint.h>

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

#define COORD_RANGE_CHECK(x, y) {\
    if (x < 0 || y < 0 || x >= 95 || y >= 95) {\
        return;\
    }\
}

static int terrain_colours[TERRAIN_COLOUR_COUNT];

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

#endif
