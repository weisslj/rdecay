#include <gtk/gtk.h>
#include <math.h>

#include "atoms.h"
#include "afield.h"
#include "ui_afield.h"
#include "input.h"

void arrange_atoms(GtkWidget *afield, Atoms *atoms)
{
    gdouble field_width, field_height, n_root, f_aspect,
            wide, wide_cols, wide_rows, rows_raw, cols_raw;
    gint rows, cols, row, col;
    gint32 i;

    field_width = afield->allocation.width;
    field_height = afield->allocation.height;

    n_root = sqrt(atoms->number);
    f_aspect = field_width / field_height;

    rows_raw = sqrt((gdouble) atoms->number * f_aspect);
    cols_raw = sqrt((gdouble) atoms->number / f_aspect);

    rows = (gint) (rows_raw);
    cols = (gint) (cols_raw);

    while ((rows * cols) < atoms->number) {
        if (((gdouble) rows - rows_raw) < ((gdouble) cols - cols_raw))
            rows++;
        else
            cols++;
    }

    wide_rows = field_width / rows;
    wide_cols = field_height / cols;

    wide = (wide_rows < wide_cols) ? wide_rows : wide_cols;

    atoms->wide = wide;

    i = 0;
    for (col = 0; col < cols && i < atoms->number; col++) {
        for (row = 0; row < rows && i < atoms->number; row++) {
            (atoms->list + i)->state = 0;
            (atoms->list + i)->x = (gdouble) row * wide;
            (atoms->list + i)->y = (gdouble) col * wide;
            i++;
        }
    }
}

void draw_atom_field(GtkWidget *afield, Atoms *atoms)
{
    gint32 i;
    for (i = 0; i < atoms->number; i++)
        draw_atom(afield, (atoms->list + i), atoms->wide);
}
