#ifndef _SIM_H
#define _SIM_H

#include <gtk/gtk.h>
#include <gsl/gsl_rng.h>

#define ATOM_STATES 3

typedef struct _SimData SimData;

struct _SimData {
    gint states;
    gint atoms[ATOM_STATES];
    gdouble thalf[ATOM_STATES - 1];
};

void sim_decay(GtkWidget *button, gsl_rng *rand);

#endif /* _SIM_H */
