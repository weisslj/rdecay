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

#include "afield.h"
#include "atoms.h"
#include "util.h"

#include <gtk/gtk.h>
#include <gsl/gsl_rng.h>
#include <math.h>

/* erstellt ein neues Atomfeld */
AtomField *afield_new(gulong number, GtkWidget *darea)
{
    AtomField *af;
    gulong i;

    /* reserviert Speicher */
    af = (AtomField *) g_malloc(sizeof(AtomField));

    /* speichert die Anzahl der Atome */
    af->number = number;

    /* deaktiviert den Uniform-Modus */
    af->uniform = FALSE;

    /* erstellt trotzdem vorsorglich einen Uniform-Stil */
    af->ustyle = gdk_gc_new(darea->window);

    /* reserviert Speicher */
    af->pos = (gulong *) g_malloc(ATOM_STATES * sizeof(gulong));

    for (i = 0; i < ATOM_STATES; i++)
        af->pos[i] = 0;

    /* reserviert Speicher für die Atomlisten; wenn nicht genügend
       Speicher vorhanden ist, wird der Uniform-Modus aktiviert */
    af->atoms = (AtomInfo *) g_try_malloc(number * sizeof(AtomInfo));
    if (af->atoms == NULL) {
        af->mask = NULL;
        af->uniform = TRUE;
        return af;
    }

    af->mask = (gulong *) g_try_malloc(number * sizeof(gulong));
    if (af->mask == NULL) {
        g_free(af->atoms);
        af->atoms = NULL;
        af->uniform = TRUE;
        return af;
    }

    /* setzt den Status auf Null */
    for (i = 0; i < number; i++)
        (af->atoms + i)->state = 0;

    /* richtet die Atome aus */
    afield_arrange(af, darea);

    return af;
}

/* gibt den Speicher eines erstellten Atomfelds frei */
void afield_free(AtomField *af)
{
    g_free(af->atoms);
    g_free(af->mask);
    g_free(af->pos);
    g_free(af);
}

/* setzt das Atomfeld auf eine neue Anzahl von Atomen */
void afield_reset(AtomField *af, gulong number)
{
    gulong i;

    /* speichert die Anzahl der Atome */
    af->number = number;

    /* deaktiviert den Uniform-Modus */
    af->uniform = FALSE;

    /* setzt den Status auf Null */
    for (i = 0; i < ATOM_STATES; i++)
        af->pos[i] = 0;

    /* passt den Speicherverbrauch an, wechselt bei zu wenig Speicher
       in den Uniform-Modus */
    af->atoms = (AtomInfo *) g_try_realloc(af->atoms,
                                           number * sizeof(AtomInfo));
    if (af->atoms == NULL) {
        g_free(af->mask);
        af->mask = NULL;
        af->uniform = TRUE;
        return;
    }
    af->mask = (gulong *) g_realloc(af->mask, number * sizeof(gulong));
    if (af->mask == NULL) {
        g_free(af->atoms);
        af->atoms = NULL;
        af->uniform = TRUE;
        return;
    }

    /* setzt den Status auf Null */
    for (i = 0; i < number; i++)
        (af->atoms + i)->state = 0;
}

/* erstellt eine zufälle Maske für das Atomfeld */
void afield_randomize(AtomField *af, gsl_rng *rand)
{
    gulong i, x, *pool;

    /* reserviert Speicher für den Zahlen-Pool;
       wenn nicht genug Speicher vorhanden ist, schalte in
       den Uniform-Modus */
    pool = (gulong *) g_try_malloc(af->number * sizeof(gulong));
    if (pool == NULL) {
        g_free(af->atoms);
        g_free(af->mask);
        af->atoms = NULL;
        af->mask = NULL;
        af->uniform = TRUE;
        return;
    }

    /* füllt den Pool mit Zahlen (von 0 bis number) */
    for (i = 0; i < af->number; i++)
        pool[i] = i;

    i = af->number;
    do {
        /* generiert eine Zufallszahl, Bereich: [0; i] */
        x = gsl_rng_uniform_int(rand, i);

        /* fügt die x-te Zahl aus dem Pool der Maske hinzu, und
           entfernt dann diese Zahl aus dem Pool */
        af->mask[af->number - i] = pool[x];
        nlist_remove(pool, x, i--);
    } while (i != 0);

    /* stellt reservierten Speicher wieder zur Verfügung */
    g_free(pool);
}

/* ordnet die Atome im Atomfeld an */
void afield_arrange(AtomField *af, GtkWidget *darea)
{
    gdouble n_root, f_aspect, rows_raw, cols_raw;
    gint wide_cols, wide_rows, wide, padding, rows, cols, row, col;
    gulong i;

    /* schaltet in den Uniform-Modus, wenn weniger Pixel als
       Atome vorhanden sind */
    if ((gulong) (darea->allocation.width * darea->allocation.height)
            < af->number) {
        g_free(af->atoms);
        g_free(af->mask);
        af->atoms = NULL;
        af->mask = NULL;
        af->uniform = TRUE;
        return;
    }

    /* suchst eine optimale Reihen-Spalten Verteilung */
    n_root = sqrt(af->number);
    f_aspect = (gdouble) darea->allocation.width / darea->allocation.height;

    rows_raw = sqrt((gdouble) af->number * f_aspect);
    cols_raw = sqrt((gdouble) af->number / f_aspect);

    rows = (gint) (rows_raw);
    cols = (gint) (cols_raw);

    while ((gulong) (rows * cols) < af->number) {
        if ((rows - rows_raw) < (cols - cols_raw))
            rows++;
        else
            cols++;
    }

    /* berechnet die optimale Ausdehnung */
    wide_rows = (gdouble) darea->allocation.width / (gdouble) rows;
    wide_cols = (gdouble) darea->allocation.height / (gdouble) cols;

    wide = MIN(wide_rows, wide_cols);
    af->wide = 0.8 * wide + 0.5;

    padding = (wide - af->wide) / 2.0;


    /* weist jedem Atom seine Position zu */
    i = 0;
    for (col = 0; col < cols && i < af->number; col++) {
        for (row = 0; row < rows && i < af->number; row++) {
            (af->atoms + i)->x = row * wide + padding;
            (af->atoms + i)->y = col * wide + padding;
            i++;
        }
    }
}
