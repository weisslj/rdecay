#include <stdio.h>
#include "gtk/gtk.h"

static gboolean delete_event(GtkWidget *widget, GdkEvent *event,
        gpointer data)
{
    gtk_main_quit();
    return FALSE;
}

static GtkWidget *create_buttons(GtkWidget *box)
{
    GtkWidget *top, *b1, *b2;

    top = gtk_widget_get_toplevel(box);

    b1 = gtk_button_new_with_label("Hello World");
    b2 = gtk_button_new_with_label("bla");

    gtk_box_pack_start(GTK_BOX(box), b1, TRUE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(box), b2, TRUE, TRUE, 10);

    g_object_set_data(G_OBJECT(top), "n0_time", b1);
    g_object_set_data(G_OBJECT(top), "b2", b2);
    if (g_object_get_data(G_OBJECT(top), "n0_time") == NULL)
        printf("1: NULL\n");
    else
        printf("1: ok\n");


    gtk_widget_show(b1);
    gtk_widget_show(b2);

    return box;
}

int main(int argc, char *argv[])
{
    GtkWidget *button, *window, *box;
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(G_OBJECT(window), "delete_event",
            G_CALLBACK(delete_event), NULL);

    box = gtk_hbox_new(FALSE, 10);

    gtk_container_add(GTK_CONTAINER(window), box);

    create_buttons(box);

    gtk_widget_show(window);

    button = g_object_get_data(G_OBJECT(window), "b2");
    g_signal_connect(G_OBJECT(button), "clicked",
            G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show(box);
    gtk_main();

    return 0;
}
