#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "topwin.h"

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    create_top_window();

    gtk_main();

    return 0;
}
