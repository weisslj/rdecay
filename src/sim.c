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

#define decay_stat_n(a, g) \
((a == 1) ? decay_stat_n1 : \
(g == 2) ? decay_stat_n2_of_2 : \
(g == 3) ? ((a == 2) ? decay_stat_n2_of_3 : decay_stat_n3_of_3) : \
decay_stat_math_is_too_complicated)

#define decay_stat_a(a, g) \
((a == 1) ? decay_stat_a1 : \
(g == 3) ? decay_stat_a2_of_3 : \
decay_stat_math_is_too_complicated)

enum { SIM_STOP = 1, SIM_QUIT };

static void get_sim_input(GtkWidget *top, SimData *data);

static void resume_sim(GtkWidget *button, MyTimer *timer);
static void pause_sim(GtkWidget *button, MyTimer *timer);
static void stop_sim(GtkWidget *button, gint *leave);
static void quit_sim(GtkWidget *button_quit, gint *leave);

static void change_speed(GtkWidget *scale, MyTimer *timer);

static gdouble decay_stat_n1(gdouble t, SimData *data);
static gdouble decay_stat_n2_of_2(gdouble t, SimData *data);
static gdouble decay_stat_n2_of_3(gdouble t, SimData *data);
static gdouble decay_stat_n3_of_3(gdouble t, SimData *data);

static gdouble decay_stat_a1(gdouble t, SimData *data);
static gdouble decay_stat_a2_of_3(gdouble t, SimData *data);

static gdouble decay_stat_math_is_too_complicated(gdouble t, SimData *data);

static gdouble calc_duration(SimData *data);

static gint decay_binomial(gdouble t, gulong n,
                           gdouble thalf, gsl_rng *rand);

/* erstellt die Simulationsstruktur */
SimData *simdata_new(gsl_rng *rand)
{
    SimData *sdata;

    sdata = (SimData *) g_malloc(sizeof(SimData));

    sdata->afield = NULL;
    sdata->coord_number = NULL;
    sdata->coord_activity = NULL;
    sdata->rand = rand;

    sdata->atoms = (gulong *) g_malloc(ATOM_STATES * sizeof(gulong));
    sdata->thalf = (gdouble *)
                   g_malloc((ATOM_STATES - 1) * sizeof(gdouble));

    return sdata;
}

/* stellt den Speicher der Simulationsstruktur
   wieder zur Verfügung */
void simdata_free(SimData *sdata)
{
    /* FIXME */
    g_free(sdata);
}

/* startet die Simulation */
void sim_decay(GtkWidget *button_start, SimData *sdata)
{
    GtkWidget *top, **darea, *button_stop, *button_quit,
              *spin_number, **spin_htime,
              **label_atom, *label_time,
              *spin_states, **radio_graph_type,
              **check_graph_real, **check_graph_stat,
              **check_graph_step, **progress_atom,
              *scale_speed, **menu_htime;
    GdkGC **style_graph_real, **style_graph_stat;
    gdouble t, tstart, tnext, told, tstep, tloop,
            activities[ATOM_STATES - 1],
            tnext_graph, told_graph, tstep_graph,
            tloop_graph, duration, graph_step_rate,
            thalf_max;
    gint i, state, leave;
    gint (*decay_real)(gdouble, gulong, gdouble, gsl_rng *);
    gulong number, decays[ATOM_STATES], decays_buf[ATOM_STATES];
    gboolean changed, showfps;
    gpointer func;

    Graph **graph_number_real, **graph_number_stat, **graph_number_step,
          **graph_activity_real, **graph_activity_stat,
          **graph_activity_step;
    MyTimer *timer;

    /* holt ein paar gespeicherte Widgets */
    top = gtk_widget_get_toplevel(button_start);
    button_stop = g_object_get_data(G_OBJECT(top), "button_stop");
    button_quit = g_object_get_data(G_OBJECT(top), "button_quit");
    spin_number = g_object_get_data(G_OBJECT(top), "spin_number");
    spin_htime = g_object_get_data(G_OBJECT(top), "spin_htime");
    menu_htime = g_object_get_data(G_OBJECT(top), "menu_htime");
    darea = g_object_get_data(G_OBJECT(top), "darea");
    label_atom = g_object_get_data(G_OBJECT(top), "label_atom");
    label_time = g_object_get_data(G_OBJECT(top), "label_time");
    progress_atom = g_object_get_data(G_OBJECT(top), "progress_atom");
    scale_speed = g_object_get_data(G_OBJECT(top), "scale_speed");
    spin_states = g_object_get_data(G_OBJECT(top), "spin_states");
    radio_graph_type = g_object_get_data(G_OBJECT(top), "radio_graph_type");
    check_graph_real = g_object_get_data(G_OBJECT(top), "check_graph_real");
    check_graph_stat = g_object_get_data(G_OBJECT(top), "check_graph_stat");
    check_graph_step = g_object_get_data(G_OBJECT(top), "check_graph_step");
    style_graph_real = g_object_get_data(G_OBJECT(top), "style_graph_real");
    style_graph_stat = g_object_get_data(G_OBJECT(top), "style_graph_stat");

    /* holt Einstellungen */
    get_sim_input(top, sdata);
    showfps = opt_get_showfps();
    number = sdata->atoms[0];
    duration = calc_duration(sdata);

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
    if (!sdata->afield->uniform)
        afield_draw(darea[0], sdata->afield);
    else
        afield_tint(darea[0], sdata->afield, sdata->atoms, sdata->states);
    darea_update(darea[0]);


    /* erstellt, wenn nötig, neue Koordinatensysteme */
    if (sdata->coord_number == NULL) {
        sdata->coord_number = coord_system_new(darea[1],
            "t", "s", "N", "", TRUE, FALSE,
            0, duration, 0, sdata->atoms[0]);
        g_signal_connect(G_OBJECT(radio_graph_type[0]), "toggled",
                         G_CALLBACK(coord_system_store),
                         sdata->coord_number);
    } else
        coord_system_adjust(sdata->coord_number, darea[1],
            0, duration, 0, sdata->atoms[0]);

    if (sdata->coord_activity == NULL) {
        sdata->coord_activity = coord_system_new(darea[1],
            "t", "s", "A", "Bq", TRUE, TRUE,
            0, duration, 0, decay_stat_a1(0, sdata));
        g_signal_connect(G_OBJECT(radio_graph_type[1]), "toggled",
                         G_CALLBACK(coord_system_store),
                         sdata->coord_activity);
    } else
        coord_system_adjust(sdata->coord_activity, darea[1],
                            0, duration,
                            0, decay_stat_a1(0, sdata));

    /* verankert das richtige Koordinatensystem im Zeichenbereich
       und zeichnet es */
    darea_clear(darea[1]);
    if (gtk_toggle_button_get_active(
                GTK_TOGGLE_BUTTON(radio_graph_type[0]))) {
        g_object_set_data(G_OBJECT(darea[1]), "coord",
                          sdata->coord_number);
        coord_system_draw(darea[1], sdata->coord_number);
    } else {
        g_object_set_data(G_OBJECT(darea[1]), "coord",
                          sdata->coord_activity);
        coord_system_draw(darea[1], sdata->coord_activity);
    }
    darea_update(darea[1]);


    /* reserviert Speicherplatz für die Graphen */
    graph_number_real = (Graph **)
                        g_malloc(sdata->states * sizeof(Graph *));
    graph_number_stat = (Graph **)
                        g_malloc(sdata->states * sizeof(Graph *));
    graph_number_step = (Graph **)
                        g_malloc(sdata->states * sizeof(Graph *));
    graph_activity_real = (Graph **)
                          g_malloc((sdata->states - 1) * sizeof(Graph *));
    graph_activity_stat = (Graph **)
                          g_malloc((sdata->states - 1) * sizeof(Graph *));
    graph_activity_step = (Graph **)
                          g_malloc((sdata->states - 1) * sizeof(Graph *));

    /* setzt die Aktualisierungsrate der Graphen ein */
    thalf_max = fmax_n(sdata->states - 1, sdata->thalf);
    tstep_graph = thalf_max / 100.0;
    graph_step_rate = (ABS(thalf_max - sdata->thalf[0]) > 100)
                      ? thalf_max : sdata->thalf[0];

    /* beginnt die Graphen des realen Zerfalls */
    graph_number_real[0] = graph_new(style_graph_real[0],
        gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(check_graph_real[0])),
        0, sdata->atoms[0]);
    for (i = 1; i < sdata->states; i++)
        graph_number_real[i] = graph_new(style_graph_real[i],
            gtk_toggle_button_get_active(
                GTK_TOGGLE_BUTTON(check_graph_real[i])),
            0, 0);
    for (i = 0; i < (sdata->states - 1); i++)
        graph_activity_real[i] = graph_new(style_graph_real[i],
            gtk_toggle_button_get_active(
                GTK_TOGGLE_BUTTON(check_graph_real[i])),
            0, 0);

    /* erstellt die Graphen des statistischen Zerfalls */
    for (i = 0; i < sdata->states; i++) {
        graph_number_stat[i] = graph_new_by_func(
            (GraphFunc) decay_stat_n(i + 1, sdata->states), sdata,
            style_graph_stat[i], gtk_toggle_button_get_active(
                GTK_TOGGLE_BUTTON(check_graph_stat[i])),
            0, duration, tstep_graph);
        graph_number_step[i] = graph_step_new_by_func(
            (GraphFunc) decay_stat_n(i + 1, sdata->states), sdata,
            style_graph_stat[i], gtk_toggle_button_get_active(
                GTK_TOGGLE_BUTTON(check_graph_step[i])),
            0, duration, graph_step_rate);
    }
    for (i = 0; i < (sdata->states - 1); i++) {
        graph_activity_stat[i] = graph_new_by_func(
            (GraphFunc) decay_stat_a(i + 1, sdata->states), sdata,
            style_graph_stat[i], gtk_toggle_button_get_active(
                GTK_TOGGLE_BUTTON(check_graph_stat[i])),
            0, duration, tstep_graph);
        graph_activity_step[i] = graph_step_new_by_func(
            (GraphFunc) decay_stat_a(i + 1, sdata->states), sdata,
            style_graph_stat[i], gtk_toggle_button_get_active(
                GTK_TOGGLE_BUTTON(check_graph_step[i])),
            0, duration, graph_step_rate);
    }


    /* verankert die Graphen im entsprechenden Koordinatensystem */
    coord_system_clear(sdata->coord_number);
    for (i = 0; i < sdata->states; i++) {
        coord_system_add_graph(sdata->coord_number,
                               graph_number_real[i]);
        coord_system_add_graph(sdata->coord_number,
                               graph_number_stat[i]);
        coord_system_add_graph(sdata->coord_number,
                               graph_number_step[i]);
    }
    coord_system_clear(sdata->coord_activity);
    for (i = 0; i < (sdata->states - 1); i++) {
        coord_system_add_graph(sdata->coord_activity,
                               graph_activity_real[i]);
        coord_system_add_graph(sdata->coord_activity,
                               graph_activity_stat[i]);
        coord_system_add_graph(sdata->coord_activity,
                               graph_activity_step[i]);
    }


    /* trennt die Callbackfunktionen wieder von den Auswahlkästen */
    for (i = 0; i < ATOM_STATES; i++) {
        g_signal_handlers_disconnect_matched(G_OBJECT(check_graph_real[i]),
            G_SIGNAL_MATCH_FUNC, 0, 0, NULL, (gpointer) graph_toggle, NULL);
        g_signal_handlers_disconnect_matched(G_OBJECT(check_graph_stat[i]),
            G_SIGNAL_MATCH_FUNC, 0, 0, NULL, (gpointer) graph_toggle, NULL);
        g_signal_handlers_disconnect_matched(G_OBJECT(check_graph_step[i]),
            G_SIGNAL_MATCH_FUNC, 0, 0, NULL, (gpointer) graph_toggle, NULL);

        g_signal_handlers_disconnect_matched(G_OBJECT(check_graph_real[i]),
            G_SIGNAL_MATCH_FUNC, 0, 0, NULL, (gpointer) graph_redraw, NULL);
        g_signal_handlers_disconnect_matched(G_OBJECT(check_graph_stat[i]),
            G_SIGNAL_MATCH_FUNC, 0, 0, NULL, (gpointer) graph_redraw, NULL);
        g_signal_handlers_disconnect_matched(G_OBJECT(check_graph_step[i]),
            G_SIGNAL_MATCH_FUNC, 0, 0, NULL, (gpointer) graph_redraw, NULL);
    }

    /* verbindet die Auswahlboxen für die Graphen
       mit Callback-Funktionen */
    for (i = 0; i < sdata->states; i++) {
        g_signal_connect(G_OBJECT(check_graph_real[i]), "toggled",
            G_CALLBACK(graph_toggle), graph_number_real[i]);
        g_signal_connect(G_OBJECT(check_graph_stat[i]), "toggled",
            G_CALLBACK(graph_toggle), graph_number_stat[i]);
        g_signal_connect(G_OBJECT(check_graph_step[i]), "toggled",
            G_CALLBACK(graph_toggle), graph_number_step[i]);
    }
    for (i = 0; i < (sdata->states - 1); i++) {
        g_signal_connect(G_OBJECT(check_graph_real[i]), "toggled",
            G_CALLBACK(graph_toggle), graph_activity_real[i]);
        g_signal_connect(G_OBJECT(check_graph_stat[i]), "toggled",
            G_CALLBACK(graph_toggle), graph_activity_stat[i]);
        g_signal_connect(G_OBJECT(check_graph_step[i]), "toggled",
            G_CALLBACK(graph_toggle), graph_activity_step[i]);
    }
    for (i = 0; i < sdata->states; i++) {
        g_signal_connect_swapped(G_OBJECT(check_graph_real[i]), "toggled",
            G_CALLBACK(graph_redraw), darea[1]);
        g_signal_connect_swapped(G_OBJECT(check_graph_stat[i]), "toggled",
            G_CALLBACK(graph_redraw), darea[1]);
        g_signal_connect_swapped(G_OBJECT(check_graph_step[i]), "toggled",
            G_CALLBACK(graph_redraw), darea[1]);
    }

    /* zeichnet alle ausgewählte Graphen */
    graph_redraw(darea[1]);

    /* setzt die Status-Anzeigen auf die Ausgangswerte */
    status_update_atoms(label_atom, progress_atom, sdata->atoms, number);
    status_update_time(label_time, 0.0); 

    /* Setzt den Zerfalls-Buffer auf 0 */
    for (i = 0; i < sdata->states; i++)
        decays_buf[i] = 0;

    /* ersetzt den Start-Button durch den Pause-Button */
    g_signal_handlers_block_by_func(G_OBJECT(button_start),
                                    (gpointer) sim_decay, sdata);
    gtk_button_set_label(GTK_BUTTON(button_start), _("pause"));
    gtk_button_leave(GTK_BUTTON(button_start));

    /* bereitet den Stop- und Beenden-Button vor */
    leave = FALSE;
    gtk_widget_set_sensitive(button_stop, TRUE);
    g_signal_connect(G_OBJECT(button_stop), "clicked",
                     G_CALLBACK(stop_sim), &leave);
    g_signal_handlers_block_matched(G_OBJECT(button_quit),
                                    G_SIGNAL_MATCH_FUNC,
                                    0, 0, NULL,
                                    (gpointer) gtk_widget_destroy,
                                    NULL);
    g_signal_connect(G_OBJECT(button_quit), "clicked",
                     G_CALLBACK(quit_sim), &leave);


    /* wählt die reale Zerfallsfunktion aus */
    decay_real = decay_binomial;

    /* setzt die Aktualisierungsrate auf 0
       oder den per Kommandozeile angegebenen Wert */
    tstep = (!opt_get_fps()) ? 0.0 : 1.0 / opt_get_fps();

    /* startet den Timer */
    timer = timer_new(scale_get_speed(scale_speed));

    /* verknüpft den Pause-Button und den Geschwindigkeitsregler
       mit Callback-Funktionen */
    g_signal_connect(G_OBJECT(button_start), "clicked",
                     G_CALLBACK(pause_sim), timer);
    g_signal_connect(G_OBJECT(scale_speed), "value-changed",
                     G_CALLBACK(change_speed), timer);

    /* setzt Zeitvariablen auf die (wenige) bisher vergangene Zeit */
    t = tstart = tnext = told =
    tnext_graph = told_graph =
    timer_elapsed(timer);


    /* der Versuch läuft solange, bis alle Atome in den letzten Zustand
       gekommen sind, oder der Nutzer den Stop-Button drückt */
    while (sdata->atoms[sdata->states-1] < number && (!leave)) {

        /* in t wird die seit dem Start der Simulation vergangene Zeit
           gespeichert */
        t = timer_elapsed(timer) - tstart;

        /* wenn die Aktualisierunsrate maximal sein soll, dann wird
           eventuell gewartet */
        if (t >= tnext) {

            /* tloop ist die Zeit eines Schleifendurchlaufs, zu
               ihrer Ermittlung wird told als Hilfsvariable
               eingesetzt */
            tloop = t - told;
            told = t;

            /* gibt eventuell die Schleifendurchläufe pro Sekunde aus */
            if (showfps)
                printf("fps: %.2f\n", 1.0 / tloop);

            /* noch ist nichts zerfallen */
            changed = FALSE;

            /* Überprüfe für jedes instabile Element ... */
            for (state = sdata->states - 2; state >= 0; state--) {

                /* ... ob es im Zeitraum tloop Zerfälle gab ... */
                decays[state] = (sdata->atoms[state] > 0)
                    ? decay_real(tloop, sdata->atoms[state],
                                 sdata->thalf[state], sdata->rand)
                    : 0;

                /* ... und speichere sie im Buffer. */
                decays_buf[state] += decays[state];

                /* setze changed auf TRUE, falls etwas zerfallen ist */
                if (decays[state] > 0)
                    changed = TRUE;
            }

            /* Wenn etwas zerfallen ist, dann ... */
            if (changed) {
                /* ... wird für jedes instabile Element ... */
                for (state = sdata->states - 2; state >= 0; state--) {

                    /* ... das Atomfeld aktualisiert ... */
                    if (sdata->afield->uniform)
                        afield_tint(darea[0], sdata->afield,
                                    sdata->atoms, sdata->states);
                    else
                        afield_distrib_decays(darea[0], sdata->afield,
                                              decays[state], state);
                    darea_update(darea[0]);

                    /* ... und die Mengenverhältnisse aktualisiert. */
                    sdata->atoms[state + 0] -= decays[state];
                    sdata->atoms[state + 1] += decays[state];
                }

                /* aktualisiert die Statusanzeige der Atome */
                status_update_atoms(label_atom, progress_atom,
                                    sdata->atoms, number);
            }

            /* dient zum Einhalten der Aktualisierungsrate */
            tnext += tstep;
        }

        /* für die Graphen gibt es aus Performance-Gründen eine
           eigene Aktualisierungsrate */
        if (t >= tnext_graph) {
            tloop_graph = t - told_graph;
            told_graph = t;

            /* berechnet die Aktivitäten */
            for (i = 0; i < (sdata->states - 1); i++) {
                activities[i] = decays_buf[i] / tloop_graph;
                decays_buf[i] = 0;
            }

            /* fügt neue Punkte zu den Graphen hinzu */
            for (state = 0; state < sdata->states; state++)
                graph_add(graph_number_real[state], t, sdata->atoms[state]);
            for (state = 0; state < (sdata->states - 1); state++)
                graph_add(graph_activity_real[state], t, activities[state]);

            /* zeichnet die jeweils letzten Punkte */
            graph_update(darea[1]);
            darea_update(darea[1]);

            /* s. oben */
            tnext_graph += tstep_graph;
        }

        /* aktualisiert die Zeitanzeige */
        status_update_time(label_time, t); 

        /* wenn irgendwelche Toolkit-Operationen anstehen, dann ist jetzt
           Gelegenheit dazu, diese auszuführen */
        g_main_context_iteration(NULL, FALSE);
    }

    /* Fügt die Endpunkte zum Graphen hinzu ... */
    for (state = 0; state < sdata->states; state++)
        graph_add(graph_number_real[state], t, sdata->atoms[state]);
    for (state = 0; state < (sdata->states - 1); state++)
        graph_add(graph_activity_real[state], t, activities[state]);

    /* ... und zeichnet sie. */
    graph_update(darea[1]);
    darea_update(darea[1]);

    /* ersetzt den Pause-Button wieder durch den Start-button */
    func = (gpointer) ((timer_is_running(timer)) ? pause_sim : resume_sim);
    g_signal_handlers_disconnect_matched(G_OBJECT(button_start),
                                         G_SIGNAL_MATCH_FUNC,
                                         0, 0, NULL,
                                         func,
                                         NULL);

    /* trennt den Geschwindigkeitsregler wieder von seiner Funktion */
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

    /* setzt den Beenden-Button wieder zurück */
    g_signal_handlers_disconnect_matched(G_OBJECT(button_quit),
                                         G_SIGNAL_MATCH_FUNC,
                                         0, 0, NULL,
                                         (gpointer) quit_sim,
                                         NULL);
    g_signal_handlers_unblock_matched(G_OBJECT(button_quit),
                                      G_SIGNAL_MATCH_FUNC,
                                      0, 0, NULL,
                                      (gpointer) gtk_widget_destroy,
                                      NULL);

    /* bereitet den Start-Button auf neue Simulation vor */
    gtk_button_set_label(GTK_BUTTON(button_start), _("start"));
    g_signal_handlers_unblock_by_func(G_OBJECT(button_start),
                                      (gpointer) sim_decay, sdata);


    /* stellt den Speicher der erstellten Objekte wieder zur Verfügung */
    g_free(graph_number_real);
    g_free(graph_number_stat);
    g_free(graph_number_step);
    g_free(graph_activity_real);
    g_free(graph_activity_stat);
    g_free(graph_activity_step);
    timer_destroy(timer);

    /* wenn der Beenden-Button gedrückt wurde, wird
       das GUI zerstört */
    if (leave == SIM_QUIT)
        gtk_widget_destroy(top);
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
    g_signal_handlers_disconnect_by_func(G_OBJECT(button),
        (gpointer) pause_sim, timer);
    g_signal_connect(G_OBJECT(button), "clicked",
        G_CALLBACK(resume_sim), timer);
    gtk_button_set_label(GTK_BUTTON(button), _("resume"));
}

/* stoppt die Simulation */
static void stop_sim(GtkWidget *button_stop, gint *leave)
{
    IGNORE(button_stop);
    *leave = SIM_STOP;
}

/* beendet die Simulation */
static void quit_sim(GtkWidget *button_quit, gint *leave)
{
    IGNORE(button_quit);
    *leave = SIM_QUIT;
}

/* ändert die Geschwindigkeit */
static void change_speed(GtkWidget *scale, MyTimer *timer)
{
    timer_set_speed(timer, scale_get_speed(scale));
}

/* berechnet die ungefähre Dauer des Vorgangs */
static gdouble calc_duration(SimData *data)
{
    gdouble thalf_max;

    thalf_max =  fmax_n(data->states - 1, data->thalf);

    return -  thalf_max * log2(1.0 / data->atoms[0]) + thalf_max;
}

static gdouble decay_stat_n1(gdouble t, SimData *data)
{
    return (gulong) ((data->atoms[0] *
                    pow(0.5, (t / data->thalf[0]))) + 0.5);
}

static gdouble decay_stat_n2_of_2(gdouble t, SimData *data)
{
    return (gulong) ((data->atoms[0] -
                    (data->atoms[0] * pow(0.5, (t / data->thalf[0])))) + 0.5);
}

static gdouble decay_stat_n2_of_3(gdouble t, SimData *data)
{
    return (gulong) (((data->atoms[0] * data->thalf[1] *
                    (pow(0.5, (t / data->thalf[0])) -
                    pow(0.5, (t / data->thalf[1])))) /
                    (data->thalf[0] - data->thalf[1])) + 0.5); 
}

static gdouble decay_stat_n3_of_3(gdouble t, SimData *data)
{
    return (gulong) ((data->atoms[0] -
                    (data->atoms[0] * ((data->thalf[0] *
                    pow(0.5, (t / data->thalf[0])) -
                    data->thalf[1] * pow(0.5, (t / data->thalf[1]))) /
                    (data->thalf[0] - data->thalf[1])))) + 0.5);
}

static gdouble decay_stat_a1(gdouble t, SimData *data)
{
    return (data->atoms[0] * pow(0.5, (t / data->thalf[0])) * log(2)) /
           data->thalf[0];
}

static gdouble decay_stat_a2_of_3(gdouble t, SimData *data)
{
    return (data->atoms[0] * log(2) * (pow(0.5, (t / data->thalf[0])) -
           pow(0.5, (t / data->thalf[1])))) /
           (data->thalf[0] - data->thalf[1]);
}

static gdouble decay_stat_math_is_too_complicated(gdouble t, SimData *data)
{
    IGNORE(t);
    IGNORE(data);
    return 0.000;
}

/* berechnet über die Binomial-Verteilung, wieviele von den n Atomen
   mit der Halbwertszeit thalf im Zeitraum t zerfallen */
static gint decay_binomial(gdouble t, gulong n,
                           gdouble thalf, gsl_rng *rand)
{
    return gsl_ran_binomial(rand, 1.0 - pow(0.5, (t / thalf)), n);
}

/* holt die Eingaben des Nutzers */
static void get_sim_input(GtkWidget *top, SimData *data)
{
    GtkWidget *spin_states, *spin_number, **spin_htime, **menu_htime;
    gint i;

    spin_states = g_object_get_data(G_OBJECT(top), "spin_states");
    spin_number = g_object_get_data(G_OBJECT(top), "spin_number");
    spin_htime = g_object_get_data(G_OBJECT(top), "spin_htime");
    menu_htime = g_object_get_data(G_OBJECT(top), "menu_htime");

    data->states = gtk_spin_button_get_value_as_int(
                       GTK_SPIN_BUTTON(spin_states));
    data->atoms[0] = (gulong)
                     gtk_spin_button_get_value(
                             GTK_SPIN_BUTTON(spin_number));

    for (i = 1; i < ATOM_STATES ; i++)
        data->atoms[i] = 0;
    for (i = 0; i < (data->states - 1); i++)
        data->thalf[i] = gtk_spin_button_get_value(
                         GTK_SPIN_BUTTON(spin_htime[i])) *
                         GPOINTER_TO_INT(g_object_get_data(G_OBJECT(
                         gtk_menu_get_active(GTK_MENU(
                         gtk_option_menu_get_menu(GTK_OPTION_MENU(
                         menu_htime[i]))))), "factor"));
}
