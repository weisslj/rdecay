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

#include "sim.h"
#include "opt.h"
#include "atoms.h"
#include "afield.h"
#include "graph.h"
#include "ui_afield.h"
#include "ui_graph.h"
#include "darea.h"
#include "status.h"
#include "timer.h"
#include "util.h"
#include "gui.h"

#include <gtk/gtk.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

enum { GRAPH_NUMBER, GRAPH_ACTIVITY };

static void get_sim_input(GtkWidget *top, SimData *data);

static void resume_sim(GtkWidget *button, MyTimer *timer);
static void pause_sim(GtkWidget *button, MyTimer *timer);
static void stop_sim(GtkWidget *button, gboolean *quit);

static void change_speed(GtkWidget *scale, MyTimer *timer);

static gdouble decay_stat_n1(gdouble t, SimData *data);
static gdouble decay_stat_n2_of_2(gdouble t, SimData *data);
static gdouble decay_stat_n2_of_3(gdouble t, SimData *data);
static gdouble decay_stat_n3_of_3(gdouble t, SimData *data);

static gdouble decay_stat_a1(gdouble t, SimData *data);
static gdouble decay_stat_a2_of_3(gdouble t, SimData *data);

static gdouble calc_duration(gulong number, gdouble thalf);

static gint decay_poisson(gdouble t, gulong n, gdouble thalf, gsl_rng *rand);

/* startet die Simulation */
void sim_decay(GtkWidget *button_start, SimData *sdata)
{
    GtkWidget *top, **darea, *button_stop,
              *spin_number, **spin_htime,
              **label_atom, *label_time,
              *spin_states, **radio_graph_type,
              **check_graph_real, **check_graph_stat,
              **check_graph_step,
              *scale_speed, **menu_htime;
    GdkGC **style_graph_real, **style_graph_stat;
    gdouble t, tstart, tnext, told, tstep, tloop, activities[ATOM_STATES - 1],
            tnext_graph, told_graph, tstep_graph,tloop_graph;
    gint i, state, graph_type;
    gulong number, decays[ATOM_STATES], decays_buf[ATOM_STATES];
    gboolean quit, changed, showfps;
    gpointer func;

    Graph **graph_number_real, **graph_number_stat, **graph_number_step,
          **graph_activity_real, **graph_activity_stat, **graph_activity_step;
    MyTimer *timer;

    /* holt ein paar gespeicherte Widgets */
    top = gtk_widget_get_toplevel(button_start);
    button_stop = g_object_get_data(G_OBJECT(top), "button_stop");
    spin_number = g_object_get_data(G_OBJECT(top), "spin_number");
    spin_htime = g_object_get_data(G_OBJECT(top), "spin_htime");
    menu_htime = g_object_get_data(G_OBJECT(top), "menu_htime");
    darea = g_object_get_data(G_OBJECT(top), "darea");
    label_atom = g_object_get_data(G_OBJECT(top), "label_atom");
    label_time = g_object_get_data(G_OBJECT(top), "label_time");
    scale_speed = g_object_get_data(G_OBJECT(top), "scale_speed");
    spin_states = g_object_get_data(G_OBJECT(top), "spin_states");
    radio_graph_type = g_object_get_data(G_OBJECT(top), "radio_graph_type");
    check_graph_real = g_object_get_data(G_OBJECT(top), "check_graph_real");
    check_graph_stat = g_object_get_data(G_OBJECT(top), "check_graph_stat");
    check_graph_step = g_object_get_data(G_OBJECT(top), "check_graph_step");
    style_graph_real = g_object_get_data(G_OBJECT(top), "style_graph_real");
    style_graph_stat = g_object_get_data(G_OBJECT(top), "style_graph_stat");

    /* FIXME */
    get_sim_input(top, sdata);
    showfps = opt_get_showfps();
    number = sdata->atoms[0];
    graph_type = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_graph_type[0])) ? GRAPH_NUMBER : GRAPH_ACTIVITY;

    /* erstellt, wenn nötig, das Atomfeld, oder passt es an */
    if (sdata->afield == NULL) {
        sdata->afield = afield_new(sdata->atoms[0], darea[0]);
        g_signal_connect(G_OBJECT(darea[0]), "configure_event",
                                  G_CALLBACK(afield_resize), sdata->afield);
    } else {
        afield_reset(sdata->afield, sdata->atoms[0]);
        afield_arrange(sdata->afield, darea[0]);
    }
    if (!sdata->afield->uniform)
        afield_randomize(sdata->afield, sdata->rand);

    /* zeichnet das Atomfeld auf den Zeichenbereich */
    darea_clear(darea[0]);
/*        afield_benchmark(darea[0], sdata->afield, 1.0)); */
    if (!sdata->afield->uniform)
        afield_draw(darea[0], sdata->afield);
    else
        afield_tint(darea[0], sdata->afield, sdata->atoms, sdata->states);


    /* erstellt, wenn nötig, neue Koordinatensysteme */
    if (sdata->coord_number == NULL) {
        sdata->coord_number = coord_system_new(darea[1], "t", "s", "N", "", TRUE, FALSE,
                                               0, calc_duration(sdata->atoms[0], sdata->thalf[0]),
                                               0, sdata->atoms[0]);
        g_signal_connect(G_OBJECT(radio_graph_type[0]), "toggled",
                                  G_CALLBACK(coord_system_store), sdata->coord_number);

    } else {
        coord_system_adjust(sdata->coord_number, darea[1],
                            0, calc_duration(sdata->atoms[0], sdata->thalf[0]),
                            0, sdata->atoms[0]);
    }
        
    if (sdata->coord_activity == NULL) {
        sdata->coord_activity = coord_system_new(darea[1], "t", "s", "A", "Bq", TRUE, TRUE,
                                                 0, calc_duration(sdata->atoms[0], sdata->thalf[0]),
                                                 0, decay_stat_a1(0, sdata));
        g_signal_connect(G_OBJECT(radio_graph_type[1]), "toggled",
                                  G_CALLBACK(coord_system_store), sdata->coord_activity);
    } else {
        coord_system_adjust(sdata->coord_activity, darea[1],
                            0, calc_duration(sdata->atoms[0], sdata->thalf[0]),
                            0, decay_stat_a1(0, sdata));
    }

    /* verankert das richtige Koordinatensystem im Zeichenbereich
       und zeichnet es */
    darea_clear(darea[1]);
    if (graph_type == GRAPH_NUMBER) {
        g_object_set_data(G_OBJECT(darea[1]), "coord", sdata->coord_number);
        coord_system_draw(darea[1], sdata->coord_number);
    } else {
        g_object_set_data(G_OBJECT(darea[1]), "coord", sdata->coord_activity);
        coord_system_draw(darea[1], sdata->coord_activity);
    }

    /* reserviert Speicherplatz für die Graphen */
    graph_number_real = (Graph **) g_malloc(sdata->states * sizeof(Graph *));
    graph_number_stat = (Graph **) g_malloc(sdata->states * sizeof(Graph *));
    graph_number_step = (Graph **) g_malloc(sdata->states * sizeof(Graph *));
    graph_activity_real = (Graph **) g_malloc((sdata->states - 1) * sizeof(Graph *));
    graph_activity_stat = (Graph **) g_malloc((sdata->states - 1) * sizeof(Graph *));
    graph_activity_step = (Graph **) g_malloc((sdata->states - 1) * sizeof(Graph *));

    /* beginnt die Graphen des realen Zerfall */
    graph_number_real[0] = graph_new(style_graph_real[0], gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_real[0])), 0, sdata->atoms[0]);
    for (i = 1; i < sdata->states; i++)
        graph_number_real[i] = graph_new(style_graph_real[i], gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_real[i])), 0, 0);

    for (i = 0; i < (sdata->states - 1); i++)
        graph_activity_real[i] = graph_new(style_graph_real[i], gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_real[i])), 0, 0);

    /* setzt die FIXME */
    tstep_graph = fmax_n(sdata->states - 1, sdata->thalf) / 100.0;

    /* erstellt die Graphen des statistischen Zerfalls */
    graph_number_stat[0] = graph_new_by_func((GraphFunc) decay_stat_n1, sdata, style_graph_stat[0],
                                             gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_stat[0])), 0,
                                             calc_duration(sdata->atoms[0], sdata->thalf[0]), tstep_graph);
    graph_number_step[0] = graph_step_new_by_func((GraphFunc) decay_stat_n1, sdata, style_graph_stat[0],
                                                  gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_step[0])), 0,
                                                  calc_duration(sdata->atoms[0], sdata->thalf[0]), sdata->thalf[0]);
    if (sdata->states == 2) {
        graph_number_stat[1] = graph_new_by_func((GraphFunc) decay_stat_n2_of_2, sdata, style_graph_stat[1],
                                                 gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_stat[1])), 0,
                                                 calc_duration(sdata->atoms[0], sdata->thalf[0]), tstep_graph);
        graph_number_step[1] = graph_step_new_by_func((GraphFunc) decay_stat_n2_of_2, sdata, style_graph_stat[1],
                                                      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_step[1])), 0,
                                                      calc_duration(sdata->atoms[0], sdata->thalf[0]), sdata->thalf[0]);
    } else {
        graph_number_stat[1] = graph_new_by_func((GraphFunc) decay_stat_n2_of_3, sdata, style_graph_stat[1],
                                                 gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_stat[1])), 0,
                                                 calc_duration(sdata->atoms[0], sdata->thalf[0]), tstep_graph);
        graph_number_step[1] = graph_step_new_by_func((GraphFunc) decay_stat_n2_of_3, sdata, style_graph_stat[1],
                                                      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_step[1])), 0,
                                                      calc_duration(sdata->atoms[0], sdata->thalf[0]), sdata->thalf[1]);
        graph_number_stat[2] = graph_new_by_func((GraphFunc) decay_stat_n3_of_3, sdata, style_graph_stat[2],
                                                 gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_stat[2])), 0,
                                                 calc_duration(sdata->atoms[0], sdata->thalf[0]), tstep_graph);
        graph_number_step[2] = graph_step_new_by_func((GraphFunc) decay_stat_n3_of_3, sdata, style_graph_stat[2],
                                                      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_step[2])), 0,
                                                      calc_duration(sdata->atoms[0], sdata->thalf[0]), sdata->thalf[1]);
    }

    /* FIXME */
    graph_activity_stat[0] = graph_new_by_func((GraphFunc) decay_stat_a1, sdata, style_graph_stat[0],
                                               gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_stat[0])), 0,
                                               calc_duration(sdata->atoms[0], sdata->thalf[0]), tstep_graph);
    graph_activity_step[0] = graph_step_new_by_func((GraphFunc) decay_stat_a1, sdata, style_graph_stat[0],
                                                    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_step[0])), 0,
                                                    calc_duration(sdata->atoms[0], sdata->thalf[0]), sdata->thalf[0]);
    if (sdata->states == 3) {
        graph_activity_stat[1] = graph_new_by_func((GraphFunc) decay_stat_a2_of_3, sdata, style_graph_stat[1],
                                                   gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_stat[1])), 0,
                                                   calc_duration(sdata->atoms[0], sdata->thalf[0]), tstep_graph);
        graph_activity_step[1] = graph_step_new_by_func((GraphFunc) decay_stat_a2_of_3, sdata, style_graph_stat[1],
                                                        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_graph_step[1])), 0,
                                                        calc_duration(sdata->atoms[0], sdata->thalf[0]), sdata->thalf[1]);
    }

    if (graph_type == GRAPH_NUMBER)
        for (i = 0; i < sdata->states; i++) {
            if ((graph_number_stat[i])->active)
                graph_draw(graph_number_stat[i], darea[1], sdata->coord_number);
            if ((graph_number_step[i])->active)
                graph_draw(graph_number_step[i], darea[1], sdata->coord_number);
        }
    else
        for (i = 0; i < (sdata->states - 1); i++) {
            if ((graph_activity_stat[i])->active)
                graph_draw(graph_activity_stat[i], darea[1], sdata->coord_activity);
            if ((graph_activity_step[i])->active)
                graph_draw(graph_activity_step[i], darea[1], sdata->coord_activity);
        }

    /* verankert die Graphen im entsprechenden Koordinatensystem */
    coord_system_clear(sdata->coord_number);
    coord_system_clear(sdata->coord_activity);
    for (i = 0; i < sdata->states; i++) {
        coord_system_add_graph(sdata->coord_number, graph_number_real[i]);
        coord_system_add_graph(sdata->coord_number, graph_number_stat[i]);
        coord_system_add_graph(sdata->coord_number, graph_number_step[i]);
    }
    for (i = 0; i < (sdata->states - 1); i++) {
        coord_system_add_graph(sdata->coord_activity, graph_activity_real[i]);
        coord_system_add_graph(sdata->coord_activity, graph_activity_stat[i]);
        coord_system_add_graph(sdata->coord_activity, graph_activity_step[i]);
    }


    /* trennt die Callbackfunktionen wieder von den Auswahlkästen */
    for (i = 0; i < sdata->states; i++) {
        g_signal_handlers_disconnect_matched(G_OBJECT(check_graph_real[i]), G_SIGNAL_MATCH_FUNC, 0, 0, NULL, (gpointer) graph_toggle, NULL);
        g_signal_handlers_disconnect_matched(G_OBJECT(check_graph_stat[i]), G_SIGNAL_MATCH_FUNC, 0, 0, NULL, (gpointer) graph_redraw, NULL);
    }
    /* verbindet die Auswahlboxen für die Graphen mit Callback-Funktionen */
    for (i = 0; i < sdata->states; i++) {
        g_signal_connect(G_OBJECT(check_graph_real[i]), "toggled", G_CALLBACK(graph_toggle), graph_number_real[i]);
        g_signal_connect(G_OBJECT(check_graph_stat[i]), "toggled", G_CALLBACK(graph_toggle), graph_number_stat[i]);
    }
    for (i = 0; i < (sdata->states - 1); i++) {
        g_signal_connect(G_OBJECT(check_graph_real[i]), "toggled", G_CALLBACK(graph_toggle), graph_activity_real[i]);
        g_signal_connect(G_OBJECT(check_graph_stat[i]), "toggled", G_CALLBACK(graph_toggle), graph_activity_stat[i]);
    }
    for (i = 0; i < sdata->states; i++) {
        g_signal_connect_swapped(G_OBJECT(check_graph_real[i]), "toggled", G_CALLBACK(graph_redraw), darea[1]);
        g_signal_connect_swapped(G_OBJECT(check_graph_stat[i]), "toggled", G_CALLBACK(graph_redraw), darea[1]);
    }

    /* FIXME */
    for (i = 0; i < sdata->states; i++)
        decays_buf[i] = 0;


    /* setzt die Status-Anzeigen auf die Ausgangswerte */
    status_update_atoms(label_atom, sdata->atoms);
    status_update_time(label_time, 0.0); 

    /* ersetzt den Start-Button durch den Pausebutton */
    g_signal_handlers_block_by_func(G_OBJECT(button_start), (gpointer) sim_decay, sdata);
    gtk_button_set_label(GTK_BUTTON(button_start), _("pause"));
    gtk_button_leave(GTK_BUTTON(button_start));

    /* bereitet den Stop-Button vor */
    quit = FALSE;
    gtk_widget_set_sensitive(button_stop, TRUE);

    /* setzt die Aktualisierungsrate auf FIXME */
    if (opt_get_fps() == 0.0)
        tstep = 0.0;
    else
        tstep = 1.0 / opt_get_fps();

    /* startet den timer */
    timer = timer_new(scale_get_speed(scale_speed));

    /* verknüpft den Pause- und Stop-Button mit Callback-Funktionen */
    g_signal_connect(G_OBJECT(button_start), "clicked", G_CALLBACK(pause_sim), timer);
    g_signal_connect(G_OBJECT(button_stop), "clicked", G_CALLBACK(stop_sim), &quit);
    g_signal_connect(G_OBJECT(scale_speed), "value-changed", G_CALLBACK(change_speed), timer);

    /* setzt Zeitvariablen auf die (wenige) bisher vergangene Zeit */
    tstart = tnext = told = tnext_graph = told_graph = timer_elapsed(timer);

    /* der Versuch läuft solange, bis alle Atome in den letzten Zustand
       gekommen sind, oder der Nutzer den Stop-Button drückt */
    while (sdata->atoms[sdata->states-1] < number && (!quit)) {
        t = timer_elapsed(timer) - tstart;
        if (t >= tnext) {
            tloop = t - told;
            told = t;

            if (showfps)
                printf("fps: %.2f\n", 1.0 / tloop);

            changed = FALSE;
            for (state = sdata->states - 2; state >= 0; state--) {
                decays[state] = (sdata->atoms[state] > 0) ? decay_poisson(tloop, sdata->atoms[state], sdata->thalf[state], sdata->rand) : 0;
                if (decays[state] > 0)
                    changed = TRUE;
                decays_buf[state] += decays[state];
            }

            if (changed) {
                for (state = sdata->states - 2; state >= 0; state--) {
                    if (sdata->afield->uniform)
                        afield_tint(darea[0], sdata->afield, sdata->atoms, sdata->states);
                    else
                        afield_distrib_decays(darea[0], sdata->afield, decays[state], state);

                    sdata->atoms[state + 0] -= decays[state];
                    sdata->atoms[state + 1] += decays[state];
                }

                status_update_atoms(label_atom, sdata->atoms);

                
            }

            tnext += tstep;
        }

        if (t >= tnext_graph) {
            tloop_graph = t - told_graph;
            told_graph = t;

            for (i = 0; i < (sdata->states - 1); i++) {
                activities[i] = decays_buf[i] / tloop_graph;
                decays_buf[i] = 0;
            }

/*            graph_add(graph_number_real[0], t, sdata->atoms[0] / (gdouble) sdata->atoms[1]); */
            graph_add(graph_number_real[0], t, sdata->atoms[0]);
            for (state = 1; state < sdata->states; state++)
                graph_add(graph_number_real[state], t, sdata->atoms[state]);
            for (state = 0; state < (sdata->states - 1); state++)
                graph_add(graph_activity_real[state], t, activities[state]);

            darea_update(darea[0]);
            graph_update(darea[1]);
            darea_update(darea[1]);

            tnext_graph += tstep_graph;
        }
        
        status_update_time(label_time, t); 

        /* wenn irgendwelche Toolkit-Operationen anstehen, dann ist jetzt
           Gelegenheit dazu, diese auszuführen */
        while (gtk_events_pending())
            gtk_main_iteration();
    }

    /* ersetzt den Pause-Button wieder durch den Start-button */
    func = (gpointer) ((timer_is_running(timer)) ? pause_sim : resume_sim);
    g_signal_handlers_disconnect_matched(G_OBJECT(button_start),
                                         G_SIGNAL_MATCH_FUNC,
                                         0, 0, NULL,
                                         func,
                                         NULL);

    /* FIXME */
    g_signal_handlers_disconnect_matched(G_OBJECT(scale_speed),
                                         G_SIGNAL_MATCH_FUNC,
                                         0, 0, NULL,
                                         (gpointer) change_speed,
                                         NULL);

    /* stoppt den Timer */
    timer_stop(timer);

    /* deaktiviert den Stop-Button */
    g_signal_handlers_disconnect_matched(G_OBJECT(button_stop),
                                         G_SIGNAL_MATCH_FUNC,
                                         0, 0, NULL,
                                         (gpointer) stop_sim,
                                         NULL);
    gtk_widget_set_sensitive(button_stop, FALSE);

    /* bereitet den Start-Button auf neue Simulation vor */
    gtk_button_set_label(GTK_BUTTON(button_start), _("start"));
    g_signal_handlers_unblock_by_func(G_OBJECT(button_start), (gpointer) sim_decay, sdata);


    /* stellt den Speicher der erstellten Objekte wieder zur Verfügung */
    g_free(graph_number_real);
    g_free(graph_number_stat);
    g_free(graph_number_step);
    g_free(graph_activity_real);
    g_free(graph_activity_stat);
    g_free(graph_activity_step);
    timer_destroy(timer);
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
static void stop_sim(GtkWidget *button_stop, gboolean *quit)
{
    IGNORE(button_stop);
    *quit = TRUE;
}

static void change_speed(GtkWidget *scale, MyTimer *timer)
{
    timer_set_speed(timer, scale_get_speed(scale));
}

/* berechnet die Dauer eines Zerfalls */
static gdouble calc_duration(gulong number, gdouble thalf)
{
    return -thalf * log2(1.0 / number) + 2 * thalf;
}

/* die exponentielle Wachstumsfunktion */
static gdouble decay_stat_n1(gdouble t, SimData *data)
{
    return (gulong) ((data->atoms[0] * pow(0.5, (t / data->thalf[0]))) + 0.5);
}

static gdouble decay_stat_n2_of_2(gdouble t, SimData *data)
{
    return (gulong) ((data->atoms[0] * (1 - pow(0.5, (t / data->thalf[0])))) + 0.5);
}

static gdouble decay_stat_n2_of_3(gdouble t, SimData *data)
{
    return (gulong) ((data->atoms[0] * ((data->thalf[1] / (data->thalf[0] - data->thalf[1])) * (pow(0.5, (t / data->thalf[0])) - pow(0.5, (t / data->thalf[1]))))) + 0.5);
}

static gdouble decay_stat_n3_of_3(gdouble t, SimData *data)
{
    return (gulong) ((data->atoms[0] * (1 - ((data->thalf[0] * pow(0.5, (t / data->thalf[0])) - data->thalf[1] * pow(0.5, (t / data->thalf[1]))) / (data->thalf[0] - data->thalf[1])))) + 0.5);
}

static gdouble decay_stat_a1(gdouble t, SimData *data)
{
    return data->atoms[0] * pow(0.5, (t / data->thalf[0])) * (log(2) / data->thalf[0]);
}

static gdouble decay_stat_a2_of_3(gdouble t, SimData *data)
{
    return data->atoms[0] * ((log(2) / (data->thalf[0] - data->thalf[1])) * (pow(0.5, (t / data->thalf[0])) - pow(0.5, (t / data->thalf[1]))));
}

/* berechnet über die Poisson-Verteilung, wieviele von den n Atomen
   mit der Halbwertszeit thalf im Zeitraum t zerfallen */
static gint decay_poisson(gdouble t, gulong n, gdouble thalf, gsl_rng *rand)
{
    gulong x;
   /* return (gsl_ran_binomial(rand, (1.0 - pow(0.5, (t / thalf))), n)); */
    x = gsl_ran_poisson(rand, ((1.0 - pow(0.5, (t / thalf)))) * n);
   return (x > n) ? n : x;
}

static void get_sim_input(GtkWidget *top, SimData *data)
{
    GtkWidget *spin_states, *spin_number, **spin_htime, **menu_htime;
    gint i;

    spin_states = g_object_get_data(G_OBJECT(top), "spin_states");
    spin_number = g_object_get_data(G_OBJECT(top), "spin_number");
    spin_htime = g_object_get_data(G_OBJECT(top), "spin_htime");
    menu_htime = g_object_get_data(G_OBJECT(top), "menu_htime");

    /* holt die Eingaben des Nutzers */
    data->states = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_states));
    data->atoms[0] = (gulong) gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_number));
    for (i = 1; i < ATOM_STATES ; i++)
        data->atoms[i] = 0;
    for (i = 0; i < (data->states - 1); i++)
        data->thalf[i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_htime[i])) * GPOINTER_TO_INT(g_object_get_data(G_OBJECT(gtk_menu_get_active(GTK_MENU(gtk_option_menu_get_menu(GTK_OPTION_MENU(menu_htime[i]))))), "factor"));
}
