#include <gtk/gtk.h>
#include <gsl/gsl_rng.h>
#include <math.h>

#include "atoms.h"
#include "afield.h"
#include "ui_afield.h"
#include "input.h"

static void copy_coord(AtomCoord *a, AtomCoord *b);

AtomField *afield_new(gint number, gint field_width, gint field_height)
{
    AtomField *af;

    af = (AtomField *) g_malloc(sizeof(AtomField));
    af->coords = (AtomCoord *) g_malloc(number * sizeof(AtomCoord));

    af->number = number;

    afield_arrange(af, field_width, field_height);

    return af;
}

void afield_free(AtomField *af)
{
    g_free(af->coords);
    g_free(af);
}

/* Ordnet die Liste der Atome zufällig neu an */
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
            (af->coords + i)->state = 0;
            (af->coords + i)->x = row * af->wide;
            (af->coords + i)->y = col * af->wide;
            i++;
        }
    }
}

void afield_draw(GtkWidget *darea, AtomField *af)
{
    gint i;
    for (i = 0; i < af->number; i++)
        draw_atom(darea, (af->coords + i), af->wide);
}

static void copy_coord(AtomCoord *a, AtomCoord *b)
{
    a->state = b->state;
    a->x = b->x;
    a->y = b->y;
}
