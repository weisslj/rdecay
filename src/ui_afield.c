/* 
 * ui_afield.c - Toolkit-Spezifisches zum Atomfeld
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

#include "ui_afield.h"
#include "afield.h"
#include "util.h"

/* passt das Atomfeld an eine veränderte Größe an */
gboolean afield_resize(GtkWidget *darea, GdkEventConfigure *event, AtomField *af)
{
    IGNORE(event);
    afield_arrange(af, darea->allocation.width, darea->allocation.height);
    afield_draw(darea, af);

    return FALSE;
}

/* zeichnet ein Atom auf das Atomfeld */
void draw_atom(GtkWidget *darea, AtomCoord *coord, gint wide)
{
    GtkWidget *top;
    GdkPixmap *pixmap;
    GdkGC **style;
    gint real_wide, wide_diff;

    /* holt die verschiedenen Styles */
    top = gtk_widget_get_toplevel(darea);
    style = (GdkGC **) g_object_get_data(G_OBJECT(top), "style");

    /* holt das pixmap vom Zeichenbereich */
    pixmap = g_object_get_data(G_OBJECT(darea), "pixmap");

    /* berechnet die echte Breite, und den Abstand, der zu den
       umliegenden Atomen eingehalten wird */
    real_wide = wide * 0.8;
    wide_diff = (wide - real_wide) / 2.0;

    /* zeichnet das Rechteck */
    gdk_draw_rectangle(pixmap,
                       style[coord->state],
                       TRUE,
                       coord->x + wide_diff, coord->y + wide_diff,
                       real_wide, real_wide);

    /* gibt den Bereich zum Zeichnen auf den Bildschirm frei */
    gtk_widget_queue_draw_area(darea,
                               coord->x + wide_diff, coord->y + wide_diff,
                               real_wide, real_wide);
}

/* zeichnet das Atomfeld in einen Zeichenbereich */
void afield_draw(GtkWidget *darea, AtomField *af)
{
    gint i;
    for (i = 0; i < af->number; i++)
        draw_atom(darea, (af->coords + i), af->wide);
}

