#ifndef _ATOMS_H
#define _ATOMS_H

#include <glib.h>

#define ATOM_STATES 3

typedef struct _AtomList {
    gint8 state;
    gdouble x;
    gdouble y;
} AtomList;

typedef struct _Atoms {
    gint32 number;
    gint32 states[ATOM_STATES];
    gdouble wide;
    AtomList *list;
} Atoms;

Atoms *create_atoms(gint32 number);
void destroy_atoms(Atoms *atoms);
void randomize_atom_list(AtomList *atom_list, gint32 number);

#endif /* _ATOMS_H */
