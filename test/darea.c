#include <stdio.h>
#include "gtk/gtk.h"

static GdkPixmap *pixmap = NULL;

static gboolean configure_event( GtkWidget         *widget,
                                 GdkEventConfigure *event )
{
  if (pixmap)
    g_object_unref (pixmap);

  pixmap = gdk_pixmap_new (widget->window,
               widget->allocation.width,
               widget->allocation.height,
               -1);
  gdk_draw_rectangle (pixmap,
              widget->style->white_gc,
              TRUE,
              0, 0,
              widget->allocation.width,
              widget->allocation.height);
    gdk_draw_rectangle(pixmap, widget->style->black_gc, TRUE,
            0, 0, 20, 20);

  return TRUE;
}

static gboolean expose_event( GtkWidget      *widget,
                              GdkEventExpose *event )
{   
  gdk_draw_drawable (widget->window,
             widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
             pixmap,
             event->area.x, event->area.y,
             event->area.x, event->area.y,
             event->area.width, event->area.height);

  return FALSE;
}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event,
        gpointer data)
{
    gtk_main_quit();
    return FALSE;
}

int main(int argc, char *argv[])
{
    GtkWidget *window, *darea;
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(G_OBJECT(window), "delete_event",
            G_CALLBACK(delete_event), NULL);

    darea = gtk_drawing_area_new();

    g_signal_connect (G_OBJECT (darea), "expose_event",
            G_CALLBACK (expose_event), NULL);
    g_signal_connect (G_OBJECT (darea),"configure_event",
            G_CALLBACK (configure_event), NULL);


    gtk_container_add(GTK_CONTAINER(window), darea);

    gtk_widget_show(darea);
    gtk_widget_show(window);

    gtk_main();

    return 0;
}
