/* 
 * ui_graph.h - Toolkit-Spezifisches zum Graphen
 *
 * Copyright (C) 2004-2017 Johannes Weißl
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

#ifndef _UI_GRAPH_H
#define _UI_GRAPH_H

#include "graph.h"
#include "coord.h"

#include <gtk/gtk.h>

gboolean graph_rebuild(GtkWidget *darea);
gboolean graph_redraw(GtkWidget *darea);
void graph_update(GtkWidget *darea);
void coord_system_draw(GtkWidget *darea, CoordSystem *coord);
void graph_draw(Graph *gr, GtkWidget *darea, CoordSystem *coord);
void graph_draw_line(GtkWidget *darea, CoordSystem *coord,
                     gdouble x1, gdouble y1,
                     gdouble x2, gdouble y2,
                     GdkGC *style);
gboolean coord_draw_pos(GtkWidget *darea, GdkEventButton *event);
void coord_system_store(GtkWidget *widget, CoordSystem *coord);

#endif /* _UI_GRAPH_H */
