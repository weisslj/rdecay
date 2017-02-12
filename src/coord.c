/* 
 * coord.c - das Koordinatensystem
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

#include "coord.h"
#include "graph.h"
#include "util.h"

#include <gtk/gtk.h>
#include <stdarg.h>

enum { WIDTH, HEIGHT };

static gdouble gen_step_size(gdouble len, gboolean fract);

static gint get_number_size(gdouble n, gdouble fract, gint type,
                            PangoLayout *layout);
static gint get_text_size(gint type, PangoLayout *layout,
                          const gchar *format, ...);

static gint get_y_label_width(CoordSystem *coord, PangoLayout *layout);
static gint get_x_label_height(CoordSystem *coord, PangoLayout *layout);

/* erstellt ein neues Koordinatensystem */
CoordSystem *coord_system_new(GtkWidget *darea,
                              const gchar *x_title, const gchar *x_unit,
                              const gchar *y_title, const gchar *y_unit,
                              gboolean fract_x, gboolean fract_y,
                              gdouble min_x, gdouble max_x,
                              gdouble min_y, gdouble max_y)
{
    CoordSystem *coord;

    /* bricht ab, wenn der größte Wert niedriger als der
       Kleinste ist, oder umgekehrt */
    if (min_x >= max_x || min_y >= max_y)
        return NULL;

    coord = (CoordSystem *) g_malloc(sizeof(CoordSystem));

    coord->x_title = g_strdup(x_title);
    coord->y_title = g_strdup(y_title);

    coord->x_unit = g_strdup(x_unit);
    coord->y_unit = g_strdup(y_unit);

    coord->fract_x = fract_x;
    coord->fract_y = fract_y;

    coord->graphs = NULL;

    coord_system_adjust(coord, darea,
                        min_x, max_x, min_y, max_y);

    return coord;
}

/* stellt den Speicher eines erstellten Koordinatensystems
   wieder zur Verfügung */
void coord_system_free(CoordSystem *coord)
{
    g_free(coord->x_title);
    g_free(coord->y_title);
    g_free(coord->x_unit);
    g_free(coord->y_unit);
    coord_system_clear(coord);
    g_free(coord);
}

/* befreiht das Koordinatensystem von allen Graphen */
void coord_system_clear(CoordSystem *coord)
{
    GList *graph;

    if (coord->graphs == NULL)
        return;

    graph = g_list_first(coord->graphs);

    while (graph != NULL) {
        graph_free(graph->data);
        graph = graph->next;
    }

    g_list_free(coord->graphs);
    coord->graphs = NULL;
}

/* verankert einen Graphen im Koordinatensystem */
void coord_system_add_graph(CoordSystem *coord, Graph *gr)
{
    coord->graphs = g_list_append(coord->graphs, gr);
}

/* berechnet alle Werte, die man zum Zeichnen eines Koordinatensystems
   und von Graphen benötigt, anhand der Größe des Feldes, und den
   maximal erwarteten Werten */
void coord_system_adjust(CoordSystem *coord, GtkWidget *darea,
                         gdouble min_x, gdouble max_x,
                         gdouble min_y, gdouble max_y)
{
    GdkScreen *screen;
    PangoLayout *layout;
    gint x_axis, y_axis, cm_x, cm_y;

    /* bricht ab, wenn der größte Wert niedriger als der
       Kleinste ist, oder umgekehrt */
    if (min_x >= max_x || min_y >= max_y)
        return;

    layout = g_object_get_data(G_OBJECT(darea), "layout");

    /* rechnet einen Zentimeter in Pixel um */
    screen = gdk_screen_get_default();
    cm_x = (gint) ((gdouble) (10 * gdk_screen_get_width(screen)) /
                   (gdouble) gdk_screen_get_width_mm(screen) + 0.5);
    cm_y = (gint) ((gdouble) (10 * gdk_screen_get_height(screen)) /
                   (gdouble) gdk_screen_get_height_mm(screen) + 0.5);

    /* kopiert die Minimal- und Maximalwerte in die Struktur */
    coord->min_x = min_x;
    coord->max_x = max_x;
    coord->min_y = min_y;
    coord->max_y = max_y;

    /* berechnet die Beginn- und Endpunkte der Achsen */
    coord->x_axis_begin = get_y_label_width(coord, layout) + 15;
    coord->x_axis_end = darea->allocation.width - ARROW_SHANK -
                        get_text_size(WIDTH, layout, "%s [%s]",
                        coord->x_title, coord->x_unit) - 5;
    coord->y_axis_begin = darea->allocation.height -
                          get_x_label_height(coord, layout) - 15;
    coord->y_axis_end = ARROW_SHANK + get_text_size(HEIGHT, layout,
                        "%s [%s]", coord->y_title, coord->y_unit) + 10;

    /* berechnet die Länge der Achsen */
    x_axis = ABS(coord->x_axis_end - coord->x_axis_begin);
    y_axis = ABS(coord->y_axis_end - coord->y_axis_begin);

    /* berechnet den Faktor zum Umrechnen der Größen */
    coord->x_fact = (gdouble) x_axis / (gdouble) (max_x - min_x);
    coord->y_fact = (gdouble) y_axis / (gdouble) (max_y - min_y);

    /* errechnet die Schrittgröße der Achsen */
    coord->step_x = gen_step_size(2 * cm_x / coord->x_fact, coord->fract_x);
    coord->step_y = gen_step_size(2 * cm_y / coord->y_fact, coord->fract_y);

    /* berechnet die Koordinaten des Ursprungs */
    coord->zero_x = coord->x_axis_begin - ((gdouble) x_axis *
                    ((gdouble) min_x / (gdouble) (max_x - min_x)));
    coord->zero_y = coord->y_axis_end + ((gdouble) y_axis *
                    ((gdouble) max_y / (gdouble) (max_y - min_y)));
}

/* rechnet den X-Wert eines Punktes im Koordinatensystems in einen
   echten auf dem Zeichenbereich um */
gint coord_real_x(gdouble x, CoordSystem *coord)
{
    return (gint) (coord->zero_x + x * coord->x_fact + 0.5);
}

/* siehe coord_real_x, nur Y-Wert statt X-Wert */
gint coord_real_y(gdouble y, CoordSystem *coord)
{
    return (gint) (coord->zero_y - y * coord->y_fact + 0.5);
}

/* berechnet die Schrittgröße der Achsennummerierung */
static gdouble gen_step_size(gdouble len, gboolean fract)
{
    gdouble x;

    if (len >= 1.0) {
        for (x = 1.0; (len / x) > 1.0; x *= 10.0)
            ;
    } else {
        if (!fract)
            return 1.0;
        for (x = 1.0; (len / x) < 0.1; x /= 10.0)
            ;
    }

    return closest_val(len, 4, x, x / 2.0, x / 5.0, x / 10.0);
}

/* berechnet die Größe von Zahlen auf dem Zeichenbereich */
static gint get_number_size(gdouble n, gdouble fract, gint type,
                            PangoLayout *layout)
{
    gchar *str;
    gint width, height;

    str = strdup_pretty_number(n, fract);

    pango_layout_set_text(layout, str, -1);
    pango_layout_get_pixel_size(layout, &width, &height);

    g_free(str);

    if (type == WIDTH)
        return width;
    else if (type == HEIGHT)
        return height;
    else
        return -1;
}

/* berechnet die Größe von Text auf dem Zeichenbereich */
static gint get_text_size(gint type, PangoLayout *layout,
                          const gchar *format, ...)
{
    gint width, height;
    va_list ap;

    va_start(ap, format);
    layout_vprintf(layout, format, ap);
    va_end(ap);

    pango_layout_get_pixel_size(layout, &width, &height);

    if (type == WIDTH)
        return width;
    else if (type == HEIGHT)
        return height;
    else
        return -1;
}

/* berechnet die Breite der Y-Achsen Beschriftung */
static gint get_y_label_width(CoordSystem *coord, PangoLayout *layout)
{
    gint pos, neg, title;

    pos = get_number_size(coord->max_y, coord->fract_y, WIDTH, layout);
    neg = get_number_size(coord->min_y, coord->fract_y, WIDTH, layout);
    title = get_text_size(WIDTH, layout, "%s [%s]",
                          coord->y_title, coord->y_unit);

    return max_n(3, pos, neg, title);
}

/* berechnet die Höhe der X-Achsen Beschriftung */
static gint get_x_label_height(CoordSystem *coord, PangoLayout *layout)
{
    gint pos, neg, title;

    pos = get_number_size(coord->max_x, coord->fract_x, HEIGHT, layout);
    neg = get_number_size(coord->min_x, coord->fract_x, HEIGHT, layout);
    layout_printf(layout, "%s [%s]", coord->x_title, coord->x_unit);
    title = get_text_size(HEIGHT, layout, "%s [%s]",
                          coord->x_title, coord->x_unit);

    return max_n(3, pos, neg, title);
}
