#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <gsl/gsl_rng.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

#include "sim.h"
#include "random.h"
#include "darea.h"
#include "color.h"
#include "util.h"

#include "ui_afield.h"
#include "ui_graph.h"

#define N_BOXES 13
#define N_LABELS 3

static gboolean delete_event(GtkWidget *window, GdkEvent *event, gpointer data);
static void destroy(GtkWidget *window, gpointer data);

int main(int argc, char *argv[])
{
    gint i, scr_width, scr_height, width, height;
    GdkScreen *screen;
    GdkColormap *colormap;
    GtkWidget *window, *box[N_BOXES], **darea,
              *button_start, *button_stop, *button_quit,
              *label[N_LABELS], **label_atom, *label_time,
              *spin_number, **spin_htime,
              *pane;
    GtkAdjustment *adj_number, *adj_htime[ATOM_STATES-1];
    gboolean color_success[N_COLORS];
    
    GdkColor *color;
    GdkGC **style;
    gsl_rng *rand;

    /* aktiviert Sprach- und Ländereinstellungen */
    gtk_set_locale();
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    /* initialisiert das Toolkit (für Fenster, Buttons, etc.) */
    gtk_init(&argc, &argv);

    /* ruft die Abmessungen des Bildschirms (screen) ab */
    screen = gdk_screen_get_default();
    scr_width = gdk_screen_get_width(screen);
    scr_height = gdk_screen_get_height(screen);

    /* erstellt das Hauptfenster, und setzt Größe, Namen und Randabstand */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window),
                                scr_width * 0.85, scr_height * 0.85);
    gtk_widget_set_name(window, _("simulation of radioactive decay"));
    gtk_container_set_border_width(GTK_CONTAINER(window), 5);

    /* initialisiert den Zufallsgenerator "Mersenne Twister" mit einem
       Wert, der aus Nutzereingaben (Mausbewegung, etc.) berechnet wird */
    rand = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(rand, random_get_seed());

    g_signal_connect_swapped(G_OBJECT(window),
                             "destroy",
                             G_CALLBACK(gsl_rng_free),
                             rand);

    /* verknüpft das Hauptfenster mit Callback-Funktionen */
    g_signal_connect(G_OBJECT(window),
                     "delete_event",
                     G_CALLBACK(delete_event),
                     NULL);

    g_signal_connect(G_OBJECT(window),
                     "destroy",
                     G_CALLBACK(destroy),
                     NULL);

    /* holt die Breite und Höhe des Hauptfensters */
    gtk_window_get_size(GTK_WINDOW(window), &width, &height);

    /* reserviert Platz für die Listen der GTK-Widgets */
    darea = (GtkWidget **) g_malloc(N_DAREAS * sizeof(GtkWidget *));
    label_atom = (GtkWidget **) g_malloc(ATOM_STATES * sizeof(GtkWidget *));
    spin_htime = (GtkWidget **) g_malloc((ATOM_STATES-1) * sizeof(GtkWidget *));

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

    box[0] = gtk_vbox_new(FALSE, 20);
    gtk_container_add(GTK_CONTAINER(window), box[0]);

    box[1] = gtk_hbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box[0]), box[1], TRUE, TRUE, 0);

    /* erstellt den verstellbaren Balken */
    pane = gtk_hpaned_new();
    gtk_box_pack_start(GTK_BOX(box[1]), pane, TRUE, TRUE, 0);
    gtk_paned_set_position(GTK_PANED(pane), 0.6 * width);

    /* erstellt die beiden Zeichenfelder */
    darea[0] = darea_new();
/*    gtk_box_pack_start(GTK_BOX(box[1]), darea[0], TRUE, TRUE, 0); */
    gtk_paned_pack1(GTK_PANED(pane), darea[0], FALSE, FALSE);

    box[2] = gtk_vbox_new(FALSE, 20);
/*    gtk_box_pack_start(GTK_BOX(box[1]), box[2], TRUE, TRUE, 0); */
    gtk_paned_pack2(GTK_PANED(pane), box[2], FALSE, FALSE);

    darea[1] = darea_new();
    gtk_box_pack_start(GTK_BOX(box[2]), darea[1], TRUE, TRUE, 0);

    box[3] = gtk_vbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box[2]), box[3], FALSE, FALSE, 0);


    /* erstellt die Status-Anzeigen */
    label_atom[0] = gtk_label_new(_("mother atoms: -"));
    label_atom[1] = gtk_label_new(_("doughter atoms: -"));
    label_atom[2] = gtk_label_new(_("grandchild atoms: -"));
    label_time = gtk_label_new(_("time: -"));

    gtk_box_pack_start(GTK_BOX(box[3]), label_atom[0], FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box[3]), label_atom[1], FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box[3]), label_atom[2], FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box[3]), label_time, FALSE, FALSE, 0);


    box[4] = gtk_hbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box[0]), box[4], FALSE, FALSE, 0);

    box[5] = gtk_vbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box[4]), box[5], FALSE, FALSE, 0);


    /* erstellt die Eingabefelder */
    adj_number = (GtkAdjustment *)
                 gtk_adjustment_new(200.0, 1.0,
                                    10000.0, 1.0,
                                    10.0, 0);

    adj_htime[0] = (GtkAdjustment *)
                   gtk_adjustment_new(10.0, 0.1,
                                      1000000.0, 0.1,
                                      10.0, 0);

    adj_htime[1] = (GtkAdjustment *)
                   gtk_adjustment_new(10.0, 0.1,
                                      1000000.0, 0.1,
                                      10.0, 0);

    box[8] = gtk_hbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box[5]), box[8], FALSE, FALSE, 0);

    label[0] = gtk_label_new(_("atoms: "));
    gtk_box_pack_start(GTK_BOX(box[8]), label[0], FALSE, FALSE, 0);

    spin_number = gtk_spin_button_new(adj_number, 0.5, 0);
    gtk_box_pack_start(GTK_BOX(box[8]), spin_number, FALSE, FALSE, 0);

    box[9] = gtk_hbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box[5]), box[9], FALSE, FALSE, 0);

    label[1] = gtk_label_new(_("half-life: "));
    gtk_box_pack_start(GTK_BOX(box[9]), label[1], FALSE, FALSE, 0);

    spin_htime[0] = gtk_spin_button_new(adj_htime[0], 0.5, 2);
    gtk_box_pack_start(GTK_BOX(box[9]), spin_htime[0], FALSE, FALSE, 0);

    box[10] = gtk_hbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box[5]), box[10], FALSE, FALSE, 0);

    label[2] = gtk_label_new(_("half-life2: "));
    gtk_box_pack_start(GTK_BOX(box[10]), label[2], FALSE, FALSE, 0);

    spin_htime[1] = gtk_spin_button_new(adj_htime[1], 0.5, 2);
    gtk_box_pack_start(GTK_BOX(box[10]), spin_htime[1], FALSE, FALSE, 0);


    box[6] = gtk_vbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box[4]), box[6], FALSE, FALSE, 0);


    /* FIXME Platzhalter */


    box[7] = gtk_vbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box[4]), box[7], FALSE, FALSE, 0);


    /* erstellt die Buttons */
    button_start = gtk_button_new_with_label(_("start"));
    gtk_box_pack_start(GTK_BOX(box[7]), button_start, TRUE, FALSE, 0);

    button_stop = gtk_button_new_with_label(_("stop"));
    gtk_box_pack_start(GTK_BOX(box[7]), button_stop, TRUE, FALSE, 0);

    button_quit = gtk_button_new_with_label(_("quit"));
    gtk_box_pack_start(GTK_BOX(box[7]), button_quit, TRUE, FALSE, 0);

    /* verknüpft den Startbutton mit der Simulationsfunktion */
    g_signal_connect(G_OBJECT(button_start),
                     "clicked",
                     G_CALLBACK(sim_decay),
                     rand);

    /* schaltet den Stopbutton aus */
    gtk_widget_set_sensitive(button_stop, FALSE);

    /* das Hauptfenster wird zerstört,
       wenn der Beenden-Knopf gedrückt wird */
    g_signal_connect_swapped(G_OBJECT(button_quit),
                             "clicked",
                             G_CALLBACK(gtk_widget_destroy),
                             G_OBJECT(window));


    /* gibt alle erstellten Widgets zum Zeichnen auf dem Bildschirm frei */
    for (i = 0; i < N_BOXES; i++)
        gtk_widget_show(box[i]);
    for (i = 0; i < N_DAREAS; i++)
        gtk_widget_show(darea[i]);
    for (i = 0; i < N_LABELS; i++)
        gtk_widget_show(label[i]);
    for (i = 0; i < ATOM_STATES; i++)
        gtk_widget_show(label_atom[i]);
    gtk_widget_show(label_time);
    gtk_widget_show(spin_number);
    for (i = 0; i < ATOM_STATES-1; i++)
        gtk_widget_show(spin_htime[i]);
    gtk_widget_show(pane);
    gtk_widget_show(button_start);
    gtk_widget_show(button_stop);
    gtk_widget_show(button_quit);
    gtk_widget_show(window);

    /* erstellt eine Liste mit ein paar Farben, die dann
       später im Programm benutzt werden (das _muss_ nach
       dem Zeichnen auf den Bildschirm passieren */
    colormap = gdk_colormap_get_system();
    color = (GdkColor *) g_malloc(N_COLORS * sizeof(GdkColor));
    style = (GdkGC **) g_malloc(N_STYLES * sizeof(GdkGC *));
    color[0] = color_new(0xff, 0x00, 0x00);
    color[1] = color_new(0x00, 0xff, 0x00);
    color[2] = color_new(0x00, 0x00, 0xff);
    color[3] = color_new(0, 0, 0);
    color[4] = color_new(0, 0, 0);
    gdk_colormap_alloc_colors(colormap, color, N_COLORS, FALSE, TRUE, color_success);
    for (i = 0; i < N_STYLES; i++) {
        style[i] = gdk_gc_new(window->window);
        gdk_gc_set_foreground(style[i], &color[i]);
    }
    g_free(color);

    /* speichert die Adressen von wichtigen
       Widgets anhand von kurzen Namen im Hauptfenster */
    g_object_set_data(G_OBJECT(window), "darea", darea);
    g_object_set_data(G_OBJECT(window), "style", style);
    g_object_set_data(G_OBJECT(window), "spin_number", spin_number);
    g_object_set_data(G_OBJECT(window), "spin_htime", spin_htime);
    g_object_set_data(G_OBJECT(window), "button_start", button_start);
    g_object_set_data(G_OBJECT(window), "button_stop", button_stop);
    g_object_set_data(G_OBJECT(window), "button_quit", button_quit);
    g_object_set_data(G_OBJECT(window), "label_atom", label_atom);
    g_object_set_data(G_OBJECT(window), "label_time", label_time);

    /* startet die GTK-Hauptschleife */
    gtk_main();

    return EXIT_SUCCESS;
}

static gboolean delete_event(GtkWidget *window, GdkEvent *event, gpointer data)
{
    IGNORE(window);
    IGNORE(event);
    IGNORE(data);
    return FALSE;
}

static void destroy(GtkWidget *window, gpointer data)
{
    IGNORE(window);
    IGNORE(data);
    gtk_main_quit();
}
