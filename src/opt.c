/* 
 * opt.c - Optionenverwaltung
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

#include "opt.h"
#include "arg.h"

#include <glib.h>

static gdouble fps = 0;
static gboolean showfps = FALSE;

/* initialisiert die Optionen */
void opt_init(gint *argc, gchar **argv[])
{
    fps = arg_get_double(argc, argv, "--fps", "");
    showfps = arg_find(argc, argv, "--showfps", "");
}

/* gibt die Optionen frei */
void opt_free(void)
{
    return;
}

/* gibt die Frames in der Skunde zurück */
gdouble opt_get_fps(void)
{
    return fps;
}

/* gibt zurück, ob die FPS ausgegeben werden sollen */
gboolean opt_get_showfps(void)
{
    return showfps;
}
