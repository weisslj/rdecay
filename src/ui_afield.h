#ifndef _UI_AFIELD_H
#define _UI_AFIELD_H

#include <gtk/gtk.h>

#include "afield.h"

GtkWidget *create_atom_field(GtkWidget *parent_box, gdouble width, gdouble height);
void draw_atom(GtkWidget *darea, AtomCoord *coord, gint wide);

#endif /* _UI_AFIELD_H */
