#ifndef _AFIELD_H
#define _AFIELD_H

#include <gtk/gtk.h>
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
void afield_arrange(AtomField *af, gint field_width, gint field_height);
void afield_draw(GtkWidget *darea, AtomField *af);

#endif /* _AFIELD_H */
