#include <gtk/gtk.h>
#include <math.h>
#include <time.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "sim.h"
#include "input.h"
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
static void stop_sim(GtkWidget *button, gint *quit);

static gdouble calc_duration(gint number, gdouble thalf);
static gdouble exp_growth(gdouble t, SimData *data);
static gint decay_real(gdouble t, gint n, gdouble thalf, gsl_rng *rand);

static gint decay_stat(gdouble t, gint n, gint n0, gdouble thalf);

void sim_decay(GtkWidget *button, gsl_rng *rand)
{
    GtkWidget *top, *afield, *graph, *bstop, *n0_input, *htime_input;
    gdouble t, thalf, tstart, tnext, told, tstep, tloop;

    CoordSystem *coord;
    Graph *gr;
    Point *point, *old_point;

    MyTimer *timer;
    gint quit;
    gint number, pos, state, decays, i;
    AtomField *af;

    SimData *sdata;
    GraphFunc *gf;

    top = gtk_widget_get_toplevel(button);

    n0_input = g_object_get_data(G_OBJECT(top), "n0_input");
    htime_input = g_object_get_data(G_OBJECT(top), "htime_input");
    afield = g_object_get_data(G_OBJECT(top), "atom_field");
    graph = g_object_get_data(G_OBJECT(top), "graph");

    g_signal_handlers_block_by_func(G_OBJECT(button), (gpointer) sim_decay, rand);
    gtk_button_set_label(GTK_BUTTON(button), _("pause"));
    gtk_button_leave(GTK_BUTTON(button));

    quit = 0;
    bstop = g_object_get_data(G_OBJECT(top), "stop_button");
    gtk_widget_set_sensitive(bstop, TRUE);
    g_signal_connect(G_OBJECT(bstop), "clicked", G_CALLBACK(stop_sim), &quit);

    sdata = (SimData *) g_malloc(sizeof(SimData));
    number = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(n0_input));
    thalf = gtk_spin_button_get_value(GTK_SPIN_BUTTON(htime_input));
    sdata->atoms[0] = number;
    sdata->states = 3;
    sdata->atoms[1] = 0;
    sdata->atoms[2] = 0;
    sdata->thalf[0] = thalf;
    sdata->thalf[1] = 10.0;

    gf = (GraphFunc *) g_malloc(sizeof(GraphFunc));
    gf->func = exp_growth;
    gf->data = sdata;

    af = afield_new(number, afield->allocation.width, afield->allocation.height);
    afield_randomize(af, rand);

    tstep = 0.003;
    pos = 0;

/*    update_status_label(atoms->states[0]); */
    darea_clear(afield);
    afield_draw(afield, af);

    coord = coord_system_new(graph->allocation.width, graph->allocation.height,
                             0, calc_duration(number, thalf),
                             0, number);

    darea_clear(graph);
    coord_system_draw(graph, coord);

    graph_draw_func(gf, graph, coord);

    gr = graph_new(0);

    timer = timer_new();
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(pause_sim), timer);
    tstart = tnext = told = timer_elapsed(timer);

    while(sdata->atoms[0] > 0 && (!quit)) {
        t = timer_elapsed(timer) - tstart;
        if (t >= tnext) {
            tloop = t - told;
            told = t;
            for (state = 0; state < sdata->states - 1; state++) {
                decays = decay_real(tloop, sdata->atoms[state], thalf, rand);
                if (decays > 0) {
                    sdata->atoms[state + 0] -= decays;
                    sdata->atoms[state + 1] += decays;

                    update_status_atoms(afield, sdata->atoms[0], sdata->atoms[1], sdata->atoms[2]);

                    for (i = 0; i < decays; i++) {
                        (af->coords + pos)->state = 1;
                        draw_atom(afield, (af->coords + pos), af->wide);
                        pos++;
                    }
                }

                point = point_alloc(t, sdata->atoms[0]);
                if (gr->points != NULL) {
                    old_point = gr->points->data;
                    graph_draw_line(graph, coord, old_point->x, old_point->y, point->x, point->y, "style1");
                }
                graph_add(gr, point);
            }

            tnext += tstep;
        }
        update_status_time(afield, t); 

        while (gtk_events_pending())
            gtk_main_iteration();
/*        while (g_main_iteration(FALSE)); */
    }

    g_signal_handlers_disconnect_matched(G_OBJECT(bstop),
                                         G_SIGNAL_MATCH_FUNC,
                                         0,
                                         0,
                                         NULL,
                                         (gpointer) stop_sim,
                                         NULL);

    gtk_widget_set_sensitive(bstop, FALSE);

    if (timer_is_running(timer))
        g_signal_handlers_disconnect_matched(G_OBJECT(button),
                G_SIGNAL_MATCH_FUNC, 0, 0, NULL, (gpointer) pause_sim, NULL);
    else
        g_signal_handlers_disconnect_matched(G_OBJECT(button), G_SIGNAL_MATCH_FUNC, 0, 0, NULL, (gpointer) resume_sim, NULL);

    g_signal_handlers_unblock_by_func(G_OBJECT(button), (gpointer) sim_decay, rand);
    gtk_button_set_label(GTK_BUTTON(button), _("start"));

    timer_free(timer);

/*    start_buttons_set_sensitive(TRUE); */

    afield_free(af);

    coord_system_free(coord);
}

static void resume_sim(GtkWidget *button, MyTimer *timer)
{
    timer_start(timer);
    g_signal_handlers_disconnect_by_func(G_OBJECT(button),
    (gpointer) resume_sim, timer);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(pause_sim), timer);
    gtk_button_set_label(GTK_BUTTON(button), _("pause"));
}

static void pause_sim(GtkWidget *button, MyTimer *timer)
{
    timer_stop(timer);
    g_signal_handlers_disconnect_by_func(G_OBJECT(button), (gpointer) pause_sim, timer);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(resume_sim), timer);
    gtk_button_set_label(GTK_BUTTON(button), _("resume"));
}

static void stop_sim(GtkWidget *button, gint *quit)
{
    *quit = 1;
}

static gdouble calc_duration(gint number, gdouble thalf)
{
    return -thalf * log2(1.0 / number) + thalf;
}

static gdouble exp_growth(gdouble t, SimData *data)
{
    return (gint) (data->atoms[0] * pow(0.5, (t / data->thalf[0])) + 0.5);
}

static gint decay_stat(gdouble t, gint n, gint n0, gdouble thalf)
{
    return n - (gint) ((n0 * pow(0.5, t / thalf)) + 0.5);
}

static gint decay_real(gdouble t, gint n, gdouble thalf, gsl_rng *rand)
{
/*   return (gsl_ran_binomial(rand, (1.0 - pow(0.5, (t / thalf))), n)); */
   return gsl_ran_poisson(rand, ((1.0 - pow(0.5, (t / thalf)))) * n);
}
