#include <gtk/gtk.h>
#include <math.h>
#include <time.h>

#include "sim.h"
#include "atoms.h"
#include "input.h"
#include "afield.h"
#include "ui_afield.h"
#include "ui_graph.h"
#include "status.h"
#include "timer.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

static void resume_sim(GtkWidget *button, MyTimer *timer);
static void pause_sim(GtkWidget *button, MyTimer *timer);

static void resume_sim(GtkWidget *button, MyTimer *timer)
{
    timer_start(timer);
    g_signal_handlers_disconnect_by_func(G_OBJECT(button), (gpointer) resume_sim, timer);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(pause_sim), timer);
    gtk_button_set_label(GTK_BUTTON(button), "Pause");
}

static void pause_sim(GtkWidget *button, MyTimer *timer)
{
    timer_stop(timer);
    g_signal_handlers_disconnect_by_func(G_OBJECT(button), (gpointer) pause_sim, timer);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(resume_sim), timer);
    gtk_button_set_label(GTK_BUTTON(button), "Weiter");
}

static void stop_sim(GtkWidget *button, gint *quit)
{
    *quit = 1;
}

static gint32 decay_stat(gint32 n0, gdouble t, gdouble thalf)
{
    return (gint32) ((gdouble) n0 * pow(0.5, (t / thalf)) + 0.5);
}
    
static gint32 decay_real(gint32 n, gdouble t, gdouble thalf, gsl_rng *rand)
{
   return (gsl_ran_binomial(rand, (1.0 - pow(0.5, (t / thalf))), n));
/*    return (gsl_ran_poisson(rand, (1.0 - pow(0.5, (t / thalf))), n)); */
}

void sim_decay(GtkWidget *button, gsl_rng *rand)
{
    GtkWidget *top, *afield, *graph, *bstop, *n0_input, *htime_input;
    gdouble t, thalf, tstart, tnext, told, tstep, tloop;
    MyTimer *timer;
    gsl_rng *rand;
    gint quit;
    gint32 number, pos, n_real, n_stat, i;
    Atoms *atoms;

    top = gtk_widget_get_toplevel(button);

    n0_input = g_object_get_data(G_OBJECT(top), "n0_input");
    htime_input = g_object_get_data(G_OBJECT(top), "htime_input");
    afield = g_object_get_data(G_OBJECT(top), "atom_field");
    graph = g_object_get_data(G_OBJECT(top), "graph");

    g_signal_handlers_block_by_func(G_OBJECT(button), (gpointer) sim_decay, menu);
    gtk_button_set_label(GTK_BUTTON(button), "Pause");
    gtk_button_leave(GTK_BUTTON(button));

    quit = 0;
    bstop = g_object_get_data(G_OBJECT(top), "stop_button");
    gtk_widget_set_sensitive(bstop, TRUE);
    g_signal_connect(G_OBJECT(bstop), "clicked", G_CALLBACK(stop_sim), &quit);

    number = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(n0_input));
    thalf = gtk_spin_button_get_value(GTK_SPIN_BUTTON(htime_input));

    atoms = create_atoms(number);
    arrange_atoms(afield, atoms);
    randomize_atom_list(atoms->list, atoms->number);

    tstep = 0.003;
    pos = 0;

/*    update_status_label(atoms->states[0]); */
    clear_atom_field(afield);
    draw_atom_field(afield, atoms);
    clear_graph(graph);
    draw_graph_coord_system(graph);
    while (g_main_iteration(FALSE));

/*    rand = gsl_rng_alloc(gsl_rng_taus2); */
    rand = gsl_rng_alloc(gsl_rng_taus);
    gsl_rng_set(rand, time(NULL));

    timer = timer_new();
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(pause_sim), timer);
    tstart = tnext = told = timer_elapsed(timer);

    while(atoms->states[0] > 0 && (!quit)) {
        t = timer_elapsed(timer) - tstart;
        if (t >= tnext) {
            tloop = t - told;
            told = t;
            n_real = decay_real(atoms->states[0], tloop, thalf, rand);
            if (n_real > 0) {
                atoms->states[0] -= n_real;
                atoms->states[1] += n_real;
                gdk_beep();
                update_status_atoms(afield, atoms->states[0], atoms->states[1], 0);
                /* draw graph */
                for (i = 0; i < n_real; i++) {
                    (atoms->list + pos)->state = 1;
                    draw_atom(afield, (atoms->list + pos), atoms->wide);
                    pos++;
                }
            }
/*            n_stat = decay_stat(atoms->states[0], thalf, t); */
            tnext += tstep;
        }
        update_status_time(afield, t); 

        while (gtk_events_pending())
            gtk_main_iteration();
/*        while (g_main_iteration(FALSE)); */
    }

    g_signal_handlers_disconnect_matched(G_OBJECT(bstop), G_SIGNAL_MATCH_FUNC, 0, 0, NULL, (gpointer) stop_sim, NULL);
    gtk_widget_set_sensitive(bstop, FALSE);

    if (timer_is_running(timer))
        g_signal_handlers_disconnect_matched(G_OBJECT(button), G_SIGNAL_MATCH_FUNC, 0, 0, NULL, (gpointer) pause_sim, NULL);
    else
        g_signal_handlers_disconnect_matched(G_OBJECT(button), G_SIGNAL_MATCH_FUNC, 0, 0, NULL, (gpointer) resume_sim, NULL);

    g_signal_handlers_unblock_by_func(G_OBJECT(button), (gpointer) sim_decay, menu);
    gtk_button_set_label(GTK_BUTTON(button), "Start");

    timer_destroy(timer);

/*    start_buttons_set_sensitive(TRUE); */

    destroy_atoms(atoms);
}
