/* 
 * gui.c - die Benutzeroberfläche
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

#include "gui.h"
#include "opt.h"
#include "atoms.h"
#include "darea.h"
#include "color.h"
#include "util.h"
#include "ui_graph.h"
#include "ui_afield.h"

#include <gtk/gtk.h>
#include <stdlib.h>
#include <math.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

#define N_BOXES 8
#define DEFAULT_NUMBER 200
#define DEFAULT_HTIME(x) atoms_get_htime(x)

static gboolean delete_event(void);
static void destroy(void);

static void toggle_states(GtkWidget *spin_states, GtkWidget *top);
static void set_graph_type_number(GtkWidget *radio, GtkWidget *top);
static void set_graph_type_activity(GtkWidget *radio, GtkWidget *top);

static GtkWidget *create_time_menu(void);

static GtkWidget *create_template_menu(void);
static void set_template(GtkWidget *menu, GtkWidget *top);
static void add_template(GtkWidget *menu, gint n, const gchar *text, ...);

static gchar *format_log_scale(GtkScale *scale, gdouble value);
static gboolean reset_scale(GtkWidget *scale, GdkEventButton *event);

static void create_dareas(GtkWidget *top, GtkWidget *box1, GtkWidget *box2);
static void create_status(GtkWidget *top, GtkWidget *box);
static void create_sim_input(GtkWidget *top, GtkWidget *box);
static void create_graph_input(GtkWidget *top, GtkWidget *box);
static void create_buttons(GtkWidget *top, GtkWidget *box);

static void create_colors(GtkWidget *top);
static void color_graph_input(GtkWidget *top);

enum { N_SECOND = 0, N_MINUTE, N_HOUR, N_DAY, N_YEAR };

/* erstellt die Bedienungsoberfläche */
GtkWidget *gui_create(void)
{
    gint i, scr_width, scr_height;
    GdkScreen *screen;
    GtkWidget *window, *box[N_BOXES];

    /* ruft die Abmessungen des Bildschirms (screen) ab */
    screen = gdk_screen_get_default();
    scr_width = gdk_screen_get_width(screen);
    scr_height = gdk_screen_get_height(screen);

    /* erstellt das Hauptfenster, und setzt Größe,
       Namen und Randabstand */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window),
                                scr_width * 0.85, scr_height * 0.85);
    gtk_window_set_title(GTK_WINDOW(window),
                         _("simulation of radioactive decay"));
    gtk_container_set_border_width(GTK_CONTAINER(window), 5);

    /* verknüpft das Hauptfenster mit Callback-Funktionen */
    g_signal_connect(G_OBJECT(window),
                     "delete_event",
                     G_CALLBACK(delete_event),
                     NULL);

    g_signal_connect(G_OBJECT(window),
                     "destroy",
                     G_CALLBACK(destroy),
                     NULL);

    /* erstellt Layout

       Skizze der Kästen (box)
       |-0---------------------------------------------------|
       | |-1-----------------------------------------------| |
       | |                               |-2-------------| | |
       | |                               |               | | |
       | |                               |               | | |
       | |                               |               | | |
       | |                               |               | | |
       | |                               | |-3---------| | | |
       | |                               | |           | | | |
       | |                               | |-----------| | | |
       | |                               |---------------| | |
       | |-------------------------------------------------| |
       |                                                     |
       | |-4-----------------------------------------------| |
       | | |-5---| |-6------------------------| |-7------| | |
       | | |     | |                          | |        | | |
       | | |-----| |--------------------------| |--------| | |
       | |-------------------------------------------------| |
       |-----------------------------------------------------|
    */

    box[0] = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), box[0]);

    box[1] = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box[0]), box[1], TRUE, TRUE, 0);

    box[2] = gtk_vbox_new(FALSE, 0);

    /* erstellt die beiden Zeichenbereiche */
    create_dareas(window, box[1], box[2]);

    box[3] = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box[2]), box[3], FALSE, FALSE, 0);

    /* erstellt die Status-Anzeigen */
    create_status(window, box[3]);

    box[4] = gtk_hbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box[0]), box[4], FALSE, FALSE, 0);

    box[5] = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box[4]), box[5], FALSE, FALSE, 0);

    /* erstellt die Eingabefelder */
    create_sim_input(window, box[5]);

    box[6] = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box[4]), box[6], FALSE, FALSE, 0);

    /* erstellt die Graphen-Auswahl */
    create_graph_input(window, box[6]);

    box[7] = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box[4]), box[7], TRUE, TRUE, 0);

    toggle_states(g_object_get_data(G_OBJECT(window), "spin_states"),
                  window);

    /* erstellt die Buttons */
    create_buttons(window, box[7]);

    /* gibt die Widgets zum Zeichnen auf dem Bildschirm frei */
    for (i = 0; i < N_BOXES; i++)
        gtk_widget_show(box[i]);
    gtk_widget_show(window);

    /* erstellt eine Liste mit ein paar Farben, die dann
       später im Programm benutzt werden (das _muss_ nach
       dem Zeichnen auf den Bildschirm passieren) */
    create_colors(window);

    /* setzt die Farben der Checkboxen der Graphen */
    color_graph_input(window);
    
    return window;
}

/* zerstört das GUI */
void destroy(void)
{
    opt_free();
    gtk_main_quit();
}


/* berechnet die eingestellte Ablaufgeschwindigkeit */
gdouble scale_get_speed(GtkWidget *scale_speed)
{
    return pow(10.0, gtk_range_get_value(GTK_RANGE(scale_speed)));
}

/* erstellt die beiden Zeichenbereiche */
static void create_dareas(GtkWidget *top, GtkWidget *box1, GtkWidget *box2)
{
    GtkWidget *pane, *frame[N_DAREAS], **darea;
    gint i, width, height;

    /* reserviert Speicher für die Zeichenbereiche */
    darea = (GtkWidget **) g_malloc(N_DAREAS * sizeof(GtkWidget *));
    g_signal_connect_swapped(G_OBJECT(top), 
                             "destroy", 
                             G_CALLBACK(g_free),
                             darea);

    /* holt die Breite und Höhe des Hauptfensters */
    gtk_window_get_size(GTK_WINDOW(top), &width, &height);

    /* erstellt den verstellbaren Balken */
    pane = gtk_hpaned_new();
    gtk_box_pack_start(GTK_BOX(box1), pane, TRUE, TRUE, 0);
    gtk_paned_set_position(GTK_PANED(pane), 0.6 * width);

    /* erstellt die beiden Zeichenfelder */
    frame[0] = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[0]), GTK_SHADOW_IN);
    gtk_paned_pack1(GTK_PANED(pane), frame[0], FALSE, FALSE);
    darea[0] = darea_new();
    gtk_container_add(GTK_CONTAINER(frame[0]), darea[0]);

    gtk_paned_pack2(GTK_PANED(pane), box2, FALSE, FALSE);
    frame[1] = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[1]), GTK_SHADOW_IN);
    gtk_box_pack_start(GTK_BOX(box2), frame[1], TRUE, TRUE, 0);
    darea[1] = darea_new();
    gtk_widget_add_events(darea[1], GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(darea[1]), "configure_event",
                              G_CALLBACK(graph_rebuild), NULL);
    g_signal_connect(G_OBJECT(darea[1]), "button-press-event",
                              G_CALLBACK(coord_draw_pos), NULL);
    gtk_container_add(GTK_CONTAINER(frame[1]), darea[1]);

    g_object_set_data(G_OBJECT(top), "darea", darea);

    for (i = 0; i < N_DAREAS; i++) {
        gtk_widget_show(darea[i]);
        gtk_widget_show(frame[i]);
    }
    gtk_widget_show(pane);
}

static void create_status(GtkWidget *top, GtkWidget *box)
{
    GtkWidget *frame, *table, *label[ATOM_STATES + 1],
              **label_atom, *label_time, **progress_atom;
    gint i;

    label_atom = (GtkWidget **)
                 g_malloc(ATOM_STATES * sizeof(GtkWidget *));
    progress_atom = (GtkWidget **)
                 g_malloc(ATOM_STATES * sizeof(GtkWidget *));
    g_signal_connect_swapped(G_OBJECT(top), 
                             "destroy", 
                             G_CALLBACK(g_free),
                             label_atom);
    g_signal_connect_swapped(G_OBJECT(top), 
                             "destroy", 
                             G_CALLBACK(g_free),
                             progress_atom);

    frame = gtk_frame_new(NULL);
    gtk_box_pack_start(GTK_BOX(box), frame, FALSE, FALSE, 0);

    table = gtk_table_new(ATOM_STATES + 1, 3, FALSE);
    gtk_container_add(GTK_CONTAINER(frame), table);

    for (i = 0; i < ATOM_STATES; i++) {
        label[i] = gtk_label_new(NULL);
        label_printf(label[i], "%s: ", _(atoms_get_name(i)));

        label_atom[i] = gtk_label_new("-");
        gtk_misc_set_alignment(GTK_MISC(label_atom[i]), 0, 0);
        gtk_table_attach(GTK_TABLE(table), label_atom[i], 1, 2, i, i + 1,
                         GTK_FILL, GTK_SHRINK, 0, 0);

        progress_atom[i] = gtk_progress_bar_new();
        gtk_table_attach(GTK_TABLE(table), progress_atom[i], 2, 3, i, i + 1,
                         GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
    }

    label[i] = gtk_label_new(_("time: "));
    label_time = gtk_label_new("-");
    gtk_misc_set_alignment(GTK_MISC(label_time), 0, 0);
    gtk_table_attach(GTK_TABLE(table), label_time, 1, 2, i, i + 1,
                     GTK_FILL, GTK_SHRINK, 0, 0);

    for (i = 0; i <= ATOM_STATES; i++) {
        gtk_misc_set_alignment(GTK_MISC(label[i]), 1, 0);
        gtk_table_attach(GTK_TABLE(table), label[i], 0, 1, i, i + 1,
                         GTK_FILL, GTK_SHRINK, 0, 0);
    }

    g_object_set_data(G_OBJECT(top), "label_atom", label_atom);
    g_object_set_data(G_OBJECT(top), "label_time", label_time);
    g_object_set_data(G_OBJECT(top), "progress_atom", progress_atom);

    for (i = 0; i <= ATOM_STATES; i++)
        gtk_widget_show(label[i]);
    for (i = 0; i < ATOM_STATES; i++) {
        gtk_widget_show(label_atom[i]);
        gtk_widget_show(progress_atom[i]);
    }
    gtk_widget_show(label_time);
    gtk_widget_show(table);
    gtk_widget_show(frame);
}

static void create_sim_input(GtkWidget *top, GtkWidget *box)
{
    GtkWidget *table, *frame, *ibox, *label[2 + (ATOM_STATES - 1)],
              *spin_number, **spin_htime, *spin_states,
              **menu_htime, *menu_template, *sep;
    GtkAdjustment *adj_number, *adj_htime[ATOM_STATES-1], *adj_states;
    gint i;

    spin_htime = (GtkWidget **) g_malloc((ATOM_STATES-1) *
                                         sizeof(GtkWidget *));
    menu_htime = (GtkWidget **) g_malloc((ATOM_STATES-1) *
                                         sizeof(GtkWidget *));
    g_signal_connect_swapped(G_OBJECT(top),
                             "destroy", 
                             G_CALLBACK(g_free),
                             spin_htime);
    g_signal_connect_swapped(G_OBJECT(top),
                             "destroy", 
                             G_CALLBACK(g_free),
                             menu_htime);

    adj_number = (GtkAdjustment *)
                 gtk_adjustment_new(DEFAULT_NUMBER, 1.0,
                                    G_MAXULONG, 1.0,
                                    100.0, 0);

    for (i = 0; i < (ATOM_STATES - 1); i++) {
        adj_htime[i] = (GtkAdjustment *)
                       gtk_adjustment_new(DEFAULT_HTIME(i), 0.1,
                                          2 << 19, 0.1,
                                          100.0, 0);
    }

    adj_states = (GtkAdjustment *)
                 gtk_adjustment_new(2, 2, ATOM_STATES, 1, 1, 0);

    frame = gtk_frame_new(NULL);
    gtk_box_pack_start(GTK_BOX(box), frame, TRUE, TRUE, 0);

    ibox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(frame), ibox);

    table = gtk_table_new(2 + (ATOM_STATES - 1), 3, FALSE);
    gtk_box_pack_start(GTK_BOX(ibox), table, FALSE, FALSE, 0);

    label[0] = gtk_label_new(_("generations: "));
    gtk_misc_set_alignment(GTK_MISC(label[0]), 0, 0);
    gtk_table_attach(GTK_TABLE(table), label[0], 0, 1, 0, 1,
                     GTK_FILL, GTK_SHRINK, 0, 0);

    spin_states = gtk_spin_button_new(adj_states, 1, 0);
    gtk_table_attach(GTK_TABLE(table), spin_states, 1, 2, 0, 1,
                     GTK_FILL, GTK_SHRINK, 0, 0);

    g_signal_connect(G_OBJECT(spin_states),
                     "value-changed",
                     G_CALLBACK(toggle_states),
                     top);

    label[1] = gtk_label_new(_("atoms: "));
    gtk_misc_set_alignment(GTK_MISC(label[1]), 0, 0);
    gtk_table_attach(GTK_TABLE(table), label[1], 0, 1, 1, 2,
                     GTK_FILL, GTK_SHRINK, 0, 0);

    spin_number = gtk_spin_button_new(adj_number, 0.5, 0);
    gtk_table_attach(GTK_TABLE(table), spin_number, 1, 2, 1, 2,
                     GTK_FILL, GTK_SHRINK, 0, 0);

    for (i = 0; i < (ATOM_STATES - 1); i++) {
        label[2+i] = gtk_label_new(NULL);
        label_printf(label[2+i], _("half-life %s"), _(atoms_get_name(i)));
        gtk_misc_set_alignment(GTK_MISC(label[2+i]), 0, 0);
        gtk_table_attach(GTK_TABLE(table), label[2+i], 0, 1, 2+i, 2+i+1,
                         GTK_FILL, GTK_SHRINK, 0, 0);

        spin_htime[i] = gtk_spin_button_new(adj_htime[i], 0.8, 2);
        gtk_table_attach(GTK_TABLE(table), spin_htime[i], 1, 2, 2+i, 2+i+1,
                         GTK_FILL, GTK_SHRINK, 0, 0);

        menu_htime[i] = create_time_menu();
        gtk_table_attach(GTK_TABLE(table), menu_htime[i], 2, 3, 2+i, 2+i+1,
                         GTK_FILL, GTK_SHRINK, 0, 0);
    }

    sep = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(ibox), sep, FALSE, FALSE, 5);

    menu_template = create_template_menu();
    g_signal_connect(G_OBJECT(menu_template),
                     "changed",
                     G_CALLBACK(set_template),
                     top);
    gtk_box_pack_start(GTK_BOX(ibox), menu_template, FALSE, FALSE, 0);

    g_object_set_data(G_OBJECT(top), "spin_states", spin_states);
    g_object_set_data(G_OBJECT(top), "spin_number", spin_number);
    g_object_set_data(G_OBJECT(top), "spin_htime", spin_htime);
    g_object_set_data(G_OBJECT(top), "menu_htime", menu_htime);

    gtk_widget_show(frame);
    gtk_widget_show(ibox);
    gtk_widget_show(table);
    gtk_widget_show(sep);
    for (i = 0; i < (2 + (ATOM_STATES - 1)); i++)
        gtk_widget_show(label[i]);
    gtk_widget_show(spin_number);
    for (i = 0; i < ATOM_STATES-1; i++) {
        gtk_widget_show(spin_htime[i]);
        gtk_widget_show(menu_htime[i]);
    }
    gtk_widget_show(menu_template);
    gtk_widget_show(spin_states);
}

static void create_graph_input(GtkWidget *top, GtkWidget *box)
{
    GtkWidget *frame, *ibox, *box_radio, *table,
              *label_atoms[ATOM_STATES], *label_graphs[3],
              **check_graph_real, **check_graph_stat,
              **check_graph_step, **radio_graph_type, *sep;
    gint i;

    radio_graph_type = (GtkWidget **) g_malloc(2 * sizeof(GtkWidget *));
    check_graph_real = (GtkWidget **) g_malloc(ATOM_STATES *
                                               sizeof(GtkWidget *));
    check_graph_stat = (GtkWidget **) g_malloc(ATOM_STATES *
                                               sizeof(GtkWidget *));
    check_graph_step = (GtkWidget **) g_malloc(ATOM_STATES *
                                               sizeof(GtkWidget *));
    g_signal_connect_swapped(G_OBJECT(top), 
                             "destroy", 
                             G_CALLBACK(g_free),
                             radio_graph_type);
    g_signal_connect_swapped(G_OBJECT(top), 
                             "destroy", 
                             G_CALLBACK(g_free),
                             check_graph_real);
    g_signal_connect_swapped(G_OBJECT(top), 
                             "destroy", 
                             G_CALLBACK(g_free),
                             check_graph_stat);
    g_signal_connect_swapped(G_OBJECT(top), 
                             "destroy", 
                             G_CALLBACK(g_free),
                             check_graph_step);


    frame = gtk_frame_new(NULL);
    gtk_box_pack_start(GTK_BOX(box), frame, TRUE, TRUE, 0);

    ibox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(frame), ibox);


    box_radio = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(ibox), box_radio, FALSE, FALSE, 0);

    radio_graph_type[0] = gtk_radio_button_new_with_label(NULL,
                                                          _("number"));
    gtk_box_pack_start(GTK_BOX(box_radio), radio_graph_type[0],
                       FALSE, FALSE, 0);

    radio_graph_type[1] = gtk_radio_button_new_with_label_from_widget(
            GTK_RADIO_BUTTON(radio_graph_type[0]), _("activity"));
    gtk_box_pack_start(GTK_BOX(box_radio), radio_graph_type[1],
                       FALSE, FALSE, 0);

    g_signal_connect(G_OBJECT(radio_graph_type[0]),
                     "clicked",
                     G_CALLBACK(set_graph_type_number),
                     top);
    g_signal_connect(G_OBJECT(radio_graph_type[1]), 
                     "clicked", 
                     G_CALLBACK(set_graph_type_activity),
                     top);

    sep = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(ibox), sep, FALSE, FALSE, 5);

    table = gtk_table_new(4, 4, FALSE);
    gtk_box_pack_start(GTK_BOX(ibox), table, FALSE, FALSE, 0);

    label_graphs[0] = gtk_label_new(_("real"));
    gtk_table_attach(GTK_TABLE(table), label_graphs[0], 0, 1, 0, 1,
                     GTK_FILL, GTK_SHRINK, 5, 0);

    label_graphs[1] = gtk_label_new(_("stat."));
    gtk_table_attach(GTK_TABLE(table), label_graphs[1], 1, 2, 0, 1,
                     GTK_FILL, GTK_SHRINK, 5, 0);

    label_graphs[2] = gtk_label_new(_("step"));
    gtk_table_attach(GTK_TABLE(table), label_graphs[2], 2, 3, 0, 1,
                     GTK_FILL, GTK_SHRINK, 5, 0);

    for (i = 0; i < ATOM_STATES; i++) {
        check_graph_real[i] = gtk_check_button_new();
        gtk_table_attach(GTK_TABLE(table), check_graph_real[i],
                         0, 1, 1+i, 1+i+1, GTK_FILL, GTK_SHRINK, 0, 0);

        check_graph_stat[i] = gtk_check_button_new();
        gtk_table_attach(GTK_TABLE(table), check_graph_stat[i],
                         1, 2, 1+i, 1+i+1, GTK_FILL, GTK_SHRINK, 0, 0);

        check_graph_step[i] = gtk_check_button_new();
        gtk_table_attach(GTK_TABLE(table), check_graph_step[i],
                         2, 3, 1+i, 1+i+1, GTK_FILL, GTK_SHRINK, 0, 0);

        label_atoms[i] = gtk_label_new(NULL);
        label_printf(label_atoms[i], _("%s atoms"), _(atoms_get_name(i)));
        gtk_misc_set_alignment(GTK_MISC(label_atoms[i]), 0, 0);
        gtk_table_attach(GTK_TABLE(table), label_atoms[i],
                         3, 4, 1+i, 1+i+1, GTK_FILL, GTK_SHRINK, 0, 0);
    }

    g_object_set_data(G_OBJECT(top), "radio_graph_type", radio_graph_type);
    g_object_set_data(G_OBJECT(top), "check_graph_real", check_graph_real);
    g_object_set_data(G_OBJECT(top), "check_graph_stat", check_graph_stat);
    g_object_set_data(G_OBJECT(top), "check_graph_step", check_graph_step);

    gtk_widget_show(frame);
    gtk_widget_show(ibox);
    gtk_widget_show(box_radio);
    gtk_widget_show(table);
    gtk_widget_show(sep);
    for (i = 0; i < ATOM_STATES; i++)
        gtk_widget_show(label_atoms[i]);
    for (i = 0; i < 3; i++)
        gtk_widget_show(label_graphs[i]);
    for (i = 0; i < 2; i++)
        gtk_widget_show(radio_graph_type[i]);
    for (i = 0; i < ATOM_STATES; i++) {
        gtk_widget_show(check_graph_real[i]);
        gtk_widget_show(check_graph_stat[i]);
        gtk_widget_show(check_graph_step[i]);
    }
}

static void create_buttons(GtkWidget *top, GtkWidget *box)
{
    GtkWidget *button_box, *button_start, *button_stop,
              *button_quit, *scale_speed, *align,
              *label;
    GtkAdjustment *adj_speed;

    adj_speed = (GtkAdjustment *)
                gtk_adjustment_new(0.0, -3.0, 15.0, 1.0, 2.0, 0.0);

    align = gtk_alignment_new(0, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(box), align, TRUE, TRUE, 0);

    button_box = gtk_vbutton_box_new();
    gtk_container_add(GTK_CONTAINER(align), button_box);

    button_start = gtk_button_new_with_label(_("start"));
    gtk_container_add(GTK_CONTAINER(button_box), button_start);

    button_stop = gtk_button_new_with_label(_("stop"));
    gtk_container_add(GTK_CONTAINER(button_box), button_stop);

    button_quit = gtk_button_new_with_label(_("quit"));
    gtk_container_add(GTK_CONTAINER(button_box), button_quit);

    label = gtk_label_new(_("speed"));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

    scale_speed = gtk_hscale_new(adj_speed);
    g_signal_connect(G_OBJECT(scale_speed),
                     "format-value",
                     G_CALLBACK(format_log_scale),
                     NULL);
    g_signal_connect(G_OBJECT(scale_speed),
                     "button-press-event",
                     G_CALLBACK(reset_scale),
                     NULL);
    gtk_box_pack_start(GTK_BOX(box), scale_speed, FALSE, FALSE, 0);

    /* schaltet den Stopbutton aus */
    gtk_widget_set_sensitive(button_stop, FALSE);

    /* das Hauptfenster wird zerstört,
       wenn der Beenden-Knopf gedrückt wird */
    g_signal_connect_swapped(G_OBJECT(button_quit),
                             "clicked",
                             G_CALLBACK(gtk_widget_destroy),
                             G_OBJECT(top));

    g_object_set_data(G_OBJECT(top), "button_start", button_start);
    g_object_set_data(G_OBJECT(top), "button_stop", button_stop);
    g_object_set_data(G_OBJECT(top), "button_quit", button_quit);
    g_object_set_data(G_OBJECT(top), "scale_speed", scale_speed);

    gtk_widget_show(button_start);
    gtk_widget_show(button_stop);
    gtk_widget_show(button_quit);
    gtk_widget_show(label);
    gtk_widget_show(scale_speed);
    gtk_widget_show(align);
    gtk_widget_show(button_box);
}

static void create_colors(GtkWidget *top)
{
    GdkColormap *colormap;
    GdkColor **color_graph_real, **color_graph_stat,
             **color_atom, *color_grid;
    GdkGC **style_graph_real, **style_graph_stat,
          **style_atom, *style_grid;
    gint i;

    color_graph_real = (GdkColor **)
                       g_malloc(ATOM_STATES * sizeof(GdkColor *));
    style_graph_real = (GdkGC **)
                       g_malloc(ATOM_STATES * sizeof(GdkGC *));

    color_graph_stat = (GdkColor **)
                       g_malloc(ATOM_STATES * sizeof(GdkColor *));
    style_graph_stat = (GdkGC **)
                       g_malloc(ATOM_STATES * sizeof(GdkGC *));

    color_atom = (GdkColor **)
                 g_malloc(ATOM_STATES * sizeof(GdkColor *));
    style_atom = (GdkGC **)
                 g_malloc(ATOM_STATES * sizeof(GdkGC *));

    g_signal_connect_swapped(G_OBJECT(top),
                             "destroy",
                             G_CALLBACK(g_free),
                             color_graph_real);
    g_signal_connect_swapped(G_OBJECT(top), 
                             "destroy", 
                             G_CALLBACK(g_free),
                             style_graph_real);
    g_signal_connect_swapped(G_OBJECT(top), 
                             "destroy", 
                             G_CALLBACK(g_free),
                             color_graph_stat);
    g_signal_connect_swapped(G_OBJECT(top), 
                             "destroy", 
                             G_CALLBACK(g_free),
                             style_graph_stat);
    g_signal_connect_swapped(G_OBJECT(top), 
                             "destroy", 
                             G_CALLBACK(g_free),
                             color_atom);
    g_signal_connect_swapped(G_OBJECT(top), 
                             "destroy",
                             G_CALLBACK(g_free),
                             style_atom);


    colormap = gdk_colormap_get_system();

    for (;;) {
        i = 0;
        if (i >= ATOM_STATES)
            break;
        color_graph_real[i] = color_alloc(colormap, 0xff, 0x8d, 0x27);
        color_graph_stat[i] = color_alloc(colormap, 0xff, 0xc8, 0x00);
        color_atom[i] = color_alloc(colormap, 0xff, 0x8d, 0x27);

        i++;
        if (i >= ATOM_STATES)
            break;
        color_graph_real[i] = color_alloc(colormap, 0xc3, 0x00, 0x00);
        color_graph_stat[i] = color_alloc(colormap, 0xff, 0x50, 0x50);
        color_atom[i] = color_alloc(colormap, 0xc3, 0x00, 0x00);

        i++;
        color_graph_real[i] = color_alloc(colormap, 0x00, 0x64, 0x00);
        color_graph_stat[i] = color_alloc(colormap, 0x90, 0xee, 0x90);
        color_atom[i] = color_alloc(colormap, 0x00, 0x64, 0x00);

        i++;
        if (i >= ATOM_STATES)
            break;
        color_graph_real[i] = color_alloc(colormap, 0x00, 0x00, 0x8b);
        color_graph_stat[i] = color_alloc(colormap, 0xad, 0xd8, 0xe6);
        color_atom[i] = color_alloc(colormap, 0x00, 0x00, 0x8b);

        i++;
        if (i >= ATOM_STATES)
            break;
        color_graph_real[i] = color_alloc(colormap, 0x64, 0x00, 0x64);
        color_graph_stat[i] = color_alloc(colormap, 0xb0, 0x17, 0xae);
        color_atom[i] = color_alloc(colormap, 0x64, 0x00, 0x64);

        i++;
        if (i >= ATOM_STATES)
            break;
        color_graph_real[i] = color_alloc(colormap, 0x78, 0x2a, 0x2a);
        color_graph_stat[i] = color_alloc(colormap, 0xa5, 0x4f, 0x4f);
        color_atom[i] = color_alloc(colormap, 0x78, 0x2a, 0x2a);

        i++;
        if (i >= ATOM_STATES)
            break;
        color_graph_real[i] = color_alloc(colormap, 0x00, 0x00, 0x00);
        color_graph_stat[i] = color_alloc(colormap, 0x9a, 0x9a, 0x9a);
        color_atom[i] = color_alloc(colormap, 0x00, 0x00, 0x00);

        break;
    }

    for (i += 1; i < ATOM_STATES; i++) {
        color_graph_real[i] = color_alloc(colormap, rand(), rand(), rand());
        color_graph_stat[i] = color_alloc(colormap, rand(), rand(), rand());
        color_atom[i] = color_alloc(colormap, rand(), rand(), rand());
    }

    color_grid = color_alloc(colormap, 0xb1, 0xb1, 0xb1);

    for (i = 0; i < ATOM_STATES; i++) {
        style_graph_real[i] = gdk_gc_new(top->window);
        style_graph_stat[i] = gdk_gc_new(top->window);
        style_atom[i] = gdk_gc_new(top->window);

        gdk_gc_set_foreground(style_graph_real[i], color_graph_real[i]);
        gdk_gc_set_foreground(style_graph_stat[i], color_graph_stat[i]);
        gdk_gc_set_foreground(style_atom[i], color_atom[i]);
    }
    style_grid = gdk_gc_new(top->window);
    gdk_gc_set_foreground(style_grid, color_grid);

    g_object_set_data(G_OBJECT(top), "color_graph_real", color_graph_real);
    g_object_set_data(G_OBJECT(top), "style_graph_real", style_graph_real);
    g_object_set_data(G_OBJECT(top), "color_graph_stat", color_graph_stat);
    g_object_set_data(G_OBJECT(top), "style_graph_stat", style_graph_stat);
    g_object_set_data(G_OBJECT(top), "color_atom", color_atom);
    g_object_set_data(G_OBJECT(top), "style_atom", style_atom);
    g_object_set_data(G_OBJECT(top), "color_grid", color_grid);
    g_object_set_data(G_OBJECT(top), "style_grid", style_grid);
}

static void color_graph_input(GtkWidget *top)
{
    GtkWidget **check_graph_real, **check_graph_stat,
              **check_graph_step;
    GdkColor **color_graph_real, **color_graph_stat;
    gint i;

    check_graph_real = g_object_get_data(G_OBJECT(top), "check_graph_real");
    color_graph_real = g_object_get_data(G_OBJECT(top), "color_graph_real");
    check_graph_stat = g_object_get_data(G_OBJECT(top), "check_graph_stat");
    color_graph_stat = g_object_get_data(G_OBJECT(top), "color_graph_stat");
    check_graph_step = g_object_get_data(G_OBJECT(top), "check_graph_step");

    for (i = 0; i < ATOM_STATES; i++) {
        widget_modify_bg(check_graph_real[i], color_graph_real[i]);
        widget_modify_bg(check_graph_stat[i], color_graph_stat[i]);
        widget_modify_bg(check_graph_step[i], color_graph_stat[i]);
    }
}

static gboolean delete_event(void)
{
    return FALSE;
}

static void toggle_states(GtkWidget *spin_states, GtkWidget *top)
{
    GtkWidget **spin_htime, **menu_htime, **radio_graph_type,
              **check_graph_real, **check_graph_stat,
              **check_graph_step;
    gint states, turn, i;

    radio_graph_type = g_object_get_data(G_OBJECT(top), "radio_graph_type");
    spin_htime = g_object_get_data(G_OBJECT(top), "spin_htime");
    menu_htime = g_object_get_data(G_OBJECT(top), "menu_htime");
    check_graph_real = g_object_get_data(G_OBJECT(top), "check_graph_real");
    check_graph_stat = g_object_get_data(G_OBJECT(top), "check_graph_stat");
    check_graph_step = g_object_get_data(G_OBJECT(top), "check_graph_step");

    states = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_states));
    turn = gtk_toggle_button_get_active(
           GTK_TOGGLE_BUTTON(radio_graph_type[0])) ? states : states - 1;

    for (i = 0; i < (states - 1); i++) {
        gtk_widget_set_sensitive(spin_htime[i], TRUE);
        gtk_widget_set_sensitive(menu_htime[i], TRUE);
    }
    for (i = 0; i < turn; i++) {
        gtk_widget_set_sensitive(check_graph_real[i], TRUE);
        gtk_widget_set_sensitive(check_graph_stat[i], TRUE);
        gtk_widget_set_sensitive(check_graph_step[i], TRUE);
    }

    for (i = states - 1; i < (ATOM_STATES - 1); i++) {
        gtk_widget_set_sensitive(spin_htime[i], FALSE);
        gtk_widget_set_sensitive(menu_htime[i], FALSE);
    }
    for (i = turn; i < ATOM_STATES; i++) {
        gtk_widget_set_sensitive(check_graph_real[i], FALSE);
        gtk_widget_set_sensitive(check_graph_stat[i], FALSE);
        gtk_widget_set_sensitive(check_graph_step[i], FALSE);
    }

    /* Mathematik-Hack */
    if (states > 3)
        for (i = 0; i < ATOM_STATES; i++) {
            gtk_widget_set_sensitive(check_graph_stat[i], FALSE);
            gtk_widget_set_sensitive(check_graph_step[i], FALSE);
        }
}

static void set_graph_type_number(GtkWidget *radio, GtkWidget *top)
{
    GtkWidget **check_graph_real, **check_graph_stat,
              **check_graph_step, *spin_states;
    gint states, i;

    IGNORE(radio);
    spin_states = g_object_get_data(G_OBJECT(top), "spin_states");
    check_graph_real = g_object_get_data(G_OBJECT(top), "check_graph_real");
    check_graph_stat = g_object_get_data(G_OBJECT(top), "check_graph_stat");
    check_graph_step = g_object_get_data(G_OBJECT(top), "check_graph_step");

    states = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_states));

    for (i = 0; i < states; i++) {
        gtk_widget_set_sensitive(check_graph_real[i], TRUE);
        gtk_widget_set_sensitive(check_graph_stat[i], TRUE);
        gtk_widget_set_sensitive(check_graph_step[i], TRUE);
    }

    /* Mathematik-Hack */
    if (states > 3)
        for (i = 0; i < ATOM_STATES; i++) {
            gtk_widget_set_sensitive(check_graph_stat[i], FALSE);
            gtk_widget_set_sensitive(check_graph_step[i], FALSE);
        }
}
    
static void set_graph_type_activity(GtkWidget *radio, GtkWidget *top)
{
    GtkWidget **check_graph_real, **check_graph_stat,
              **check_graph_step, *spin_states;
    gint states, i;

    IGNORE(radio);
    spin_states = g_object_get_data(G_OBJECT(top), "spin_states");
    check_graph_real = g_object_get_data(G_OBJECT(top), "check_graph_real");
    check_graph_stat = g_object_get_data(G_OBJECT(top), "check_graph_stat");
    check_graph_step = g_object_get_data(G_OBJECT(top), "check_graph_step");

    states = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_states));

    for (i = 0; i < (states - 1); i++) {
        gtk_widget_set_sensitive(check_graph_real[i], TRUE);
        gtk_widget_set_sensitive(check_graph_stat[i], TRUE);
    }
    gtk_widget_set_sensitive(check_graph_real[i], FALSE);
    gtk_widget_set_sensitive(check_graph_stat[i], FALSE);
    gtk_widget_set_sensitive(check_graph_step[i], FALSE);

    /* Mathematik-Hack */
    if (states > 3)
        for (i = 0; i < ATOM_STATES; i++) {
            gtk_widget_set_sensitive(check_graph_stat[i], FALSE);
            gtk_widget_set_sensitive(check_graph_step[i], FALSE);
        }
}

static gchar *format_log_scale(GtkScale *scale, gdouble value)
{
    IGNORE(scale);
    return strdup_pretty_number(pow(10, value), TRUE);
}

static gboolean reset_scale(GtkWidget *scale, GdkEventButton *event)
{
    if (event->button == 3)
        gtk_range_set_value(GTK_RANGE(scale), 0.0);
    return FALSE;
}

static GtkWidget *create_time_menu(void)
{
    GtkWidget *menu, *option_menu, *menu_item[5];

    menu = gtk_menu_new();

    menu_item[N_SECOND] = gtk_menu_item_new_with_label("s");
    menu_item[N_MINUTE] = gtk_menu_item_new_with_label("min");
    menu_item[N_HOUR] = gtk_menu_item_new_with_label("h");
    menu_item[N_DAY] = gtk_menu_item_new_with_label("d");
    menu_item[N_YEAR] = gtk_menu_item_new_with_label("a");

    g_object_set_data(G_OBJECT(menu_item[N_SECOND]), "factor",
                      GINT_TO_POINTER(1));
    g_object_set_data(G_OBJECT(menu_item[N_MINUTE]), "factor",
                      GINT_TO_POINTER(60));
    g_object_set_data(G_OBJECT(menu_item[N_HOUR]), "factor",
                      GINT_TO_POINTER(3600));
    g_object_set_data(G_OBJECT(menu_item[N_DAY]), "factor",
                      GINT_TO_POINTER(86400));
    g_object_set_data(G_OBJECT(menu_item[N_YEAR]), "factor",
                      GINT_TO_POINTER(31536000));

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item[N_SECOND]);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item[N_MINUTE]);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item[N_HOUR]);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item[N_DAY]);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item[N_YEAR]);

    gtk_widget_show(menu_item[N_SECOND]);
    gtk_widget_show(menu_item[N_MINUTE]);
    gtk_widget_show(menu_item[N_HOUR]);
    gtk_widget_show(menu_item[N_DAY]);
    gtk_widget_show(menu_item[N_YEAR]);

    option_menu = gtk_option_menu_new();
    gtk_option_menu_set_menu(GTK_OPTION_MENU(option_menu), menu);

    gtk_widget_show(menu);

    return option_menu;
}

static void add_template(GtkWidget *menu, gint n, const gchar *text, ...)
{
    va_list ap;
    GtkWidget *menu_item;
    gdouble *htime;
    gint i, *htime_unit;

    htime = (gdouble *) g_malloc(n * sizeof(gdouble));
    htime_unit = (gint *) g_malloc(n * sizeof(gint));

    va_start(ap, text);
    for (i = 0; i < n; i++) {
        htime[i] = va_arg(ap, gdouble);
        htime_unit[i] = va_arg(ap, gint);
    }
    va_end(ap);

    menu_item = gtk_menu_item_new_with_label(text);

    g_object_set_data(G_OBJECT(menu_item), "states", GINT_TO_POINTER(n+1));
    g_object_set_data(G_OBJECT(menu_item), "htime", htime);
    g_object_set_data(G_OBJECT(menu_item), "htime_unit", htime_unit);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    gtk_widget_show(menu_item);
}

static GtkWidget *create_template_menu(void)
{
    GtkWidget *menu, *option_menu;
    gint i;

    menu = gtk_menu_new();
    i = 0;

    add_template(menu, 2, "Th-232 -> Ra-228 -> Pb-208",
                 1.4e10, N_YEAR, 5.8, N_YEAR);
    i++;

    add_template(menu, 4, "Am-241 -> Np-237 -> U-233 -> Th-229 -> Bi-209",
                 4.3e2, N_YEAR, 2.1e6, N_YEAR, 1.6e5, N_YEAR, 7.3e3, N_YEAR);
    i++;

    add_template(menu, 3, "Th-230 -> Ra-226 -> Pb-210 -> Pb-206",
                 7.5e4, N_YEAR, 1.6e3, N_YEAR, 22.0, N_YEAR);
    i++;

    add_template(menu, 3, "U-235 -> Pa-231 -> Ac-227 -> Pb-207",
                 7.0e8, N_YEAR, 3.3e4, N_YEAR, 22.0, N_YEAR);
    i++;

    option_menu = gtk_option_menu_new();
    gtk_option_menu_set_menu(GTK_OPTION_MENU(option_menu), menu);
    gtk_option_menu_set_history(GTK_OPTION_MENU(option_menu),
                                i-1);

    gtk_widget_show(menu);

    return option_menu;
}

static void set_template(GtkWidget *menu, GtkWidget *top)
{
    GtkWidget **spin_htime, **menu_htime, *spin_states, *template;
    gdouble *htime;
    gint i, *htime_unit, states;

    spin_states = g_object_get_data(G_OBJECT(top), "spin_states");
    spin_htime = g_object_get_data(G_OBJECT(top), "spin_htime");
    menu_htime = g_object_get_data(G_OBJECT(top), "menu_htime");

    template = gtk_menu_get_active(
               GTK_MENU(gtk_option_menu_get_menu(GTK_OPTION_MENU(menu))));

    states = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(template),
                                               "states"));
    htime = g_object_get_data(G_OBJECT(template), "htime");
    htime_unit = g_object_get_data(G_OBJECT(template), "htime_unit");

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_states), states);
    for (i = 0; i < (states - 1); i++) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_htime[i]),
                                  htime[i]);
        gtk_option_menu_set_history(GTK_OPTION_MENU(menu_htime[i]),
                                    htime_unit[i]);
    }
}
