#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <gsl/gsl_rng.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

#include "topwin.h"
#include "random.h"
#include "color.h"

int main(int argc, char *argv[])
{
    GtkWidget *window;
    gsl_rng *rand;

    GdkColor *color;
    GdkGC *style;

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    gtk_init(&argc, &argv);

    rand = gsl_rng_alloc(gsl_rng_taus);
    gsl_rng_set(rand, random_get_seed());

    window = create_top_window(rand);

    style = gdk_gc_new(window->window);
    color = color_new(window, 100, 123, 100);
    gdk_gc_set_foreground(style, color);

    gtk_main();

    gsl_rng_free(rand);
    gtk_widget_destroy(window);

    return 0;
}
