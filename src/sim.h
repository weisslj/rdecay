/* 
 * sim.h - die Simulation des Zerfalls
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

#ifndef _SIM_H
#define _SIM_H

#include <gtk/gtk.h>
#include <gsl/gsl_rng.h>

#define ATOM_STATES 3

typedef struct _SimData SimData;

struct _SimData {
    gint states;
    gint atoms[ATOM_STATES];
    gdouble thalf[ATOM_STATES - 1];
};

void sim_decay(GtkWidget *button, gsl_rng *rand);

#endif /* _SIM_H */
