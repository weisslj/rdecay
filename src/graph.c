#include <stdio.h>
#include <gtk/gtk.h>

#include "graph.h"

CoordSystem *create_coord_system(gint field_width, gint field_height)
{
    CoordSystem *coord;
    gint x_axis, y_axis;

    coord = (CoordSystem *) g_malloc(sizeof(CoordSystem));

    x_axis = 0.95 * field_width;
    y_axis = 0.95 * field_height;

    coord->len_top = 0.95 * y_axis;
    coord->len_bottom = y_axis - coord->len_top;

    coord->len_right = 0.95 * x_axis;
    coord->len_left = x_axis - coord->len_right;

    coord->zero.x = ((field_width - x_axis) / 2) + coord->len_left;
    coord->zero.y = ((field_height - y_axis) / 2) + coord->len_top;

    coord->unit = 20;

    return coord;
}

void destroy_coord_system(CoordSystem *coord)
{
    g_free(coord);
}

Point coord_get_real_point(Point coord_point, CoordSystem *coord)
{
    Point real_point;

    real_point.x = coord->zero.x + coord_point.x;
    real_point.y = coord->zero.y - coord_point.y;

    return real_point;
}
