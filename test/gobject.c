#include <stdio.h>
#include "gtk/gtk.h"

static gboolean delete_event(GtkWidget *widget, GdkEvent *event,
        gpointer data)
{
    gtk_main_quit();
    return FALSE;
}

static gpointer create_buttons(GtkWidget *cont)
{
    GObject *obj;
    GtkWidget *b1, *b2, *box;

    b1 = gtk_button_new_with_label("Hello World");
    b2 = gtk_button_new_with_label("bla");

    gtk_widget_show(b1);
    gtk_widget_show(b2);

    box = gtk_hbox_new(FALSE, 10);

    gtk_box_pack_start(GTK_BOX(box), b1, TRUE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(box), b2, TRUE, TRUE, 10);

    gtk_widget_show(box);

    gtk_container_add(GTK_CONTAINER(cont), box);

    obj = g_object_new(G_TYPE_OBJECT, NULL);
    g_object_set_data(obj, "b1", b1);
    g_object_set_data(obj, "b2", b2);

    return obj;
}

int main(int argc, char *argv[])
{
    GObject *bt;
    GtkWidget *button, *window;
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(G_OBJECT(window), "delete_event",
            G_CALLBACK(delete_event), NULL);

    bt = create_buttons(window);

    gtk_widget_show(window);

    button = g_object_get_data(bt, "b2");
    g_signal_connect(G_OBJECT(button), "clicked",
            G_CALLBACK(gtk_main_quit), NULL);

    gtk_main();

    return 0;
}
