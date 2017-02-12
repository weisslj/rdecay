/* 
 * ui_graph.c - Toolkit-Spezifisches zum Graphen
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
 * along with rdecay; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ui_graph.h"
#include "graph.h"
#include "darea.h"
#include "coord.h"
#include "util.h"

#include <gtk/gtk.h>
#include <math.h>

#define CHECK_BORDER(d, max, min, fact, darea) \
        if (d > max) { \
            max += dmax(1 / fact, d - max); \
            graph_rebuild(darea); \
        } else if (d < min) { \
            min -= dmax(1 / fact, min - d); \
            graph_rebuild(darea); \
        }

/* baut den Graphen neu zusammen */
gboolean graph_rebuild(GtkWidget *darea)
{
    CoordSystem *coord;
    GList *graph;

    /* holt das Koordinatensystem vom Zeichenbereich */
    coord = g_object_get_data(G_OBJECT(darea), "coord");
    if (coord == NULL)
        return FALSE;

    /* passt das Koordinatensystem an */
    coord_system_adjust(coord, darea,
                        coord->min_x, coord->max_x,
                        coord->min_y, coord->max_y);

    /* löscht den Zeichenbereich */
    darea_clear(darea);

    /* zeichnet das Koordinatensystem */
    coord_system_draw(darea, coord);

    /* springt zum ersten Graphen */
    graph = g_list_first(coord->graphs);

    /* passt nach einander jeden Graphen an */
    while (graph != NULL) {
        if (((Graph *) graph->data)->active)
            graph_draw(graph->data, darea, coord);
        graph = graph->next;
    }

    return FALSE;
}

/* zeichnet den Graphen neu */
gboolean graph_redraw(GtkWidget *darea)
{
    CoordSystem *coord;
    GList *graph;

    /* holt das Koordinatensystem vom Zeichenbereich */
    coord = g_object_get_data(G_OBJECT(darea), "coord");
    if (coord == NULL)
        return FALSE;

    /* löscht den Zeichenbereich */
    darea_clear(darea);

    /* zeichnet das Koordinatensystem */
    coord_system_draw(darea, coord);

    /* springt zum ersten Graphen */
    graph = g_list_first(coord->graphs);

    /* zeichnet die Graphen */
    while (graph != NULL) {
        if (((Graph *) graph->data)->active)
            graph_draw(graph->data, darea, coord);
        graph = graph->next;
    }

    return FALSE;
}

/* zeichnet die Graphen weiter */
void graph_update(GtkWidget *darea)
{
    CoordSystem *coord;
    GList *graph;
    Graph *gr;

    /* holt das Koordinatensystem vom Zeichenbereich */
    coord = g_object_get_data(G_OBJECT(darea), "coord");
    if (coord == NULL)
        return;

    /* springt zum ersten Graphen */
    graph = g_list_first(coord->graphs);

    /* aktualisiert die aktivierten Graphen */
    while (graph != NULL) {
        gr = (Graph *) graph->data;
        if (gr->active) {
            gr->points = g_list_last(gr->points);
            graph_draw_line(darea, coord,
                            ((Point *) gr->points->prev->data)->x,
                            ((Point *) gr->points->prev->data)->y,
                            ((Point *) gr->points->data)->x,
                            ((Point *) gr->points->data)->y,
                            gr->style);
        }
        graph = graph->next;
    }
}

/* zeichnet das Koordinatensystem auf einen Zeichenbereich */
void coord_system_draw(GtkWidget *darea, CoordSystem *coord)
{
    GtkWidget *top;
    gdouble d;
    GdkGC *style_grid;
    gint width, height, rd;
    PangoLayout *layout;
    GdkPixmap *pixmap;

    top = gtk_widget_get_toplevel(darea);
    pixmap = g_object_get_data(G_OBJECT(darea), "pixmap");
    layout = g_object_get_data(G_OBJECT(darea), "layout");
    style_grid = g_object_get_data(G_OBJECT(top), "style_grid");

    /* zeichnet die X-Achse */
    gdk_draw_line(pixmap,
                  darea->style->black_gc,
                  coord->x_axis_begin,
                  coord->zero_y,
                  coord->x_axis_end + ARROW_SHANK,
                  coord->zero_y);

    /* zeichnet den Pfeil am Ende der X-Achse */
    gdk_draw_line(pixmap,
                  darea->style->black_gc,
                  coord->x_axis_end + ARROW_SHANK - ARROW_HEIGHT,
                  coord->zero_y - (ARROW_WIDTH / 2),
                  coord->x_axis_end + ARROW_SHANK,
                  coord->zero_y);

    gdk_draw_line(pixmap,
                  darea->style->black_gc,
                  coord->x_axis_end + ARROW_SHANK - ARROW_HEIGHT,
                  coord->zero_y + (ARROW_WIDTH / 2),
                  coord->x_axis_end + ARROW_SHANK,
                  coord->zero_y);

    /* zeichnet X-Achsen Benennung */
    if (coord->x_unit[0] != '\0')
        layout_printf(layout, "%s [%s]", coord->x_title, coord->x_unit);
    else
        layout_printf(layout, "%s", coord->x_title);
    gdk_draw_layout(pixmap, darea->style->black_gc, coord->x_axis_end +
                    ARROW_SHANK, coord->zero_y + 10, layout);

    /* zeichnet die Y-Ache */
    gdk_draw_line(pixmap,
                  darea->style->black_gc,
                  coord->zero_x,
                  coord->y_axis_begin,
                  coord->zero_x,
                  coord->y_axis_end - ARROW_SHANK);

    /* zeichnet den Pfeil am Ende der Y-Ache */
    gdk_draw_line(pixmap,
                  darea->style->black_gc,
                  coord->zero_x - (ARROW_WIDTH / 2),
                  coord->y_axis_end - ARROW_SHANK + ARROW_HEIGHT,
                  coord->zero_x,
                  coord->y_axis_end - ARROW_SHANK);

    gdk_draw_line(pixmap,
                  darea->style->black_gc,
                  coord->zero_x + (ARROW_WIDTH / 2),
                  coord->y_axis_end - ARROW_SHANK + ARROW_HEIGHT,
                  coord->zero_x,
                  coord->y_axis_end - ARROW_SHANK);

    /* zeichnet Y-Achsen Benennung */
    if (coord->y_unit[0] != '\0')
        layout_printf(layout, "%s [%s]", coord->y_title, coord->y_unit);
    else
        layout_printf(layout, "%s", coord->y_title);
    pango_layout_get_pixel_size(layout, &width, &height);
    gdk_draw_layout(pixmap, darea->style->black_gc,
                            coord->zero_x - width - 10,
                            coord->y_axis_end - height - ARROW_SHANK,
                            layout);

    /* zeichnet Grid und Einheiten der X-Achse */
    for (d = coord->min_x; d <= coord->max_x; d += coord->step_x) {
        rd = coord_real_x(d, coord);
        if (rd != coord->zero_x) {
            gdk_draw_line(pixmap,
                          style_grid,
                          rd,
                          coord_real_y(coord->min_y, coord),
                          rd,
                          coord_real_y(coord->max_y, coord));
            gdk_draw_line(pixmap,
                          darea->style->black_gc,
                          rd,
                          coord->zero_y - 2,
                          rd,
                          coord->zero_y + 2);
        }
        layout_printf(layout, "%g", d);
        pango_layout_get_pixel_size(layout, &width, &height);
        gdk_draw_layout(pixmap, darea->style->black_gc, rd -
                        (width / 2.0), coord->zero_y + 10, layout);

    }

    /* zeichnet Grid und Einheiten der Y-Achse */
    for (d = coord->min_y; d <= coord->max_y; d += coord->step_y) {
        rd = coord_real_y(d, coord);
        if (rd != coord->zero_y) {
            gdk_draw_line(pixmap,
                          style_grid,
                          coord_real_x(coord->min_x, coord),
                          rd,
                          coord_real_x(coord->max_x, coord),
                          rd);
            gdk_draw_line(pixmap,
                          darea->style->black_gc,
                          coord->zero_x - 2,
                          rd,
                          coord->zero_x + 2,
                          rd);
        }
        layout_printf(layout, "%g", d);
        pango_layout_get_pixel_size(layout, &width, &height);
        gdk_draw_layout(pixmap, darea->style->black_gc, coord->zero_x -
                        10 - width, rd - (height / 2.0), layout);
    }

    /* gibt den Bereich zum Zeichnen auf dem Bildschirm frei */
    darea_update(darea);
}

/* schreibt die Position des Mauszeigers im
   Koordinatensystem auf den Zeichenbereich */
gboolean coord_draw_pos(GtkWidget *darea, GdkEventButton *event)
{
    GdkPixmap *pixmap;
    CoordSystem *coord;
    PangoLayout *layout;
    gdouble x, y;
    gchar *str_x, *str_y, *str;
    gint width, height, a, b, w, h;

    /* holt das Koordinatensystem vom Zeichenbereich */
    coord = g_object_get_data(G_OBJECT(darea), "coord");
    if (coord == NULL)
        return FALSE;

    x = (event->x - coord->zero_x) / coord->x_fact;
    y = (coord->zero_y - event->y) / coord->y_fact;

    if (x < coord->min_x || x > coord->max_x ||
            y < coord->min_y || y > coord->max_y)
        return FALSE;

    pixmap = g_object_get_data(G_OBJECT(darea), "pixmap");
    layout = g_object_get_data(G_OBJECT(darea), "layout");

    str_x = strdup_pretty_number(x, coord->fract_x);
    str_y = strdup_pretty_number(y, coord->fract_y);

    str = g_strconcat(str_x, " | ", str_y, NULL);
    pango_layout_set_text(layout, str, -1);

    pango_layout_get_pixel_size(layout, &width, &height);

    a = coord->zero_x + 20;
    b = coord->y_axis_end - ARROW_SHANK - height;
    w = darea->allocation.width - a;
    h = b + height;

    gdk_draw_rectangle(pixmap,
                       darea->style->white_gc,
                       TRUE,
                       a, b, w, h);
    gdk_draw_layout(pixmap, darea->style->black_gc, a, b, layout);
    gtk_widget_queue_draw_area(darea, a, b, w, h);

    g_free(str_x);
    g_free(str_y);
    g_free(str);

    return FALSE;
}

/* FIXME FIXME FIXME FIXME */
void coord_system_store(GtkWidget *widget, CoordSystem *coord)
{
    GtkWidget *top, **darea;

    if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
        return;

    top = gtk_widget_get_toplevel(widget);
    darea = g_object_get_data(G_OBJECT(top), "darea");

    g_object_set_data(G_OBJECT(darea[1]), "coord", coord);
    graph_redraw(darea[1]);
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
                     GdkGC *style)
{
    GdkPixmap *pixmap;
    gint a, b, c, d;

    /* holt das pixmap vom Zeichenbereich */
    pixmap = g_object_get_data(G_OBJECT(darea), "pixmap");

    /* prüft, ob die Linie über die Grenzen des Koordinatensystems
       stoßen würde, und passt es gegebenenfalls an */
    CHECK_BORDER(x2, coord->max_x, coord->min_x, coord->x_fact, darea)
    CHECK_BORDER(y2, coord->max_y, coord->min_y, coord->y_fact, darea)

    /* rechnet die Koordinaten um */
    a = coord_real_x(x1, coord);
    b = coord_real_y(y1, coord);
    c = coord_real_x(x2, coord);
    d = coord_real_y(y2, coord);

    /* zeichnet eine Linie von P1 nach P2 */
    gdk_draw_line(pixmap, style, a, b, c, d);
}
