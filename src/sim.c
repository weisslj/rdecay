/* 
 * sim.c - die Simulation des Zerfalls
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

#include <gtk/gtk.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>

#include "sim.h"
#include "afield.h"
#include "graph.h"
#include "ui_afield.h"
#include "ui_graph.h"
#include "darea.h"
#include "status.h"
#include "timer.h"
#include "util.h"
/*
#include "sound.h"
*/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

static void resume_sim(GtkWidget *button, MyTimer *timer);
static void pause_sim(GtkWidget *button, MyTimer *timer);
static void stop_sim(GtkWidget *button, gboolean *quit);

static gdouble exp_growth(gdouble t, SimData *data);
static gdouble calc_duration(gint number, gdouble thalf);

static gint decay_poisson(gdouble t, gint n, gdouble thalf, gsl_rng *rand);
static gint decay_stat(gdouble t, gint n, gint n0, gdouble thalf);

/* startet die Simulation */
void sim_decay(GtkWidget *button_start, gsl_rng *rand)
{
    GtkWidget *top, **darea, *button_stop,
              *spin_number, **spin_htime,
              **label_atom, *label_time;
    gdouble t, thalf[ATOM_STATES-1], tstart, tnext, told, tstep, tloop;
    gint number, state, decays[ATOM_STATES], i, pos;
    gboolean quit, changed;

    CoordSystem *coord;
    Graph **graph;

    MyTimer *timer;
    AtomField *afield;

    gulong sig_darea[N_DAREAS];

    SimData *sdata;
    GraphFunc *gf;

    /* holt ein paar gespeicherte Widgets */
    top = gtk_widget_get_toplevel(button_start);
    button_stop = g_object_get_data(G_OBJECT(top), "button_stop");
    spin_number = g_object_get_data(G_OBJECT(top), "spin_number");
    spin_htime = (GtkWidget **) g_object_get_data(G_OBJECT(top), "spin_htime");
    darea = (GtkWidget **) g_object_get_data(G_OBJECT(top), "darea");
    label_atom = (GtkWidget **) g_object_get_data(G_OBJECT(top), "label_atom");
    label_time = g_object_get_data(G_OBJECT(top), "label_time");

    /* ersetzt den Startbutton durch den Pausebutton */
    g_signal_handlers_block_by_func(G_OBJECT(button_start), (gpointer) sim_decay, rand);
    gtk_button_set_label(GTK_BUTTON(button_start), _("pause"));
    gtk_button_leave(GTK_BUTTON(button_start));

    /* bereitet den Stopbutton vor */
    quit = FALSE;
    gtk_widget_set_sensitive(button_stop, TRUE);
    g_signal_connect(G_OBJECT(button_stop), "clicked", G_CALLBACK(stop_sim), &quit);

    /* holt die Eingaben des Nutzers von den Spinbutton */
    number = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_number));
    for (i = 0; i < ATOM_STATES-1; i++)
        thalf[i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_htime[i]));

    /* packt die Eingaben in die SimData Struktur  FIXME */
    sdata = (SimData *) g_malloc(sizeof(SimData));
    sdata->states = 3;
    sdata->atoms[0] = number;
    sdata->atoms[1] = 0;
    sdata->atoms[2] = 0;
    sdata->thalf[0] = thalf[0];
    sdata->thalf[1] = thalf[1];

    /* bereitet die Wachstumsfunktion vor */
    gf = (GraphFunc *) g_malloc(sizeof(GraphFunc));
    gf->func = (GraphHandler) (exp_growth);
    gf->data = sdata;

    /* erstellt ein neues Atomfeld und verteilt die Atome zufällig */
    afield = afield_new(number, (darea[0])->allocation.width, (darea[0])->allocation.height);
    afield_randomize(afield, rand);

    /* verbindet den Zeichenbereich mit der resize-Funktion des Atomfelds */
    sig_darea[0] = g_signal_connect(G_OBJECT(darea[0]), "configure_event",
                                    G_CALLBACK(afield_resize), afield);

    /* zeichnet das Atomfeld auf den Zeichenbereich */
    darea_clear(darea[0]);
    afield_draw(darea[0], afield);


    /* erstellt ein neues Koordinatensystem */
    coord = coord_system_new((darea[1])->allocation.width, (darea[1])->allocation.height,
                             0, calc_duration(number, thalf[0]),
                             0, number);

    /* verbindet den Zeichenbereich mit der resize-Funktion des Graphen */
    sig_darea[1] = g_signal_connect(G_OBJECT(darea[1]), "configure_event",
                                    G_CALLBACK(graph_resize), coord);

    /* zeichnet das Koordinatensystem auf den Zeichenbereich */
    darea_clear(darea[1]);
    coord_system_draw(darea[1], coord);


    /* erstellt eine Liste der Graphen und speichert die
       Adresse im Koordinatensystem FIXME */
    graph = (Graph **) g_malloc(4 * sizeof(Graph *));
    coord->graphs = NULL;

    /* zeichnet die Wachstumsfunktion auf den Zeichenbereich FIXME */
    /* graph_draw_func(gf, darea[1], coord); */
    graph[3] = graph_new_by_func(gf, 3, coord);
    graph_draw(graph[3], darea[1], coord);


    /* erstellt den 1. Graphen FIXME */
    graph[0] = graph_new(0);
    graph[1] = graph_new(1);
    graph[2] = graph_new(2);

    /* FIXME */
    coord->graphs = g_list_append(coord->graphs, graph[0]);
    coord->graphs = g_list_append(coord->graphs, graph[1]);
    coord->graphs = g_list_append(coord->graphs, graph[2]);
    coord->graphs = g_list_append(coord->graphs, graph[3]);

    /* setzt die Status-Anzeigen auf die Anfangswerte */
    status_update_atoms(label_atom, sdata->atoms);
    status_update_time(label_time, 0.0); 

    /* setzt die Aktualisierungsrate auf 250 */
    tstep = 0.004;


    /* startet den timer */
    timer = timer_new();

    /* bereitet den Pause-Button vor */
    g_signal_connect(G_OBJECT(button_start), "clicked", G_CALLBACK(pause_sim), timer);

    /* setzt Zeitvariablen auf die (wenige) bisher vergangene Zeit */
    tstart = tnext = told = timer_elapsed(timer);

    /* der Versuch läuft solange, bis alle Atome in den letzten Zustand
       gekommen sind, oder der Nutzer den Stop-Button drückt */
    while (sdata->atoms[sdata->states-1] < number && (!quit)) {
        t = timer_elapsed(timer) - tstart;
        if (t >= tnext) {
            tloop = t - told;
            told = t;

            changed = FALSE;
            for (state = 0; state < sdata->states - 1; state++) {
                decays[state] = (sdata->atoms[state] > 0) ? decay_poisson(tloop, sdata->atoms[state], thalf[state], rand) : 0;
                if (decays[state] > 0)
                    changed = TRUE;
            }

            if (changed) {
                for (state = 0; state < sdata->states - 1; state++) {
                    sdata->atoms[state + 0] -= decays[state];
                    sdata->atoms[state + 1] += decays[state];
                }

                status_update_atoms(label_atom, sdata->atoms);

                for (state = 0; state < sdata->states - 1; state++) {
                    for (i = 0; i < decays[state]; i++) {
                        pos = afield_distrib_decay(afield, state, rand);
                        draw_atom(darea[0], (afield->coords + pos), afield->wide);
                    }
                }
            }

            graph_add(graph[0], t, sdata->atoms[0]);
            if ((graph[0])->points->prev != NULL) {
                graph_draw_line(darea[1], coord,
                                ((Point *) (graph[0])->points->prev->data)->x,
                                ((Point *) (graph[0])->points->prev->data)->y,
                                ((Point *) (graph[0])->points->data)->x,
                                ((Point *) (graph[0])->points->data)->y,
                                0);
            }
            graph_add(graph[1], t, sdata->atoms[1]);
            if ((graph[1])->points->prev != NULL) {
                graph_draw_line(darea[1], coord,
                                ((Point *) (graph[1])->points->prev->data)->x,
                                ((Point *) (graph[1])->points->prev->data)->y,
                                ((Point *) (graph[1])->points->data)->x,
                                ((Point *) (graph[1])->points->data)->y,
                                1);
            }
            graph_add(graph[2], t, sdata->atoms[2]);
            if ((graph[2])->points->prev != NULL) {
                graph_draw_line(darea[1], coord,
                                ((Point *) (graph[2])->points->prev->data)->x,
                                ((Point *) (graph[2])->points->prev->data)->y,
                                ((Point *) (graph[2])->points->data)->x,
                                ((Point *) (graph[2])->points->data)->y,
                                2);
            }

            tnext += tstep;
        }
        status_update_time(label_time, t); 

        /* wenn irgendwelche Toolkit-Operationen anstehen, dann ist jetzt
           Gelegenheit dazu, diese auszuführen */
        while (gtk_events_pending())
            gtk_main_iteration();
    }

    /* deaktiviert den Stop-Button wieder */
    g_signal_handlers_disconnect_matched(G_OBJECT(button_stop),
                                         G_SIGNAL_MATCH_FUNC,
                                         0, 0, NULL,
                                         (gpointer) stop_sim,
                                         NULL);
    gtk_widget_set_sensitive(button_stop, FALSE);


    /* ersetzt den Pause-Button wieder durch den Start-button */
    if (timer_is_running(timer)) {
        g_signal_handlers_disconnect_matched(G_OBJECT(button_start),
                                             G_SIGNAL_MATCH_FUNC,
                                             0, 0, NULL,
                                             (gpointer) pause_sim,
                                             NULL);
    } else {
        g_signal_handlers_disconnect_matched(G_OBJECT(button_start),
                                             G_SIGNAL_MATCH_FUNC,
                                             0, 0, NULL,
                                             (gpointer) resume_sim,
                                             NULL);
    }
    g_signal_handlers_unblock_by_func(G_OBJECT(button_start), (gpointer) sim_decay, rand);
    gtk_button_set_label(GTK_BUTTON(button_start), _("start"));

    /* trennt die Callbackfunktionen wieder  von den Zeichenbereichen */
    g_signal_handler_disconnect(G_OBJECT(darea[0]), sig_darea[0]);
    g_signal_handler_disconnect(G_OBJECT(darea[1]), sig_darea[1]);

    /* stellt den Speicher der erstellten Objekte wieder zur Verfügung */
    timer_free(timer);
    afield_free(afield);
    coord_system_free(coord);
}

/* fährt mit der Simulation fort */
static void resume_sim(GtkWidget *button, MyTimer *timer)
{
    timer_start(timer);
    g_signal_handlers_disconnect_by_func(G_OBJECT(button),
                                         (gpointer) resume_sim, timer);
    g_signal_connect(G_OBJECT(button), "clicked",
                     G_CALLBACK(pause_sim), timer);
    gtk_button_set_label(GTK_BUTTON(button), _("pause"));
}

/* pausiert die Simulation */
static void pause_sim(GtkWidget *button, MyTimer *timer)
{
    timer_stop(timer);
    g_signal_handlers_disconnect_by_func(G_OBJECT(button), (gpointer) pause_sim, timer);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(resume_sim), timer);
    gtk_button_set_label(GTK_BUTTON(button), _("resume"));
}

/* stoppt die Simulation */
static void stop_sim(GtkWidget *button, gboolean *quit)
{
    IGNORE(button);
    *quit = TRUE;
}

/* berechnet die Dauer eines Zerfalls */
static gdouble calc_duration(gint number, gdouble thalf)
{
    return -thalf * log2(1.0 / number) + thalf;
}

/* die exponentielle Wachstumsfunktion */
static gdouble exp_growth(gdouble t, SimData *data)
{
    return (gint) (data->atoms[0] * pow(0.5, (t / data->thalf[0])) + 0.5);
}

/* FIXME */
static gint decay_stat(gdouble t, gint n, gint n0, gdouble thalf)
{
    return n - (gint) ((n0 * pow(0.5, t / thalf)) + 0.5);
}

/* berechnet über die Poisson-Verteilung, wieviele von den n Atomen
   mit der Halbwertszeit thalf im Zeitraum t zerfallen */
static gint decay_poisson(gdouble t, gint n, gdouble thalf, gsl_rng *rand)
{
   /* return (gsl_ran_binomial(rand, (1.0 - pow(0.5, (t / thalf))), n)); */
   return gsl_ran_poisson(rand, ((1.0 - pow(0.5, (t / thalf)))) * n);
}
