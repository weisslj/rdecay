/* 
 * util.c - kleine Hilfsfunktionen
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

#include "util.h"

#include <gtk/gtk.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

/* berechnet den Zweierlogarithmus von x */
gdouble log2(gdouble x)
{
    return log(x) / log(2);
}

/* setzt den Text eines Pango Layouts wie mit printf */
gboolean layout_printf(PangoLayout *layout, const gchar *format, ...)
{
    gchar *text;
    va_list ap;

    va_start(ap, format);
    text = g_strdup_vprintf(format, ap);
    va_end(ap);

    if (text == NULL)
        return FALSE;

    pango_layout_set_text(layout, text, -1);

    g_free(text);

    return TRUE;
}

/* setzt den Text eines Pango Layouts wie mit vprintf */
gboolean layout_vprintf(PangoLayout *layout, const gchar *format, va_list arg)
{
    gchar *text;

    text = g_strdup_vprintf(format, arg);

    if (text == NULL)
        return FALSE;

    pango_layout_set_text(layout, text, -1);

    g_free(text);

    return TRUE;
}

/* schreibt auf ein GtkLabel wie mit printf */
gboolean label_printf(GtkWidget *label, const gchar *format, ...)
{
    gchar *text;
    va_list ap;

/*    printf("label_printf\n");

    printf("label: %p | format: %p [%s]\n", (gpointer) label, (gpointer) format, format); */

    va_start(ap, format);
    text = g_strdup_vprintf(format, ap);
    va_end(ap);


    if (text == NULL) {
        printf("text == NULL\n\n");
        return FALSE;
    }

/*    printf("text: %p [%s]\n", (gpointer) text, text); */

    gtk_label_set_text(GTK_LABEL(label), text);
    g_free(text);

/*    printf("\n"); */

    return TRUE;
}

/* rechntet x hoch y */
gint ipow(gint x, guint y)
{
    gint z;

    z = 1;
    while (y-- > 0)
        z *= x;

    return z;
}

/* rundet eine Fließkommazahl auf eine bestimme Stelle hinter dem Komma */
gdouble round_digits(gdouble x, guint digits)
{
    gint n;

    n = ipow(10, digits);
    x *= n;
    x = ROUND(x, gint);
    x /= n;

    return x;
}

/* gibt den größten der beiden Werte zurück */
gdouble dmax(gdouble x, gdouble y)
{
    return MAX(x, y);
}

/* gibt den größten der n angegebenen Werte wieder */
gint max_n(gint n, ...)
{
    gint i, x, max;
    va_list ap;

    if (n <= 0)
        return 0;

    va_start(ap, n);

    max = va_arg(ap, gint);

    for (i = 1; i < n; i++) {
        x = va_arg(ap, gint);
        if (x > max)
            max = x;
    }

    va_end(ap);

    return max;
}

/* gibt den größten der n angegebenen Werte wieder */
gdouble fmax_n(gint n, gdouble *num)
{
    gint i;
    gdouble max;

    if (n <= 0)
        return 0;

    max = num[0];

    for (i = 1; i < n; i++) {
        if (num[i] > max)
            max = num[i];
    }

    return max;
}

/* gibt den Wert, der am nächsten an den n angegebenen
   Werten liegt, wieder */
gdouble closest_val(gdouble val, gint n, ...)
{
    gdouble x, diff, cl_x, min_diff;
    gint i;
    va_list ap;

    if (n <= 0)
        return 0.0;

    va_start(ap, n);

    cl_x = va_arg(ap, gdouble);
    min_diff = ABS(val - cl_x);

    for (i = 1; i < n; i++) {
        x = va_arg(ap, gdouble);
        diff = ABS(val - x);
        if (diff < min_diff) {
            cl_x = x;
            min_diff = diff;
        }
    }

    va_end(ap);

    return cl_x;
}

/* entfernt ein Element aus einer Zahlen-Liste */
void nlist_remove(gulong *array, gulong index, gulong len)
{
    /* guint i;

    for (i = index + 1; i < len; i++)
        array[i - 1] = array[i]; */

    array[index] = array[len - 1];
}

/* FIXME */
void widget_modify_bg(GtkWidget *widget, GdkColor *color)
{
    gtk_widget_modify_bg(widget, GTK_STATE_NORMAL, color);
}
