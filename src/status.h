/* 
 * status.h - die Statusanzeige
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

#ifndef _STATUS_H
#define _STATUS_H

#include <gtk/gtk.h>

void status_update_time(GtkWidget *label_time, gdouble time);
void status_update_atoms(GtkWidget **label_atom,
                         GtkWidget **progress_atom,
                         gulong *atoms, gulong total);

#endif /* _STATUS_H */
