#ifndef _GRAPH_H
#define _GRAPH_H

#include <glib.h>

#include "coord.h"
#include "sim.h"

typedef struct _Point Point;

struct _Point {
    gdouble x;
    gdouble y;
};


typedef struct _Graph Graph;

struct _Graph {
    const gchar *style;
    GList *points;
};


typedef struct _GraphFunc GraphFunc;

struct _GraphFunc {
    gdouble (*func)(gdouble, gpointer);
    gpointer data;
};

Point *point_alloc(gdouble x, gdouble y);

Graph *graph_new(const gchar *style);
void graph_add(Graph *gr, Point *p);
void graph_free(Graph *gr);

void graph_draw_func(GraphFunc *gf, GtkWidget *darea, CoordSystem *coord);

#endif /* _GRAPH_H */
