#include <stdio.h>
#include "gtk/gtk.h"

int main(int argc, char *argv[])
{
    GtkWidget *window;
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    gtk_widget_show(window);

    gtk_main();

    return 0;
}
