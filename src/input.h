#ifndef _INPUT_H
#define _INPUT_H

#include <gtk/gtk.h>
#include <gsl/gsl_rng.h>

GtkWidget *create_input_fields(GtkWidget *parent_box);
GtkWidget *create_ctrl_buttons(GtkWidget *parent_box, gsl_rng *rand);

#endif /* _INPUT_H */
