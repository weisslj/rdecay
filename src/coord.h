/* 
 * coord.h - das Koordinatensystem
 *
 * Copyright 2004 Johannes Weißl
 *
 * This file is part of rdecay.
 *
 * rdecay is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * rdecay is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rdecay; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

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

    GList *graphs;
};

CoordSystem *coord_system_new(gint field_width, gint field_height,
                              gdouble min_x, gdouble max_x,
                              gdouble min_y, gdouble max_y);

void coord_system_adjust(CoordSystem *coord,
                         gint field_width, gint field_height,
                         gdouble min_x, gdouble max_x,
                         gdouble min_y, gdouble max_y);

void coord_system_free(CoordSystem *coord);

void coord_get_real(gdouble *x, gdouble *y, CoordSystem *coord);
gint coord_real_x(gdouble x, CoordSystem *coord);
gint coord_real_y(gdouble y, CoordSystem *coord);

#endif /* _COORD_H */
