#ifndef _OPT_H
#define _OPT_H

#include <glib.h>

void opt_init(gint *argc, gchar **argv[]);
void opt_free(void);
gdouble opt_get_fps(void);
gboolean opt_get_showfps(void);

#endif /* _OPT_H */
