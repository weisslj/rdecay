#ifndef _DAREA_H
#define _DAREA_H

#include <gtk/gtk.h>

#include "darea.h"

GtkWidget *darea_new(GtkWidget *parent_box, const gchar *name,
                     gint width, gint height,
                     GCallback init, GCallback resize);

void darea_clear(GtkWidget *darea);

#endif /* _DAREA_H */
