/* 
 * coord.c - das Koordinatensystem
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

#include "coord.h"

static gdouble gen_unit_system(gdouble min, gdouble max);

/* erstellt ein neues Koordinatensystem */
CoordSystem *coord_system_new(gint field_width, gint field_height,
                              gdouble min_x, gdouble max_x,
                              gdouble min_y, gdouble max_y)
{
    CoordSystem *coord;

    /* bricht ab, wenn der größte Wert niedriger als der
       Kleinste ist, oder umgekehrt */
    if (min_x >= max_x || min_y >= max_y)
        return NULL;

    coord = (CoordSystem *) g_malloc(sizeof(CoordSystem));

    coord_system_adjust(coord, field_width, field_height,
                        min_x, max_x, min_y, max_y);

    return coord;
}

/* stellt den Speicher eines erstellten Koordinatensystems
   wieder zur Verfügung */
void coord_system_free(CoordSystem *coord)
{
    g_free(coord);
}

/* berechnet alle Werte, die man zum Zeichnen eines Koordinatensystems
   und von Graphen benötigt, anhand der Größe des Feldes, und den
   maximal erwarteten Werten */
void coord_system_adjust(CoordSystem *coord,
                         gint field_width, gint field_height,
                         gdouble min_x, gdouble max_x,
                         gdouble min_y, gdouble max_y)
{
    gint x_axis, y_axis;

    /* bricht ab, wenn der größte Wert niedriger als der
       Kleinste ist, oder umgekehrt */
    if (min_x >= max_x || min_y >= max_y)
        return;

    /* die Achsen sollen nur 95% des Feldes ausfüllen */
    x_axis = 0.95 * field_width;
    y_axis = 0.95 * field_height;

    /* berechnet die Beginn- und Endpunkte der Achsen */
    coord->x_axis_begin = (field_width - x_axis) / 2.0;
    coord->x_axis_end = coord->x_axis_begin + x_axis;
    coord->y_axis_end = (field_height - y_axis) / 2.0;
    coord->y_axis_begin = coord->y_axis_end + y_axis;

    /* errechnet die Schrittgröße der Achsen */
    coord->step_x = gen_unit_system(min_x, max_x);
    coord->step_y = gen_unit_system(min_y, max_y);

    /* kopiert die Minimal- und Maximalwerte in die Struktur */
    coord->min_x = min_x;
    coord->max_x = max_x;
    coord->min_y = min_y;
    coord->max_y = max_y;

    /* berechnet den Faktor zum Umrechnen der Größen */
    coord->x_fact = (gdouble) x_axis / (gdouble) (max_x - min_x);
    coord->y_fact = (gdouble) y_axis / (gdouble) (max_y - min_y);

    /* berechnet die Koordinaten des Ursprungs */
    coord->zero_x = coord->x_axis_begin - ((gdouble) x_axis * ((gdouble) min_x / (gdouble) (max_x - min_x)));
    coord->zero_y = coord->y_axis_end + ((gdouble) y_axis * ((gdouble) max_y / (gdouble) (max_y - min_y)));
}

/* FIXME */
gint coord_real_x(gdouble x, CoordSystem *coord)
{
    return coord->zero_x + x * coord->x_fact;
}

/* FIXME */
gint coord_real_y(gdouble y, CoordSystem *coord)
{
    return coord->zero_y - y * coord->y_fact;
}

/* FIXME */
void coord_get_real(gdouble *x, gdouble *y, CoordSystem *coord)
{
    *x = coord->zero_x + *x * coord->x_fact;
    *y = coord->zero_y - *y * coord->y_fact;
}

/* berechnet die Schrittgröße der Achsennummerierung */
static gdouble gen_unit_system(gdouble min, gdouble max)
{
    gdouble x, len, step;

    if (min > 0)
        min = 0;

    if (max < 0)
        max = 0;

    len = MAX(max - 0, 0 - min);

    for (x = 1; (len / x) > 1.0; x *= 10)
        ;

    step = x / 10;

    for (x = 0; x < (max - 0); x += step)
        ;

    max = x - 0;

    for (x = 0; x < (0 - min); x += step)
        ;

    min = 0 - x;

    return step;
}
