#include "polygon.h"

void polygon_new(Polygon *polygon) {
    //memset(polygon, 0, sizeof(Polygon));
    polygon->skip_something = 0;
    polygon->index2 = -1;
}
