/* 
 * darea.h - Zeichenbereiche
 *
 * Copyright 2004-2017 Johannes Weißl
 *
 * This file is part of rdecay.
 *
 * rdecay is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * rdecay is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rdecay.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _DAREA_H
#define _DAREA_H

#include <gtk/gtk.h>

#define N_DAREAS 2

GtkWidget *darea_new(void);
void darea_clear(GtkWidget *darea);
void darea_update(GtkWidget *darea);

void darea_init(GtkWidget *darea);
gboolean darea_resize(GtkWidget *darea);
gboolean darea_redraw(GtkWidget *darea, GdkEventExpose *event);

#endif /* _DAREA_H */
