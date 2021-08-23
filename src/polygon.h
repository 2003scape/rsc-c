#ifndef _H_POLYGON
#define _H_POLYGON

typedef struct Polygon Polygon;

#include "game-model.h"

typedef struct Polygon {
    int min_plane_x;
    int min_plane_y;
    int max_plane_x;
    int max_plane_y;
    int min_z;
    int max_z;
    GameModel *model;
    int face;
    int depth;
    int normal_x;
    int normal_y;
    int normal_z;
    int visibility;
    int facefill;
    int skip_something;
    int index;
    int index2;
} Polygon;

void polygon_new(Polygon *polygon);

#endif
