#include <stdio.h>
#include <gtk/gtk.h>

#include "coord.h"
#include "graph.h"
#include "ui_graph.h"

Point *point_alloc(gdouble x, gdouble y)
{
    Point *p;

    p = g_malloc(sizeof(Point));
    p->x = x;
    p->y = y;

    return p;
}

Point point_new(gdouble x, gdouble y)
{
    Point p;

    p.x = x;
    p.y = y;

    return p;
}


Graph *graph_new(const gchar *style)
{
    Graph *gr;

    gr = (Graph *) g_malloc(sizeof(Graph));

    gr->style = style;
    gr->points = NULL;

    return gr;
}

void graph_draw_func(GraphFunc *gf, GtkWidget *darea, CoordSystem *coord)
{
    gdouble step, x, y, x_old, y_old;
    
    step = 1 / coord->x_fact;

    x_old = coord->min_x;
    y_old = gf->func(x_old, gf->data);

    for (x = coord->min_x; x <= coord->max_x; x += step) {
        y = gf->func(x, gf->data);
        if (x != coord->min_x)
            graph_draw_line(darea, coord,
                            x_old, y_old,
                            x, y,
                            "style2");
        x_old = x;
        y_old = y;
    }
}

void graph_add(Graph *gr, Point *p)
{
    gr->points = g_list_append(gr->points, p);
    gr->points = g_list_last(gr->points);
}
    

void graph_free(Graph *gr)
{
    g_list_free(gr->points);
    g_free(gr);
}
