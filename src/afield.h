/* 
 * afield.h - das Atomfeld
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

#ifndef _AFIELD_H
#define _AFIELD_H

#include <glib.h>
#include <gsl/gsl_rng.h>

typedef struct _AtomCoord AtomCoord;

struct _AtomCoord {
    gint state;
    gint x;
    gint y;
};

typedef struct _AtomField AtomField;

struct _AtomField {
    gint number;
    gint wide;
    AtomCoord *coords;
};

AtomField *afield_new(gint number, gint field_width, gint field_height);
void afield_free(AtomField *af);
void afield_randomize(AtomField *af, gsl_rng *rand);
void afield_distrib_decays(gint decays, AtomField *af,
                           gint atoms, gint state,
                           gsl_rng *rand);
gint afield_distrib_decay(AtomField *af, gint state, gsl_rng *rand);
void afield_arrange(AtomField *af, gint field_width, gint field_height);

#endif /* _AFIELD_H */
