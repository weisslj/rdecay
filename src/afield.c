/* 
 * afield.c - das Atomfeld
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

#include <glib.h>
#include <gsl/gsl_rng.h>
#include <math.h>

#include "afield.h"

static void copy_coord(AtomCoord *a, AtomCoord *b);
static gint count_atoms(AtomField *af, gint state);

/* erstellt ein neues Atomfeld */
AtomField *afield_new(gint number, gint field_width, gint field_height)
{
    gint i;
    AtomField *af;

    /* reserviert Speicher */
    af = (AtomField *) g_malloc(sizeof(AtomField));
    af->coords = (AtomCoord *) g_malloc(number * sizeof(AtomCoord));

    /* speichert die Anzahl der Atome */
    af->number = number;

    /* setzt den Status auf Null */
    for (i = 0; i < number; i++)
        (af->coords + i)->state = 0;

    /* richtet die Atome aus */
    afield_arrange(af, field_width, field_height);

    return af;
}

/* gibt den Speicher eines erstellten Atomfelds frei */
void afield_free(AtomField *af)
{
    g_free(af->coords);
    g_free(af);
}

/* ordnet die Liste der Atome zufällig neu an */
void afield_randomize(AtomField *af, gsl_rng *rand)
{
    gint i, rand_x, *index_numbers;
    AtomCoord *temp_coords;

    /* reserviert Speicher für die Liste der Index-Zahlen */
    index_numbers = (gint *) g_malloc(af->number * sizeof(gint));

    /* erstellt die Liste der Index-Zahlen (von 0 bis number) */
    for (i = 0; i < af->number; i++)
        *(index_numbers + i) = i;

    /* reserviert Speicher für die temp. Liste der Atome */
    temp_coords = (AtomCoord *) g_malloc(af->number * sizeof(AtomCoord));

    for (i = af->number - 1; i >= 0; i--) {
        /* generiert eine Zufallszahl, Bereich: [0; i] */
        rand_x = gsl_rng_uniform_int(rand, i + 1);

        /* kopiert das i-te Element aus der Atomliste an die
         * zufällige Indexnummer der temporären Liste */
        copy_coord((temp_coords + *(index_numbers + rand_x)), (af->coords + i));
        
        /* kopiert das letzte Element an die gerade verwendete und nun
         * nicht mehr benötigte Stelle der Index-Nummern */
        *(index_numbers + rand_x) = *(index_numbers + i);
    }

    /* kopiert die Inhalte der temporären in die richtige
     * Liste der Atome */
    for (i = 0; i < af->number; i++)
        copy_coord((af->coords + i), (temp_coords + i));

    /* stellt reservierten Speicher wieder zur Verfügung */
    g_free(temp_coords);
    g_free(index_numbers);
}

void afield_distrib_decays(gint decays, AtomField *af,
                           gint atoms, gint state,
                           gsl_rng *rand)
{
    gint i;

    for (i = 0; i < decays; i++) {
        afield_distrib_decay(af, state, rand);
        atoms--;
    }
}

gint afield_distrib_decay(AtomField *af, gint state, gsl_rng *rand)
{
    gint atoms, hit, i, n;

    atoms = count_atoms(af, state);
    hit = gsl_rng_uniform_int(rand, atoms);

    n = 0;
    for (i = 0; i < af->number; i++) {
        if ((af->coords + i)->state == state) {
            if (n == hit) {
                (af->coords + i)->state++;
                return i;
            }
            n++;
        }
    }
    return -1;
}
            

/* ordnet die Atome im Atomfeld an */
void afield_arrange(AtomField *af, gint field_width, gint field_height)
{
    gdouble n_root, f_aspect, rows_raw, cols_raw;
    gint wide_cols, wide_rows, rows, cols, row, col, i;
    gdouble a, b, c;

    n_root = sqrt(af->number);
    f_aspect = (gdouble) field_width / (gdouble) field_height;

    rows_raw = sqrt((gdouble) af->number * f_aspect);
    cols_raw = sqrt((gdouble) af->number / f_aspect);

    rows = (gint) (rows_raw);
    cols = (gint) (cols_raw);

    while ((rows * cols) < af->number) {
        if ((rows - rows_raw) < (cols - cols_raw))
            rows++;
        else
            cols++;
    }

    wide_rows = (gdouble) field_width / (gdouble) rows + 0.5;
    wide_cols = (gdouble) field_height / (gdouble) cols + 0.5;

    a = (gdouble) field_width / (gdouble) rows;
    b = (gdouble) field_height / (gdouble) cols;

    c = MIN(a, b);

    af->wide = MIN(wide_rows, wide_cols);

    i = 0;
    for (col = 0; col < cols && i < af->number; col++) {
        for (row = 0; row < rows && i < af->number; row++) {
            (af->coords + i)->x = row * af->wide;
            (af->coords + i)->y = col * af->wide;
            i++;
        }
    }
}

/* kopiert die Werte der Atomkoordinate b in a */
static void copy_coord(AtomCoord *a, AtomCoord *b)
{
    a->state = b->state;
    a->x = b->x;
    a->y = b->y;
}

/* FIXME */
static gint count_atoms(AtomField *af, gint state)
{
    gint i, n;

    n = 0;
    for (i = 0; i < af->number; i++) {
        if ((af->coords + i)->state == state)
            n++;
    }

    return n;
}
