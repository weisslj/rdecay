#include "arg.h"

#include <glib.h>
#include <string.h>
#include <stdlib.h>

gboolean arg_find(gint *argc, gchar **argv[], const gchar *l, const gchar *s)
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

gchar *arg_get_string(gint *argc, gchar **argv[], const gchar *l, const gchar *s)
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

gint arg_get_int(gint *argc, gchar **argv[], const gchar *l, const gchar *s)
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

gdouble arg_get_double(gint *argc, gchar **argv[], const gchar *l, const gchar *s)
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

void arg_remove(gint *argc, gchar **argv[], gint pos, gint len)
{
    if (len <= (*argc - pos)) {
        do
            (*argv)[pos] = (*argv)[pos + len];
        while ((*argv)[pos++] != NULL);
        *argc -= len;
    }
}
