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

#include <gtk/gtk.h>
#include <gsl/gsl_rng.h>

typedef struct _AtomInfo AtomInfo;

struct _AtomInfo {
    gint state;
    gint x;
    gint y;
};


typedef struct _AtomField AtomField;

struct _AtomField {
    gulong number;
    gint wide;

    gulong *mask;
    gulong *pos;

    gboolean uniform;
    GdkGC *ustyle;

    AtomInfo *atoms;
};

AtomField *afield_new(gulong number, GtkWidget *darea);
void afield_reset(AtomField *af, gulong number);
void afield_free(AtomField *af);
void afield_randomize(AtomField *af, gsl_rng *rand);
void afield_arrange(AtomField *af, GtkWidget *darea);

#endif /* _AFIELD_H */
