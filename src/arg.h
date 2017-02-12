/* 
 * arg.h - Verarbeitung von Argumenten
 *
 * Copyright (C) 2004-2017 Johannes Weißl
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
 * along with rdecay.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _ARG_H
#define _ARG_H

#include <glib.h>

gint arg_find(gint *argc, gchar ***argv,
        const gchar *long_name, const gchar *short_name);

gchar *arg_get_string(gint *argc, gchar ***argv,
        const gchar *long_name, const gchar *short_name);
gint arg_get_int(gint *argc, gchar ***argv,
        const gchar *long_name, const gchar *short_name);
gdouble arg_get_double(gint *argc, gchar ***argv,
        const gchar *long_name, const gchar *short_name);

void arg_remove(gint *argc, gchar ***argv, gint pos, gint len);

#endif /* _ARG_H */
