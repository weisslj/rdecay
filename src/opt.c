#include "opt.h"
#include "arg.h"

#include <glib.h>

static gdouble fps = 0;
static gboolean showfps = FALSE;

void opt_init(gint *argc, gchar **argv[])
{
    fps = arg_get_double(argc, argv, "--fps", "");
    showfps = arg_find(argc, argv, "--showfps", "");
}

void opt_free(void)
{
    return;
}

gdouble opt_get_fps(void)
{
    return fps;
}

gboolean opt_get_showfps(void)
{
    return showfps;
}
