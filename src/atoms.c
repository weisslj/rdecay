#include "atoms.h"

#include <glib.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

/* die statische Liste der Atomnamen */
static const gchar *atom_names[] = {
    N_("mother"),
    N_("daughter"),
    N_("grandchild")
};

/* gibt den Namen des Atoms mit dem Status "state" zurück */
G_CONST_RETURN gchar *get_atom_name(gint state)
{
    return atom_names[state];
}
