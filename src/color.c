#include <gtk/gtk.h>

#include "color.h"

#define RGB (65535 / 255)

GdkColor *color_new(GtkWidget *widget, gint red, gint green, gint blue)
{
    GdkColor *color;

    color = (GdkColor *) g_malloc(sizeof(GdkColor));

    color->red = red * RGB;
    color->green = green * RGB;
    color->blue = blue * RGB;

    color->pixel = (gulong) (red * 65536 + green * 256 + blue);

    gdk_color_alloc(gtk_widget_get_colormap(widget), color);

    return color;
}

void color_free(GdkColor *color)
{
    gdk_color_free(color);
}
