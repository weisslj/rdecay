#include <gtk/gtk.h>

#include "graph.h"
#include "ui_graph.h"
#include "atoms.h"
#include "color.h"

static gboolean init(GtkWidget *widget,
                                GdkEventConfigure *event)
{
    GdkPixmap *pixmap;
    GdkColor *color1, *color2, *color3;
    GdkGC *style1, *style2, *style3;

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

    style1 = gdk_gc_new(widget->window);
    style2 = gdk_gc_new(widget->window);
    style3 = gdk_gc_new(widget->window);

    color1 = color_new(widget, 255, 0, 0);
    color2 = color_new(widget, 0, 255, 0);
    color3 = color_new(widget, 0, 0, 255);

    gdk_gc_set_foreground(style1, color1);
    gdk_gc_set_foreground(style2, color2);
    gdk_gc_set_foreground(style3, color3);

    g_object_set_data(G_OBJECT(widget), "pixmap", pixmap);
    g_object_set_data(G_OBJECT(widget), "style1", style1);
    g_object_set_data(G_OBJECT(widget), "style2", style2);
    g_object_set_data(G_OBJECT(widget), "style3", style3);

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

GtkWidget *create_graph(GtkWidget *parent_box, gdouble width, gdouble height)
{
    GtkWidget *top, *graph;

    top = gtk_widget_get_toplevel(parent_box);

    graph = gtk_drawing_area_new();
    gtk_widget_set_size_request(graph, width, height);

    g_signal_connect(G_OBJECT(graph), "realize",
                     G_CALLBACK(init), NULL);

    g_signal_connect(G_OBJECT(graph), "expose_event",
                     G_CALLBACK(redraw), NULL);

    g_signal_connect(G_OBJECT(graph), "configure_event",
                     G_CALLBACK(resize), NULL);

    gtk_box_pack_start(GTK_BOX(parent_box), graph, FALSE, FALSE, 20);

    g_object_set_data(G_OBJECT(top), "graph", graph);

    gtk_widget_show(graph);

    return graph;
}

void coord_system_draw(GtkWidget *graph, CoordSystem *coord)
{
    GdkPixmap *pixmap;

    pixmap = g_object_get_data(G_OBJECT(graph), "pixmap");

    gdk_draw_line(pixmap,
                  graph->style->black_gc,
                  coord->x_axis_begin,
                  coord->zero_y,
                  coord->x_axis_end,
                  coord->zero_y);

    gdk_draw_line(pixmap,
                  graph->style->black_gc,
                  coord->x_axis_end - 5,
                  coord->zero_y - 5,
                  coord->x_axis_end,
                  coord->zero_y);

    gdk_draw_line(pixmap,
                  graph->style->black_gc,
                  coord->x_axis_end - 5,
                  coord->zero_y + 5,
                  coord->x_axis_end,
                  coord->zero_y);

    gdk_draw_line(pixmap,
                  graph->style->black_gc,
                  coord->zero_x,
                  coord->y_axis_begin,
                  coord->zero_x,
                  coord->y_axis_end);

    gdk_draw_line(pixmap,
                  graph->style->black_gc,
                  coord->zero_x - 5,
                  coord->y_axis_end + 5,
                  coord->zero_x,
                  coord->y_axis_end);

    gdk_draw_line(pixmap,
                  graph->style->black_gc,
                  coord->zero_x + 5,
                  coord->y_axis_end + 5,
                  coord->zero_x,
                  coord->y_axis_end);

    gtk_widget_queue_draw_area(graph,
                               coord->x_axis_begin,
                               coord->y_axis_begin,
                               coord->x_axis_end,
                               coord->y_axis_end);
}

void graph_draw_line(GtkWidget *graph, CoordSystem *coord,
                     gdouble x1, gdouble y1,
                     gdouble x2, gdouble y2,
                     const gchar *style_name)
{
    GdkPixmap *pixmap;
    GdkGC *style;

    pixmap = g_object_get_data(G_OBJECT(graph), "pixmap");
    style = g_object_get_data(G_OBJECT(graph), style_name);

    coord_get_real(&x1, &y1, coord);
    coord_get_real(&x2, &y2, coord);

    gdk_draw_line(pixmap, style, x1, y1, x2, y2);
    gtk_widget_queue_draw_area(graph,
                               x1, y1,
                               x2, y2);
}
