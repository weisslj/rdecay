/* 
 * status.c - die Statusanzeige
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

#include <gtk/gtk.h>
#include <stdarg.h>

#include "status.h"
#include "sim.h"

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

/* die statische Liste der Atomnamen */
static const gchar *atom_names[] = {
    N_("mother"),
    N_("doughter"),
    N_("grandchild")
};

static gint label_printf(GtkWidget *label, const gchar *format, ...);

/* gibt den Namen des Atoms mit der Nummer "state" zurück */
G_CONST_RETURN gchar *get_atom_name(gint state)
{
    return atom_names[state];
}

/* aktualisiert die Statusanzeige der Atome */
void status_update_atoms(GtkWidget **label_atom, gint *atoms)
{
    gint i;

    for (i = 0; i < ATOM_STATES; i++)
        label_printf(label_atom[i], _("%s atoms: %d"), atom_names[i], atoms[i]);
}

/* aktualisiert die Statusanzeige der Zeit */
void status_update_time(GtkWidget *label_time, gdouble time)
{
    label_printf(label_time, _("time: %.2f"), time);
}

/* schreibt auf ein GtkLabel wie mit printf */
static gint label_printf(GtkWidget *label, const gchar *format, ...)
{
    gchar *text;
    va_list ap;

    va_start(ap, format);
    text = g_strdup_vprintf(format, ap);
    va_end(ap);

    if (text == NULL)
        return 0;

    gtk_label_set_text(GTK_LABEL(label), text);
    g_free(text);

    return 1;
}
