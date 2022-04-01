#include "polygon.h"

void polygon_new(GamePolygon *polygon) {
    memset(polygon, 0, sizeof(GamePolygon));
    polygon->skip_something = 0;
    polygon->index2 = -1;
}
