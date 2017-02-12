/* 
 * main.c - die Hauptdatei
 *
 * Copyright 2004-2017 Johannes Weißl
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

#include "arg.h"
#include "opt.h"
#include "sim.h"
#include "atoms.h"
#include "gui.h"
#include "random.h"
#include "util.h"

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

static void print_version(void);
static void print_help(void);

int main(int argc, char *argv[])
{
    GtkWidget *window, *button_start;
    SimData *sdata;
    gsl_rng *rand;

    /* aktiviert Sprach- und Ländereinstellungen */
    gtk_set_locale();
    bindtextdomain(PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(PACKAGE, "UTF-8");
    textdomain(PACKAGE);

    if (arg_find(&argc, &argv, "--version", "-v")) {
        print_version();
        exit(EXIT_SUCCESS);
    }

    if (arg_find(&argc, &argv, "--help", "-h")) {
        print_help();
        exit(EXIT_SUCCESS);
    }

    /* wertet die Kommandozeilen-Optionen aus */
    opt_init(&argc, &argv);

    /* initialisiert das Toolkit (für Fenster, Buttons, etc.) */
    gtk_init(&argc, &argv);

    /* erstellt die Bedienungsoberfläche */
    window = gui_create();

    /* initialisiert den Zufallsgenerator "Mersenne Twister" mit einem
       Wert, der aus Nutzereingaben (Mausbewegung, etc.) berechnet wird */
    rand = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(rand, random_get_seed());

    /* erstellt die Simulationsstruktur */
    sdata = simdata_new(rand);

    g_signal_connect_swapped(G_OBJECT(window),
                             "destroy",
                             G_CALLBACK(gsl_rng_free),
                             rand);

    /* verknüpft den Startbutton mit der Simulationsfunktion */
    button_start = g_object_get_data(G_OBJECT(window), "button_start");
    g_signal_connect(G_OBJECT(button_start),
                     "clicked",
                     G_CALLBACK(sim_decay),
                     sdata);

    /* startet die GTK-Hauptschleife */
    gtk_main();

    return EXIT_SUCCESS;
}

/* gibt die Version aus */
static void print_version(void)
{
    printf_utf8("%s\n\n", PACKAGE_STRING);
    printf_utf8(_("Copyright (C) 2004-2017 Johannes Weissl\n\
This is free software; see the source for copying conditions.  There \
is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR \
PURPOSE.\n\n"));
    printf_utf8(_("Written by Johannes Weissl.\n"));
}

/* gibt die Hilfe aus */
static void print_help(void)
{
    printf_utf8(_("Usage: rdecay [OPTION]...\n"));
    printf_utf8(_("Simulates the radioactive decay.\n\n"));

    printf_utf8(_("Options:\n"));
    printf_utf8(_("      --fps NUMBER  set frames per second to NUMBER\n"));
    printf_utf8(_("      --showfps     print fps to stdout\n"));
    printf_utf8(_("  -h, --help        display this help and exit\n"));
    printf_utf8(_("  -v, --version     output version information and exit\n"));

    printf_utf8(_("\nReport bugs to %s.\n"), PACKAGE_BUGREPORT);
}
