#ifndef _COORD_H
#define _COORD_H

#include <glib.h>

typedef struct _CoordSystem CoordSystem;

struct _CoordSystem {
    gdouble min_x;
    gdouble max_x;

    gdouble min_y;
    gdouble max_y;

    gdouble step_x;
    gdouble step_y;

    gdouble x_fact;
    gdouble y_fact;

    gint zero_x;
    gint zero_y;

    gint x_axis_begin;
    gint x_axis_end;
    gint y_axis_begin;
    gint y_axis_end;
};

CoordSystem *coord_system_new(gint field_width, gint field_height,
                              gdouble min_x, gdouble max_x,
                              gdouble min_y, gdouble max_y);

void coord_system_free(CoordSystem *coord);

void coord_get_real(gdouble *x, gdouble *y, CoordSystem *coord);
gdouble coord_real_x(gdouble x, CoordSystem *coord);
gdouble coord_real_y(gdouble y, CoordSystem *coord);

#endif /* _COORD_H */
