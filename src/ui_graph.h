#ifndef _UI_GRAPH_H
#define _UI_GRAPH_H

#include <gtk/gtk.h>
#include "graph.h"

GtkWidget *create_graph(GtkWidget *parent_box, gdouble width, gdouble height);

void coord_system_draw(GtkWidget *graph, CoordSystem *coord);
void graph_draw_line(GtkWidget *darea, CoordSystem *coord,
                     gdouble x1, gdouble y1,
                     gdouble x2, gdouble y2,
                     const gchar *style_name);

#endif /* _UI_GRAPH_H */
