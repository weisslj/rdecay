/* 
 * graph.h - der Graph
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

#ifndef _GRAPH_H
#define _GRAPH_H

#include <gtk/gtk.h>

typedef struct _Point Point;

struct _Point {
    gdouble x;
    gdouble y;
};


typedef struct _Graph Graph;

struct _Graph {
    gboolean active;
    GdkGC *style;
    GList *points;
};

typedef gdouble (*GraphFunc)(gdouble, gpointer);

Point *point_alloc(gdouble x, gdouble y);
Point point_new(gdouble x, gdouble y);
void point_free(Point *p);

Graph *graph_new(GdkGC *style, gboolean active, gdouble x, gdouble y);
Graph *graph_new_by_func(GraphFunc func, gpointer data,
                         GdkGC *style, gboolean active,
                         gdouble x_begin, gdouble x_end,
                         gdouble step);
Graph *graph_step_new_by_func(GraphFunc func, gpointer data,
                              GdkGC *style, gboolean active,
                              gdouble x_begin, gdouble x_end,
                              gdouble step);
void graph_add(Graph *gr, gdouble x, gdouble y);
void graph_toggle(GtkWidget *check, Graph *gr);
void graph_free(Graph *gr);

#endif /* _GRAPH_H */
