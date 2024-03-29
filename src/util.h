/* 
 * util.h - kleine Hilfsfunktionen
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

#ifndef _UTIL_H
#define _UTIL_H

#include <gtk/gtk.h>
#include <stdarg.h>

#define IGNORE(x) x = x
#define ROUND(x, type) ((type) ((x) + (((x) >= 0) ? 0.5 : -0.5)))
#define TOGGLE(x) ((x) ? FALSE : TRUE)

gint ipow(gint x, guint y);
gdouble round_digits(gdouble x, guint digits);
gboolean layout_printf(PangoLayout *layout, const gchar *format, ...);
gboolean layout_vprintf(PangoLayout *layout, const gchar *format,
                        va_list arg);
gboolean label_printf(GtkWidget *label, const gchar *format, ...);
gint printf_utf8(const gchar *format, ...);
gchar *strdup_pretty_number(gdouble d, gboolean fract);
gdouble dmax(gdouble x, gdouble y);
gint max_n(gint n, ...);
gdouble fmax_n(gint n, gdouble *num);
gdouble closest_val(gdouble val, gint n, ...);
void nlist_remove(gulong *array, gulong index, gulong len);
void widget_modify_bg(GtkWidget *widget, GdkColor *color);


#endif /* _UTIL_H */
