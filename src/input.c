#include <gtk/gtk.h>

#include "input.h"
#include "sim.h"

GtkWidget *create_input_fields(GtkWidget *parent_box)
{
    GtkWidget *top, *box, *number_spin, *time_spin, *number_label, *time_label;
    GtkAdjustment *number_adj, *time_adj;

    top = gtk_widget_get_toplevel(parent_box);

    number_adj = (GtkAdjustment *)
                 gtk_adjustment_new(200.0, 1.0,
                                    10000.0, 1.0,
                                    10.0, 0);

    time_adj = (GtkAdjustment *)
               gtk_adjustment_new(10.0, 0.1,
                                  1000000.0, 0.1,
                                  10.0, 0);

    box = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(parent_box), box, FALSE, FALSE, 20);

    number_label = gtk_label_new("Atome: ");
    gtk_box_pack_start(GTK_BOX(box), number_label, FALSE, FALSE, 10);

    number_spin = gtk_spin_button_new(number_adj, 0.5, 0);
    gtk_box_pack_start(GTK_BOX(box), number_spin, FALSE, FALSE, 20);

    time_label = gtk_label_new("Halbwertszeit: ");
    gtk_box_pack_start(GTK_BOX(box), time_label, FALSE, FALSE, 10);

    time_spin = gtk_spin_button_new(time_adj, 0.5, 2);
    gtk_box_pack_start(GTK_BOX(box), time_spin, FALSE, FALSE, 20);

    g_object_set_data(G_OBJECT(top), "n0_input", number_spin);
    g_object_set_data(G_OBJECT(top), "htime_input", time_spin);

    gtk_widget_show(number_label);
    gtk_widget_show(number_spin);
    gtk_widget_show(time_label);
    gtk_widget_show(time_spin);
    gtk_widget_show(box);

    return box;
}

GtkWidget *create_ctrl_buttons(GtkWidget *parent_box)
{
    GtkWidget *top, *box, *bstart, *bstop, *bquit;

    box = gtk_hbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(parent_box), box, TRUE, TRUE, 20);

    bstart = gtk_button_new_with_label("Start");
    gtk_box_pack_start(GTK_BOX(box), bstart, TRUE, TRUE, 20);

    bstop = gtk_button_new_with_label("Stop");
    gtk_box_pack_start(GTK_BOX(box), bstop, TRUE, TRUE, 20);

    bquit = gtk_button_new_with_label("Beenden");
    gtk_box_pack_start(GTK_BOX(box), bquit, TRUE, TRUE, 20);

    g_signal_connect(G_OBJECT(bstart),
                     "clicked",
                     G_CALLBACK(sim_decay),
                     NULL);

    gtk_widget_set_sensitive(bstop, FALSE);

    g_signal_connect(G_OBJECT(bquit),
                     "clicked",
                     G_CALLBACK(gtk_exit),
                     NULL);

    top = gtk_widget_get_toplevel(parent_box);

    g_object_set_data(G_OBJECT(top), "start_button", bstart);
    g_object_set_data(G_OBJECT(top), "stop_button", bstop);
    g_object_set_data(G_OBJECT(top), "quit_button", bquit);

    gtk_widget_show(bstart);
    gtk_widget_show(bstop);
    gtk_widget_show(bquit);
    gtk_widget_show(box);

    return box;
}
