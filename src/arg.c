/* 
 * arg.c - Verarbeitung von Argumenten
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

#include "arg.h"

#include <glib.h>
#include <string.h>
#include <stdlib.h>

/* überprüft, ob ein Argument angegeben wurde */
gboolean arg_find(gint *argc, gchar ***argv, const gchar *l, const gchar *s)
{
    gint i, result;

    result = FALSE;
    for (i = 1; i < *argc; i++)
        if (strcmp((*argv)[i], l) == 0 || strcmp((*argv)[i], s) == 0) {
            arg_remove(argc, argv, i, 1);
            result = TRUE;
        }

    return result;
}

/* holt den Inhalt des Arguments als Zeichenkette */
gchar *arg_get_string(gint *argc, gchar ***argv,
                      const gchar *l, const gchar *s)
{
    gint i;
    gchar *value;

    value = NULL;

    for (i = 1; i < *argc; i++) {
        if (strcmp((*argv)[i], l) != 0 && strcmp((*argv)[i], s) != 0)
            continue;
        if ((*argv)[i + 1] == NULL || *((*argv)[i + 1]) == '-') {
            g_free(value);
            value = g_strdup("");
            arg_remove(argc, argv, i, 1);
            continue;
        }
        g_free(value);
        value = g_strdup((*argv)[i + 1]);
        arg_remove(argc, argv, i, 2);
    }

    return value;
}

/* holt den Inhalt des Arguments als Ganzzahl */
gint arg_get_int(gint *argc, gchar ***argv, const gchar *l, const gchar *s)
{
    gint i;
    gchar *value;

    value = arg_get_string(argc, argv, l, s);
    if (value == NULL)
        return 0;

    i = atoi(value);
    g_free(value);

    return i;
}

/* holt den Inhalt des Arguments als Fließkommazahl */
gdouble arg_get_double(gint *argc, gchar ***argv,
                      const gchar *l, const gchar *s)
{
    gdouble d;
    gchar *value;

    value = arg_get_string(argc, argv, l, s);
    if (value == NULL)
        return 0;

    d = atof(value);
    g_free(value);

    return d;
}

/* entfernt ein Argument aus der Argumentliste */
void arg_remove(gint *argc, gchar ***argv, gint pos, gint len)
{
    if (len <= (*argc - pos)) {
        do
            (*argv)[pos] = (*argv)[pos + len];
        while ((*argv)[pos++] != NULL);
        *argc -= len;
    }
}
