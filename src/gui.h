/* 
 * gui.h - die Benutzeroberfläche
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

#ifndef _GUI_H
#define _GUI_H

#include <gtk/gtk.h>

GtkWidget *gui_create(void);
void gui_destroy(void);
gboolean gui_delete(void);
gdouble scale_get_speed(GtkWidget *scale_speed);
gulong combo_get_unit(gint active);

#endif /* _GUI_H */
