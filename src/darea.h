#ifndef _DAREA_H
#define _DAREA_H

#include <gtk/gtk.h>

#include "atoms.h"

GtkWidget *create_atom_field(GtkWidget *parent_box, gdouble width, gdouble height);
void clear_atom_field(GtkWidget *afield);
void draw_atom(GtkWidget *afield, AtomList *atom, gdouble wide);

#endif /* _DAREA_H */
