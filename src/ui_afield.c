#include <gtk/gtk.h>

#include "ui_afield.h"
#include "atoms.h"

static gboolean init(GtkWidget *widget,
                                GdkEventConfigure *event)
{
    GdkPixmap *pixmap;
    GdkColor *color1, *color2;
    GdkGC *state1, *state2;

    pixmap = gdk_pixmap_new(widget->window,
                            widget->allocation.width,
                            widget->allocation.height,
                            -1);

    gdk_draw_rectangle(pixmap,
                       widget->style->white_gc,
                       TRUE,
                       0, 0,
                       widget->allocation.width,
                       widget->allocation.height);

    state1 = gdk_gc_new(widget->window);
    state2 = gdk_gc_new(widget->window);

    color1 = (GdkColor *) g_malloc(sizeof(GdkColor));
    color2 = (GdkColor *) g_malloc(sizeof(GdkColor));

    color1->red = 100 * (65535/255);
    color1->green = 123 * (65535/255);
    color1->blue = 100 * (65535/255);
    color1->pixel = (gulong) (100 * 65536 + 123 * 256 + 100);
    
    color2->red = 13 * (65535/255);
    color2->green = 200 * (65535/255);
    color2->blue = 55 * (65535/255);
    color2->pixel = (gulong) (13 * 65536 + 200 * 256 + 55);

    gdk_gc_set_foreground(state1, color1);
    gdk_gc_set_foreground(state2, color2);

    g_object_set_data(G_OBJECT(widget), "pixmap", pixmap);
    g_object_set_data(G_OBJECT(widget), "state1", state1);
    g_object_set_data(G_OBJECT(widget), "state2", state2);

    return TRUE;
}

static gboolean resize(GtkWidget *widget,
                                GdkEventConfigure *event)
{
/*    GdkPixmap *pixmap;

    if (pixmap != NULL)
        g_object_unref(pixmap);

    pixmap = gdk_pixmap_new(widget->window,
                            widget->allocation.width,
                            widget->allocation.height,
                            -1);

    g_object_set_data(G_OBJECT(widget), "pixmap", pixmap); */

    return TRUE;
}

static gboolean redraw(GtkWidget *widget,
                             GdkEventExpose *event)
{
    GdkPixmap *pixmap;

    pixmap = g_object_get_data(G_OBJECT(widget), "pixmap");

    gdk_draw_drawable(widget->window,
                      widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
                      pixmap,
                      event->area.x, event->area.y,
                      event->area.x, event->area.y,
                      event->area.width, event->area.height);

    return FALSE;
}

GtkWidget *create_atom_field(GtkWidget *parent_box, gdouble width, gdouble height)
{
    GtkWidget *top, *afield;

    top = gtk_widget_get_toplevel(parent_box);

    afield = gtk_drawing_area_new();
    gtk_widget_set_size_request(afield, width, height);

    g_signal_connect(G_OBJECT(afield), "realize",
                     G_CALLBACK(init), NULL);

    g_signal_connect(G_OBJECT(afield), "expose_event",
                     G_CALLBACK(redraw), NULL);

    g_signal_connect(G_OBJECT(afield), "configure_event",
                     G_CALLBACK(resize), NULL);

    gtk_box_pack_start(GTK_BOX(parent_box), afield, FALSE, FALSE, 20);

    g_object_set_data(G_OBJECT(top), "atom_field", afield);

    gtk_widget_show(afield);

    return afield;
}

void clear_atom_field(GtkWidget *afield)
{
    GdkPixmap *pixmap;

    pixmap = g_object_get_data(G_OBJECT(afield), "pixmap");

    gdk_draw_rectangle(pixmap,
                       afield->style->white_gc,
                       TRUE,
                       0, 0,
                       afield->allocation.width,
                       afield->allocation.height);

    gtk_widget_queue_draw_area(afield,
                               0, 0,
                               afield->allocation.width,
                               afield->allocation.height);
}

void draw_atom(GtkWidget *afield, AtomList *atom, gdouble wide)
{
    GdkPixmap *pixmap;
    GdkGC *state1, *state2;
    gdouble real_wide, wide_diff;

    pixmap = g_object_get_data(G_OBJECT(afield), "pixmap");
    state1 = g_object_get_data(G_OBJECT(afield), "state1");
    state2 = g_object_get_data(G_OBJECT(afield), "state2");

    real_wide = wide * 0.8;
    wide_diff = (wide - real_wide) / 2;

    if (atom->state == 0) {
        gdk_draw_rectangle(pixmap,
                           state1,
                           TRUE,
                           atom->x + wide_diff, atom->y + wide_diff,
                           real_wide, real_wide);
    } else {
        gdk_draw_rectangle(pixmap,
                           state2,
                           TRUE,
                           atom->x + wide_diff, atom->y + wide_diff,
                           real_wide, real_wide);
    }

    gtk_widget_queue_draw_area(afield,
                               atom->x + wide_diff, atom->y + wide_diff,
                               real_wide, real_wide);
}
