/* 
 * color.h - Erstellen von Farben
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

#ifndef _COLOR_H
#define _COLOR_H

#include <gtk/gtk.h>

GdkColor color_new(GdkColormap *colormap,
                   guint8 red, guint8 green, guint8 blue);
GdkColor *color_alloc(GdkColormap *colormap,
                      guint8 red, guint8 green, guint8 blue);

#endif /* _COLOR_H */
