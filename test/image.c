#include <stdio.h>
#include "gtk/gtk.h"

static gboolean delete_event(GtkWidget *widget, GdkEvent *event,
        gpointer data)
{
    gtk_main_quit();
    return FALSE;
}

int main(int argc, char *argv[])
{
    GtkWidget *window, *image;
    GdkPixmap *pixmap;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(G_OBJECT(window), "delete_event",
            G_CALLBACK(delete_event), NULL);

    image = gtk_image_new();
    gtk_container_add(GTK_CONTAINER(window), image);

    gtk_widget_show(window);

    pixmap = gdk_pixmap_new(window->window,
                            600,
                            600,
                            -1);

    gtk_image_set_from_pixmap(GTK_IMAGE(image), pixmap, NULL);

/*    gdk_draw_rectangle(pixmap,
                       window->style->white_gc,
                       TRUE,
                       0, 0,
                       window->allocation.width,
                       window->allocation.height);*/

    gdk_draw_rectangle(pixmap,
                       window->style->white_gc,
                       TRUE,
                       0, 0,
                       200,
                       200);

    gdk_draw_rectangle(pixmap,
                       window->style->black_gc,
                       TRUE,
                       0, 0, 20, 20);

    gtk_widget_show(image);
    gtk_widget_queue_draw(image);

    gtk_main();

    return 0;
}
