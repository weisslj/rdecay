#include <stdio.h>
#include "gtk/gtk.h"

static GdkPixmap *pixmap = NULL;

static gboolean delete_event(GtkWidget *widget, GdkEvent *event,
        gpointer data)
{
    gtk_main_quit();
    return FALSE;
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(G_OBJECT(window), "delete_event",
            G_CALLBACK(delete_event), NULL);

    pixmap = gdk_pixmap_new(window->window,
                            window->allocation.width,
                            window->allocation.height,
                            -1);
    gdk_draw_rectangle(pixmap,
                       window->style->white_gc,
                       TRUE,
                       0, 0,
                       window->allocation.width,
                       window->allocation.height);

    gdk_draw_rectangle(pixmap,
                       window->style->black_gc,
                       TRUE,
                       0, 0, 20, 20);

    gtk_widget_show(window);

    gtk_main();

    return 0;
}
