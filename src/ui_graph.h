#ifndef _UI_GRAPH_H
#define _UI_GRAPH_H

#include <gtk/gtk.h>
#include "graph.h"

GtkWidget *create_graph(GtkWidget *parent_box, gdouble width, gdouble height);
void clear_graph(GtkWidget *graph);

void draw_graph_coord_system(GtkWidget *graph);
void draw_graph_line(GtkWidget *graph, Point p_begin, Point p_end, const gchar *style);

#endif /* _UI_GRAPH_H */
