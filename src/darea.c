#include <gtk/gtk.h>

#include "darea.h"

static gboolean redraw(GtkWidget *widget,
                       GdkEventExpose *event)
{
    GdkPixmap *pixmap;

    pixmap = g_object_get_data(G_OBJECT(widget), "pixmap");

    gdk_draw_drawable(widget->window,
                      widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
                      pixmap,
                      event->area.x, event->area.y,
                      event->area.x, event->area.y,
                      event->area.width, event->area.height);

    return FALSE;
}

GtkWidget *darea_new(GtkWidget *parent_box, const gchar *name,
                     gint width, gint height,
                     GCallback init, GCallback resize)
{
    GtkWidget *top, *darea;

    top = gtk_widget_get_toplevel(parent_box);

    darea = gtk_drawing_area_new();
    gtk_widget_set_size_request(darea, width, height);

    g_signal_connect(G_OBJECT(darea), "realize",
                     G_CALLBACK(init), NULL);

    g_signal_connect(G_OBJECT(darea), "expose_event",
                     G_CALLBACK(redraw), NULL);

    g_signal_connect(G_OBJECT(darea), "configure_event",
                     G_CALLBACK(resize), NULL);

    gtk_box_pack_start(GTK_BOX(parent_box), darea, FALSE, FALSE, 20);

    g_object_set_data(G_OBJECT(top), name, darea);

    gtk_widget_show(darea);

    return darea;
}



void darea_clear(GtkWidget *darea)
{
    GdkPixmap *pixmap;

    pixmap = g_object_get_data(G_OBJECT(darea), "pixmap");

    gdk_draw_rectangle(pixmap,
                       darea->style->white_gc,
                       TRUE,
                       0, 0,
                       darea->allocation.width,
                       darea->allocation.height);

    gtk_widget_queue_draw_area(darea,
                               0, 0,
                               darea->allocation.width,
                               darea->allocation.height);
}
