#ifndef _UI_AFIELD_H
#define _UI_AFIELD_H

#include <gtk/gtk.h>

#include "atoms.h"

GtkWidget *create_atom_field(GtkWidget *parent_box, gdouble width, gdouble height);
void clear_atom_field(GtkWidget *afield);
void draw_atom(GtkWidget *afield, AtomList *atom, gdouble wide);

#endif /* _UI_AFIELD_H */
