/* 
 * graph.c - der Graph
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

#include <glib.h>

#include "graph.h"
#include "coord.h"

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

/* erstellt einen neuen Graphen */
Graph *graph_new(gint style)
{
    Graph *gr;

    gr = (Graph *) g_malloc(sizeof(Graph));

    gr->style = style;
    gr->points = NULL;

    return gr;
}

/* erstellt einen neuen Graphen anhand der Funktion "gf" */
Graph *graph_new_by_func(GraphFunc *gf, gint style, CoordSystem *coord)
{
    Graph *gr;
    Point *p;
    gdouble step, x, y;

    gr = graph_new(style);

    /* die Schrittgröße ist ein Pixel */
    step = 1 / coord->x_fact;

    /* trägt Schritt für Schritt die Koordinaten in die Liste ein */
    for (x = coord->min_x; x <= coord->max_x; x += step) {
        y = gf->func(x, gf->data);
        p = point_alloc(x, y);
        gr->points = g_list_append(gr->points, p);
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
    
/* stellt den Speicher des Graphen wieder zur Verfügung */
void graph_free(Graph *gr)
{
    g_list_free(gr->points);
    g_free(gr);
}
