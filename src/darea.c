/* 
 * darea.c - Zeichenbereiche
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

#include "darea.h"
#include "util.h"

/* erstellt einen neuen Zeichenbereich */
GtkWidget *darea_new(void)
{
    GtkWidget *darea;

    /* reserviert Speicher */
    darea = gtk_drawing_area_new();

    /* verbindet den Bereich mit einigen Callbackfunktionen */
    g_signal_connect(G_OBJECT(darea), "realize",
                     G_CALLBACK(darea_init), NULL);

    g_signal_connect(G_OBJECT(darea), "expose_event",
                     G_CALLBACK(darea_redraw), NULL);

    g_signal_connect(G_OBJECT(darea), "configure_event",
                     G_CALLBACK(darea_resize), NULL);

    return darea;
}

/* löscht einen Zeichenbereich, heißt, er wird mit weißer
   Farbe ausgefüllt */
void darea_clear(GtkWidget *darea)
{
    GdkPixmap *pixmap;

    /* holt das pixmap vom Zeichenbereich */
    pixmap = g_object_get_data(G_OBJECT(darea), "pixmap");

    /* zeichnet das weiße Rechteck */
    gdk_draw_rectangle(pixmap,
                       darea->style->white_gc,
                       TRUE,
                       0, 0,
                       darea->allocation.width,
                       darea->allocation.height);

    /* gibt den Bereich zum Zeichnen auf dem Bildschirm frei */
    gtk_widget_queue_draw_area(darea,
                               0, 0,
                               darea->allocation.width,
                               darea->allocation.height);
}

/* bereitet einen Zeichenbereich zum Zeichnen vor */
void darea_init(GtkWidget *darea, gpointer data)
{
    GdkPixmap *pixmap;

    IGNORE(data);

    /* erstellt ein "pixmap" (der Bereich, auf dem eigentlich
       gezeichnet wird */
    pixmap = gdk_pixmap_new(darea->window,
                            darea->allocation.width,
                            darea->allocation.height,
                            -1);

    /* löscht den Zeichenbereich */
    darea_clear(darea);

    /* speichert das pixmap im Zeichenbereich */
    g_object_set_data(G_OBJECT(darea), "pixmap", pixmap);
}

/* passt den Zeichenbereich an eine veränderte Größe an */
gboolean darea_resize(GtkWidget *darea, GdkEventConfigure *event, gpointer data)
{
    GdkPixmap *pixmap;

    IGNORE(event);
    IGNORE(data);

    /* gibt den Speicher des pixmaps frei */
    pixmap = g_object_get_data(G_OBJECT(darea), "pixmap");
    if (pixmap != NULL)
        g_object_unref(pixmap);

    /* erstellt ein neues pixmap mit den neuen Maßen */
    pixmap = gdk_pixmap_new(darea->window,
                            darea->allocation.width,
                            darea->allocation.height,
                            -1);

    /* speichert das pixmap im Zeichenbereich */
    g_object_set_data(G_OBJECT(darea), "pixmap", pixmap);

    /* löscht den Zeichenbereich */
    darea_clear(darea);

    return FALSE;
}

/* zeichnet den Inhalt des Zeichenbereichs neu auf dem Bildschirm */
gboolean darea_redraw(GtkWidget *darea, GdkEventExpose *event, gpointer data)
{
    GdkPixmap *pixmap;

    IGNORE(data);

    /* holt das pixmap vom Zeichenbereich */
    pixmap = g_object_get_data(G_OBJECT(darea), "pixmap");

    /* zeichnet den betroffenen Bereich neu */
    gdk_draw_drawable(darea->window,
                      darea->style->fg_gc[GTK_WIDGET_STATE(darea)],
                      pixmap,
                      event->area.x, event->area.y,
                      event->area.x, event->area.y,
                      event->area.width, event->area.height);

    return FALSE;
}
