#ifndef _GRAPH_H
#define _GRAPH_H

#include <glib.h>

typedef struct _Point Point;

struct _Point {
    gint x;
    gint y;
};


typedef struct _CoordSystem CoordSystem;

struct _CoordSystem {
    Point zero;

    gint unit;

    gint len_top;
    gint len_left;
    gint len_bottom;
    gint len_right;
};

CoordSystem *create_coord_system(gint field_width, gint field_height);

Point coord_get_real_point(Point coord_point, CoordSystem *coord);

void destroy_coord_system(CoordSystem *coord);

#endif /* _GRAPH_H */
