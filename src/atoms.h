/* 
 * atoms.h - Deklarationen zu Atomen
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

#ifndef _ATOMS_H
#define _ATOMS_H

#define ATOM_STATES 5

#include <glib.h>

G_CONST_RETURN gchar *atoms_get_name(gint state);
gdouble atoms_get_htime(gint state);

#endif /* _ATOMS_H */
