#include <stdio.h>
#include <gtk/gtk.h>

#include "atoms.h"
#include "afield.h"

Atoms *create_atoms(gint32 number)
{
    Atoms *atoms;

    atoms = (Atoms *) g_malloc(sizeof(Atoms));
    atoms->list = (AtomList *) g_malloc(number * sizeof(AtomList));

    atoms->number = number;
    atoms->states[0] = number;
    atoms->states[1] = 0;
    atoms->states[2] = 0;

    return atoms;
}

void destroy_atoms(Atoms *atoms)
{
    g_free(atoms->list);
    g_free(atoms);
}

static void copy_alist(AtomList *a, AtomList *b)
{
    a->state = b->state;
    a->x = b->x;
    a->y = b->y;
}

/* Ordnet die Liste der Atome zufällig neu an */
void randomize_atom_list(AtomList *alist, gint32 number)
{
    GRand *rand;
    gint32 i, rand_x, *index_numbers;
    AtomList *temp_alist;

    /* reserviert Speicher für die Liste der Index-Zahlen */
    index_numbers = (gint32 *) g_malloc(number * sizeof(gint32));

    /* erstellt die Liste der Index-Zahlen (von 0 bis number) */
    for (i = 0; i < number; i++)
        *(index_numbers + i) = i;

    /* reserviert Speicher für die temp. Liste der Atome */
    temp_alist = (AtomList *) g_malloc(number * sizeof(Atoms));

    /* initialisiert die Zufalls-Funktion */
    rand = g_rand_new();

    for (i = number - 1; i >= 0; i--) {
        /* generiert eine Zufallszahl, Bereich: [0; i] */
        rand_x = g_rand_int_range(rand, 0, i + 1);

        /* kopiert das i-te Element aus der Atomliste an die
         * zufällige Indexnummer der temporären Liste */
        copy_alist((temp_alist + *(index_numbers + rand_x)), (alist + i));
        
        /* kopiert das letzte Element an die gerade verwendete und nun
         * nicht mehr benötigte Stelle der Index-Nummern */
        *(index_numbers + rand_x) = *(index_numbers + i);
    }

    g_rand_free(rand);

    /* kopiert die Inhalte der temporären in die richtige
     * Liste der Atome */
    for (i = 0; i < number; i++)
        copy_alist((alist + i), (temp_alist + i));

    /* stellt reservierten Speicher wieder zur Verfügung */
    g_free(temp_alist);
    g_free(index_numbers);
}
