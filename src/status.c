/* 
 * status.c - die Statusanzeige
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

#include "status.h"
#include "atoms.h"
#include "util.h"

#include <gtk/gtk.h>
#include <stdarg.h>
#include <stdio.h>

/* aktualisiert die Statusanzeige der Atome */
void status_update_atoms(GtkWidget **label_atom,
                         GtkWidget **progress_atom,
                         gulong *atoms, gulong total)
{
    gint i;

    for (i = 0; i < ATOM_STATES; i++) {
        label_printf(label_atom[i], "%lu", atoms[i]);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_atom[i]),
                (gdouble) atoms[i] / (gdouble) total);
    }
}

/* aktualisiert die Statusanzeige der Zeit */
void status_update_time(GtkWidget *label_time, gdouble time)
{
    if (time > 10000)
        label_printf(label_time, "%.2es", time);
    else
        label_printf(label_time, "%.2fs", time);
}
