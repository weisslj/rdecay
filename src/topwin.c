#include <gtk/gtk.h>
#include <gsl/gsl_rng.h>

#include "topwin.h"
#include "input.h"
#include "status.h"
#include "ui_afield.h"
#include "ui_graph.h"

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

static gboolean delete_event(void)
{
    gtk_main_quit();
    return FALSE;
}

static void resize(GtkWidget *window)
{
    GtkWidget *d1, *d2;
    gint width, height;
    static gint old_width = 0;
    static gint old_height = 0;
    static gint i = 1;

    d1 = g_object_get_data(G_OBJECT(window), "atom_field");
    d2 = g_object_get_data(G_OBJECT(window), "graph");

    gtk_window_get_size(GTK_WINDOW(window), &width, &height);
    i++;

    if (old_width == width && old_height == height)
        return;

    return;

    old_width = width;
    old_height = height;

    gtk_widget_set_size_request(d1, width * 0.7, height * 0.7);
    gtk_widget_set_size_request(d2, width * 0.3, height * 0.4);
/*    gtk_widget_set_size_request(d2, i, i); */
    printf("%d. size-allocate (width: %d, height: %d)\n", i, width, height);
}


GtkWidget *create_top_window(gsl_rng *rand)
{
    GdkScreen *screen;
    gint scr_width, scr_height, width, height;
    GtkWidget *window, *box1, *box2, *box3, *box4;

    screen = gdk_screen_get_default();

    scr_width = gdk_screen_get_width(screen);
    scr_height = gdk_screen_get_height(screen);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), scr_width * 0.85, scr_height * 0.85);
    gtk_widget_set_name(window, _("simulation of radioactive decay"));
    gtk_container_set_border_width(GTK_CONTAINER(window), 5);

    g_signal_connect(G_OBJECT(window),
                     "delete_event",
                     G_CALLBACK(delete_event),
                     NULL);

    g_signal_connect(G_OBJECT(window),
                     "destroy",
                     G_CALLBACK(gtk_main_quit),
                     NULL);

    gtk_window_get_size(GTK_WINDOW(window), &width, &height);

    box1 = gtk_vbox_new(FALSE, 20);
    gtk_container_add(GTK_CONTAINER(window), box1);

    box2 = gtk_hbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box1), box2, FALSE, FALSE, 20);

    create_atom_field(box2, width * 0.7, height * 0.7);

    box3 = gtk_vbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box2), box3, FALSE, FALSE, 20);

    create_graph(box3, width * 0.3, height * 0.4);

    g_signal_connect(G_OBJECT(window),
                     "size-allocate",
                     G_CALLBACK(resize),
                     NULL);

    create_status_field(box3);

    box4 = gtk_hbox_new(TRUE, 20);
    gtk_box_pack_start(GTK_BOX(box1), box4, FALSE, FALSE, 0);

    create_input_fields(box4);
    create_ctrl_buttons(box4, rand);

    gtk_widget_show(box4);
    gtk_widget_show(box3);
    gtk_widget_show(box2);
    gtk_widget_show(box1);
    gtk_widget_show(window);

    return window;
}
