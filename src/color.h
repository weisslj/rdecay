#ifndef _COLOR_H
#define _COLOR_H

#include <gtk/gtk.h>

GdkColor *color_new(GtkWidget *widget, gint red, gint green, gint blue);
void color_free(GdkColor *color);

#endif /* _COLOR_H */
