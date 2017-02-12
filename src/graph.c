/* 
 * graph.c - der Graph
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
 * along with rdecay.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "graph.h"
#include "util.h"

#include <gtk/gtk.h>

/* reserviert Speicher, erstellt neuen Punkt
   und gibt seine Adresse zurück */
Point *point_alloc(gdouble x, gdouble y)
{
    Point *p;

    p = (Point *) g_malloc(sizeof(Point));
    p->x = x;
    p->y = y;

    return p;
}

/* erstellt einen neuen Punkt */
Point point_new(gdouble x, gdouble y)
{
    Point p;

    p.x = x;
    p.y = y;

    return p;
}

/* zerstört einen Punkt */
void point_free(Point *p)
{
    g_free(p);
}

/* erstellt einen neuen Graphen des Stils style mit den
   Anfangskoordinaten x und y */
Graph *graph_new(GdkGC *style, gboolean active, gdouble x, gdouble y)
{
    Graph *gr;

    gr = (Graph *) g_malloc(sizeof(Graph));

    gr->style = style;
    gr->active = active;
    gr->points = NULL;
    graph_add(gr, x, y);

    return gr;
}

/* erstellt einen neuen Graphen anhand der Funktion "gf" */
Graph *graph_new_by_func(GraphFunc func, gpointer data,
                         GdkGC *style, gboolean active,
                         gdouble x_begin, gdouble x_end,
                         gdouble step)
{
    Graph *gr;
    gdouble x, y;

    gr = (Graph *) g_malloc(sizeof(Graph));
    gr->style = style;
    gr->active = active;
    gr->points = NULL;

    /* trägt Schritt für Schritt die Koordinaten in die Liste ein */
    for (x = x_begin; x <= x_end; x += step) {
        y = func(x, data);
        graph_add(gr, x, y);
    }

    return gr;
}

/* erstellt einen neuen stufigen Graphen */
Graph *graph_step_new_by_func(GraphFunc func, gpointer data,
                              GdkGC *style, gboolean active,
                              gdouble x_begin, gdouble x_end,
                              gdouble step)
{
    Graph *gr;
    gdouble x, y_curr, y_prev, y_next;

    gr = (Graph *) g_malloc(sizeof(Graph));
    gr->style = style;
    gr->active = active;
    gr->points = NULL;

    y_curr = func(x_begin, data);
    y_next = func(x_begin + step, data);
    graph_add(gr, x_begin, y_curr);
    if (y_curr > y_next)
        graph_add(gr, x_begin, y_next);

    for (x = x_begin + step; x <= x_end; x += step) {
        y_prev = y_curr;
        y_curr = y_next;
        y_next = func(x + step, data);

        if (y_curr > y_next) {
            graph_add(gr, x, y_curr);
            graph_add(gr, x, y_next);
        } else {
            graph_add(gr, x, y_prev);
            graph_add(gr, x, y_curr);
        }
    }

    return gr;
}

/* fügt einen Punkt zum Graphen hinzu */
void graph_add(Graph *gr, gdouble x, gdouble y)
{
    GList *l;
    Point *p;

    p = point_alloc(x, y);

    l = g_list_append(gr->points, p);
    if (gr->points == NULL)
        gr->points = l;
    else
        gr->points = g_list_last(gr->points);
}

/* schaltet den Graphen an und aus */
void graph_toggle(GtkWidget *check, Graph *gr)
{
    IGNORE(check);
    gr->active = TOGGLE(gr->active);
}

/* stellt den Speicher des Graphen wieder zur Verfügung */
void graph_free(Graph *gr)
{
    GList *point;

    point = g_list_first(gr->points);

    while (point != NULL) {
        point_free(point->data);
        point = point->next;
    }

    g_list_free(gr->points);
    g_free(gr);
}
