/* 
 * coord.h - das Koordinatensystem
 *
 * Copyright 2004-2017 Johannes Weißl
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
 * along with rdecay.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _COORD_H
#define _COORD_H

#include "graph.h"

#include <gtk/gtk.h>

#define ARROW_WIDTH 5
#define ARROW_HEIGHT 5
#define ARROW_SHANK 20


typedef struct _CoordSystem CoordSystem;

struct _CoordSystem {
    gchar *x_title;
    gchar *y_title;
    gchar *x_unit;
    gchar *y_unit;
    
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

    gboolean fract_x;
    gboolean fract_y;

    gint x_axis_begin;
    gint x_axis_end;
    gint y_axis_begin;
    gint y_axis_end;

    GList *graphs;
};

CoordSystem *coord_system_new(GtkWidget *darea,
                              const gchar *x_title, const gchar *x_unit,
                              const gchar *y_title, const gchar *y_unit,
                              gboolean fract_x, gboolean fract_y,
                              gdouble min_x, gdouble max_x,
                              gdouble min_y, gdouble max_y);

void coord_system_add_graph(CoordSystem *coord, Graph *gr);
void coord_system_adjust(CoordSystem *coord, GtkWidget *darea,
                         gdouble min_x, gdouble max_x,
                         gdouble min_y, gdouble max_y);

void coord_system_free(CoordSystem *coord);
void coord_system_clear(CoordSystem *coord);

gint coord_real_x(gdouble x, CoordSystem *coord);
gint coord_real_y(gdouble y, CoordSystem *coord);

#endif /* _COORD_H */
