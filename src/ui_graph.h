/* 
 * ui_graph.h - Toolkit-Spezifisches zum Graphen
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

#ifndef _UI_GRAPH_H
#define _UI_GRAPH_H

#include <gtk/gtk.h>

#include "graph.h"
#include "coord.h"

gboolean graph_resize(GtkWidget *darea, GdkEventConfigure *event, CoordSystem *coord);
void coord_system_draw(GtkWidget *darea, CoordSystem *coord);
void graph_draw(Graph *gr, GtkWidget *darea, CoordSystem *coord);
void graph_draw_func(GraphFunc *gf, GtkWidget *darea, CoordSystem *coord);
void graph_draw_line(GtkWidget *darea, CoordSystem *coord,
                     gdouble x1, gdouble y1,
                     gdouble x2, gdouble y2,
                     gint n_style);

#endif /* _UI_GRAPH_H */
