/* 
 * ui_afield.h - Toolkit-Spezifisches zum Atomfeld
 *
 * Copyright 2004 Johannes Weißl
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
 * along with rdecay; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _UI_AFIELD_H
#define _UI_AFIELD_H

#include "afield.h"

#include <gtk/gtk.h>

gboolean afield_resize(GtkWidget *darea, GdkEventConfigure *event, AtomField *af);
void afield_draw_atom(GtkWidget *darea, AtomField *af, gulong n);
void afield_distrib_decays(GtkWidget *darea, AtomField *af, gulong decays, gint state);
void afield_tint(GtkWidget *darea, AtomField *af, gulong *atoms, gint states);
void afield_draw(GtkWidget *darea, AtomField *af);
gboolean afield_benchmark(GtkWidget *darea, AtomField *af, gdouble max_t);

#endif /* _UI_AFIELD_H */
