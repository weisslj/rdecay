/* 
 * ui_graph.c - Toolkit-Spezifisches zum Graphen
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

#include <gtk/gtk.h>

#include "ui_graph.h"
#include "graph.h"
#include "coord.h"
#include "util.h"

/* passt das Koordinatensystem und die Graphen an eine
   veränderte Größe an */
gboolean graph_resize(GtkWidget *darea, GdkEventConfigure *event, CoordSystem *coord)
{
    GList *graph;

    IGNORE(event);

    /* passt das Koordinatensystem an */
    coord_system_adjust(coord,
                        darea->allocation.width, darea->allocation.height,
                        coord->min_x, coord->max_x,
                        coord->min_y, coord->max_y);

    /* zeichnet das Koordinatensystem */
    coord_system_draw(darea, coord);

    /* springt zum ersten Graphen */
    graph = g_list_first(coord->graphs);

    /* passt nach einander jeden Graphen an */
    while (graph != NULL) {
        graph_draw(graph->data, darea, coord);
        graph = graph->next;
    }

    return FALSE;
}

/* zeichnet das Koordinatensystem auf einen Zeichenbereich */
void coord_system_draw(GtkWidget *darea, CoordSystem *coord)
{
    GdkPixmap *pixmap;

    /* holt das pixmap vom Zeichenbereich */
    pixmap = g_object_get_data(G_OBJECT(darea), "pixmap");

    /* zeichnet die X-Achse */
    gdk_draw_line(pixmap,
                  darea->style->black_gc,
                  coord->x_axis_begin,
                  coord->zero_y,
                  coord->x_axis_end,
                  coord->zero_y);

    /* zeichnet den Pfeil am Ende der X-Achse */
    gdk_draw_line(pixmap,
                  darea->style->black_gc,
                  coord->x_axis_end - 5,
                  coord->zero_y - 5,
                  coord->x_axis_end,
                  coord->zero_y);

    gdk_draw_line(pixmap,
                  darea->style->black_gc,
                  coord->x_axis_end - 5,
                  coord->zero_y + 5,
                  coord->x_axis_end,
                  coord->zero_y);


    /* zeichnet die Y-Ache */
    gdk_draw_line(pixmap,
                  darea->style->black_gc,
                  coord->zero_x,
                  coord->y_axis_begin,
                  coord->zero_x,
                  coord->y_axis_end);

    /* zeichnet den Pfeil am Ende der Y-Ache */
    gdk_draw_line(pixmap,
                  darea->style->black_gc,
                  coord->zero_x - 5,
                  coord->y_axis_end + 5,
                  coord->zero_x,
                  coord->y_axis_end);

    gdk_draw_line(pixmap,
                  darea->style->black_gc,
                  coord->zero_x + 5,
                  coord->y_axis_end + 5,
                  coord->zero_x,
                  coord->y_axis_end);

    /* gibt den Bereich zum Zeichnen auf dem Bildschirm frei */
    gtk_widget_queue_draw_area(darea,
                               coord->x_axis_begin,
                               coord->y_axis_begin,
                               coord->x_axis_end,
                               coord->y_axis_end);
}

/* zeichnet einen Graphen auf einen Zeichenbereich */
void graph_draw(Graph *gr, GtkWidget *darea, CoordSystem *coord)
{
    GList *point;

    /* Springt an den Anfang der Punktliste */
    point = g_list_first(gr->points);

    /* zeichnet die Punkte */
    while (point != NULL && point->next != NULL) {
        graph_draw_line(darea, coord,
                        ((Point *) point->data)->x,
                        ((Point *) point->data)->y,
                        ((Point *) point->next->data)->x,
                        ((Point *) point->next->data)->y,
                        gr->style);
        point = point->next;
    }
}

/* zeichnet eine Linie von einem zum anderen Punkt */
void graph_draw_line(GtkWidget *darea, CoordSystem *coord,
                     gdouble x1, gdouble y1,
                     gdouble x2, gdouble y2,
                     gint n_style)
{
    GtkWidget *top;
    GdkPixmap *pixmap;
    GdkGC **style;
    gint a, b, c, d;

    /* holt die verschiedenen Styles */
    top = gtk_widget_get_toplevel(darea);
    style = (GdkGC **) g_object_get_data(G_OBJECT(top), "style");

    /* holt das pixmap vom Zeichenbereich */
    pixmap = g_object_get_data(G_OBJECT(darea), "pixmap");

    /* rechnet die Koordinaten um */
    a = coord_real_x(x1, coord);
    b = coord_real_y(y1, coord);
    c = coord_real_x(x2, coord);
    d = coord_real_y(y2, coord);
    /* coord_get_real(&x1, &y1, coord);
    coord_get_real(&x2, &y2, coord); */

    /* zeichnet eine Linie von P1 nach P2 */
    gdk_draw_line(pixmap, style[n_style], a, b, c, d);

    /* gibt den Bereich zum Zeichnen auf den Bildschirm frei */
    gtk_widget_queue_draw_area(darea, a, b, c, d);
}

/* zeichnet eine Funktion "gf" auf den Zeichenbereich darea */
void graph_draw_func(GraphFunc *gf, GtkWidget *darea, CoordSystem *coord)
{
    gdouble step, x, y, x_old, y_old;

    /* die Schrittgröße ist ein Pixel */
    step = 1 / coord->x_fact;

    /* berechnet die Startwerte */
    x_old = coord->min_x;
    y_old = gf->func(x_old, gf->data);

    /* zeichnet Schritt für Schritt die Punkte */
    for (x = coord->min_x; x <= coord->max_x; x += step) {
        y = gf->func(x, gf->data);
        if (x != coord->min_x)
            graph_draw_line(darea, coord,
                            x_old, y_old,
                            x, y,
                            1);
        x_old = x;
        y_old = y;
    }
}

