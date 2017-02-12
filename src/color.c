/* 
 * color.c - Erstellen von Farben
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

#include "color.h"

#include <gtk/gtk.h>

#define RGB (65535.0 / 255.0)

/* erstellt eine neue Farbe aus drei Farbwerten */
GdkColor color_new(GdkColormap *colormap,
                   guint8 red, guint8 green, guint8 blue)
{
    GdkColor color;

    color.red = red * RGB;
    color.green = green * RGB;
    color.blue = blue * RGB;

    gdk_colormap_alloc_color(colormap, &color, FALSE, TRUE);

    return color;
}

/* reserviert Speicher und erstellt eine neue Farbe */
GdkColor *color_alloc(GdkColormap *colormap,
                      guint8 red, guint8 green, guint8 blue)
{
    GdkColor *color;

    color = (GdkColor *) g_malloc(sizeof(GdkColor));

    color->red = red * RGB;
    color->green = green * RGB;
    color->blue = blue * RGB;

    gdk_colormap_alloc_color(colormap, color, FALSE, TRUE);

    return color;
}
