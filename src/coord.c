#include <stdio.h>
#include <gtk/gtk.h>

#include "coord.h"

static gdouble gen_unit_system(gdouble min, gdouble max);

CoordSystem *coord_system_new(gint field_width, gint field_height,
                              gdouble min_x, gdouble max_x,
                              gdouble min_y, gdouble max_y)
{
    CoordSystem *coord;
    gint x_axis, y_axis;

    if (min_x >= max_x || min_y >= max_y)
        return NULL;

    coord = (CoordSystem *) g_malloc(sizeof(CoordSystem));

    x_axis = 0.95 * field_width;
    y_axis = 0.95 * field_height;

    coord->x_axis_begin = (field_width - x_axis) / 2.0;
    coord->x_axis_end = coord->x_axis_begin + x_axis;

    coord->y_axis_end = (field_height - y_axis) / 2.0;
    coord->y_axis_begin = coord->y_axis_end + y_axis;

    coord->step_x = gen_unit_system(min_x, max_x);
    coord->step_y = gen_unit_system(min_y, max_y);

    coord->min_x = min_x;
    coord->max_x = max_x;
    coord->min_y = min_y;
    coord->max_y = max_y;

/*    coord->x_fact = (gdouble) (max_x - min_x) / (gdouble) x_axis;
    coord->y_fact = (gdouble) (max_y - min_y) / (gdouble) y_axis; */
    coord->x_fact = (gdouble) x_axis / (gdouble) (max_x - min_x);
    coord->y_fact = (gdouble) y_axis / (gdouble) (max_y - min_y);

    coord->zero_x = coord->x_axis_begin + ((gdouble) x_axis * ((gdouble) min_x / (gdouble) (max_x - min_x)));
    coord->zero_y = coord->y_axis_end + ((gdouble) y_axis * ((gdouble) max_y / (gdouble) (max_y - min_y)));

    return coord;
}

gdouble coord_real_x(gdouble x, CoordSystem *coord)
{
    return coord->zero_x + x * coord->x_fact;
}

gdouble coord_real_y(gdouble y, CoordSystem *coord)
{
    return coord->zero_y - y * coord->y_fact;
}

void coord_get_real(gdouble *x, gdouble *y, CoordSystem *coord)
{
    *x = coord->zero_x + *x * coord->x_fact;
    *y = coord->zero_y - *y * coord->y_fact;
}

void coord_system_free(CoordSystem *coord)
{
    g_free(coord);
}

static gdouble gen_unit_system(gdouble min, gdouble max)
{
    gdouble x, len, step;

    if (min > 0)
        min = 0;

    if (max < 0)
        max = 0;

    len = MAX(max - 0, 0 - min);

    for (x = 1; (len / x) > 1.0; x *= 10)
        ;

    step = x / 10;

    for (x = 0; x < (max - 0); x += step)
        ;

    max = x - 0;

    for (x = 0; x < (0 - min); x += step)
        ;

    min = 0 - x;

    return step;
}
