#ifndef _ARG_H
#define _ARG_H

#include <glib.h>

gint arg_find(gint *argc, gchar **argv[],
        const gchar *long_name, const gchar *short_name);

gchar *arg_get_string(gint *argc, gchar **argv[],
        const gchar *long_name, const gchar *short_name);
gint arg_get_int(gint *argc, gchar **argv[],
        const gchar *long_name, const gchar *short_name);
gdouble arg_get_double(gint *argc, gchar **argv[],
        const gchar *long_name, const gchar *short_name);

void arg_remove(gint *argc, gchar **argv[], gint pos, gint len);

#endif /* _ARG_H */
