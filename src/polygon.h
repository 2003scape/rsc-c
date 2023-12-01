#ifndef _H_POLYGON
#define _H_POLYGON

typedef struct GamePolygon GamePolygon;

#include "game-model.h"

struct GamePolygon {
    int16_t min_plane_x;
    int16_t min_plane_y;
    int16_t max_plane_x;
    int16_t max_plane_y;
    int16_t face;
    int16_t depth;
    int16_t normal_x;
    int16_t normal_y;
    int16_t normal_z;
    uint8_t visited;
    int32_t min_z;
    int32_t max_z;
    int32_t visibility;
    int32_t facefill;
    int32_t index;
    int32_t index2;
    GameModel *model;
};

void polygon_new(GamePolygon *polygon);

#endif
