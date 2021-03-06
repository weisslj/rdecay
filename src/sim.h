/* 
 * sim.h - die Simulation des Zerfalls
 *
 * Copyright (C) 2004-2017 Johannes Weißl
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

#ifndef _SIM_H
#define _SIM_H

#include "coord.h"
#include "afield.h"

#include <gtk/gtk.h>
#include <gsl/gsl_rng.h>

typedef struct _SimData SimData;

struct _SimData {
    AtomField *afield;
    CoordSystem *coord_number,
                *coord_activity;
    gsl_rng *rand;

    gint states;
    gulong *atoms;
    gdouble *thalf;
};

SimData *simdata_new(gsl_rng *rand);
void simdata_free(SimData *sdata);
void sim_decay(GtkWidget *button_start, SimData *sdata);

#endif /* _SIM_H */
