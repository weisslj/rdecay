#ifndef _STATUS_H
#define _STATUS_H

#include <gtk/gtk.h>

GtkWidget *create_status_field(GtkWidget *parent_box);
void update_status_atoms(GtkWidget *widget, gint n1, gint n2, gint n3);
void update_status_time(GtkWidget *widget, gdouble time);

#endif /* _STATUS_H */
