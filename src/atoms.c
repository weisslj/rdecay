/* 
 * atoms.c - Deklarationen zu Atomen
 *
 * Copyright 2004-2017 Johannes Weißl
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

#include "atoms.h"

#include <glib.h>
#include <stdlib.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

/* die statische Liste der Atomnamen */
static const gchar *atom_names[] = {
    N_("Mother"),
    N_("Doughter"),
    N_("Grandchild"),
    N_("Great-grandchild"),
    N_("Great-great-grandchild"),
    N_("Great-great-great-grandchild"),
    N_("Great-great-great-great-grandchild"),
    N_("Great-great-great-great-great-grandchild"),
    N_("Great-great-great-great-great-great-grandchild"),
    N_("Great-great-great-great-great-great-great-grandchild"),
    N_("Great-great-great-great-great-great-great-great-grandchild"),
    N_("Great-great-great-great-great-great-great-great-great-grandchild")
};

/* gibt den Namen des Atoms mit dem Status "state" zurück */
G_CONST_RETURN gchar *atoms_get_name(gint state)
{
    return (state <= 9) ? atom_names[state] : N_("Yet another offspring");
}

gdouble atoms_get_htime(gint state)
{
    if (state > 6)
        return rand();
    else
        return 10.0 - 2 * (state - 2);
}
