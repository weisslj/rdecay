/* 
 * ui_afield.c - Toolkit-Spezifisches zum Atomfeld
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

#include "ui_afield.h"
#include "darea.h"
#include "afield.h"
#include "timer.h"
#include "util.h"

#include <gtk/gtk.h>

/* passt das Atomfeld an eine veränderte Größe an */
gboolean afield_resize(GtkWidget *darea,
                       GdkEventConfigure *event, AtomField *af)
{
    IGNORE(event);

    if (!af->uniform)
        afield_arrange(af, darea);
    if (!af->uniform)
        afield_draw(darea, af);

    return FALSE;
}

/* zeichnet ein Atom auf das Atomfeld */
void afield_draw_atom(GtkWidget *darea, AtomField *af, gulong n)
{
    GtkWidget *top;
    GdkPixmap *pixmap;
    GdkGC **style;

    /* holt die verschiedenen Styles */
    top = gtk_widget_get_toplevel(darea);
    style = g_object_get_data(G_OBJECT(top), "style_atom");

    /* holt das pixmap vom Zeichenbereich */
    pixmap = g_object_get_data(G_OBJECT(darea), "pixmap");

    /* zeichnet das Rechteck */
    gdk_draw_rectangle(pixmap,
                       style[(af->atoms + n)->state],
                       TRUE,
                       (af->atoms + n)->x, (af->atoms + n)->y,
                       af->wide, af->wide);
}

void afield_tint(GtkWidget *darea, AtomField *af,
                 gulong *atoms, gint states)
{
    GtkWidget *top;
    GdkPixmap *pixmap;
    GdkColor **color, *new_color;
    GdkColormap *colormap;
    gdouble percent;
    gint i;

    /* holt das pixmap vom Zeichenbereich */
    top = gtk_widget_get_toplevel(darea);
    color = g_object_get_data(G_OBJECT(top), "color_atom");
    pixmap = g_object_get_data(G_OBJECT(darea), "pixmap");

    new_color = (GdkColor *) g_malloc(sizeof(GdkColor));
    new_color->red = new_color->green = new_color->blue = 0;

    /* Mischt die Farben */
    for (i = 0; i < states; i++) {
        percent = atoms[i] / (gdouble) af->number;
        new_color->red += (color[i])->red * percent;
        new_color->green += (color[i])->green * percent;
        new_color->blue += (color[i])->blue * percent;
    }

    colormap = gdk_colormap_get_system();
    gdk_colormap_alloc_color(colormap, new_color, FALSE, TRUE);

    gdk_gc_set_foreground(af->ustyle, new_color);

    gdk_draw_rectangle(pixmap,
                       af->ustyle,
                       TRUE,
                       0, 0,
                       darea->allocation.width,
                       darea->allocation.height);

    g_free(new_color);
}

/* verteilt die Atome auf dem Atomfeld */
void afield_distrib_decays(GtkWidget *darea, AtomField *af,
                           gulong decays, gint state)
{
    gulong i, hit;

    for (i = 0; i < decays; i++) {
        hit = af->mask[af->pos[state]++];
        (af->atoms + hit)->state++;
        afield_draw_atom(darea, af, hit);
    }
}

/* zeichnet das Atomfeld in einen Zeichenbereich */
void afield_draw(GtkWidget *darea, AtomField *af)
{
    gulong i;
    for (i = 0; i < af->number; i++)
        afield_draw_atom(darea, af, i);
}

/* testet, ob der Computer schnell genug für das
   Atomfeld ist */
gboolean afield_benchmark(GtkWidget *darea, AtomField *af, gdouble max_t)
{
    MyTimer *timer;
    gulong i;

    timer = timer_new(1.0);

    for (i = 0; i < af->number; i++) {
        if (timer_elapsed(timer) > max_t) {
            timer_destroy(timer);
            return FALSE;
        }
        afield_draw_atom(darea, af, i);
    }

    timer_destroy(timer);

    return TRUE;
}
