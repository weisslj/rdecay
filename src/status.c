#include <stdarg.h>
#include <gtk/gtk.h>

#include "status.h"

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

static gint label_printf(GtkWidget *label, const gchar *format, ...)
{
    gchar *text;
    va_list ap;

    va_start(ap, format);
    text = g_strdup_vprintf(format, ap);
    va_end(ap);

    if (text == NULL)
        return 0;

    gtk_label_set_text(GTK_LABEL(label), text);
    g_free(text);

    return 1;
}

GtkWidget *create_status_field(GtkWidget *parent_box)
{
    GtkWidget *top, *box, *l1, *l2, *l3, *ltime;

    top = gtk_widget_get_toplevel(parent_box);

    box = gtk_vbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(parent_box), box, FALSE, FALSE, 20);

    l1 = gtk_label_new(_("mother atoms: -"));
    l2 = gtk_label_new(_("doughter atoms: -"));
    l3 = gtk_label_new(_("grandchild atoms: -"));
    ltime = gtk_label_new(_("time: -"));

    gtk_box_pack_start(GTK_BOX(box), l1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), l2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), l3, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), ltime, FALSE, FALSE, 0);

   g_object_set_data(G_OBJECT(top), "1_status", l1);
    g_object_set_data(G_OBJECT(top), "2_status", l2);
    g_object_set_data(G_OBJECT(top), "3_status", l3);
    g_object_set_data(G_OBJECT(top), "time_status", ltime);

    gtk_widget_show(l1);
    gtk_widget_show(l2);
    gtk_widget_show(l3);
    gtk_widget_show(ltime);
    gtk_widget_show(box);

    return box;
}

void update_status_atoms(GtkWidget *widget, gint n1, gint n2, gint n3)
{
    GtkWidget *top, *l1, *l2, *l3;

    top = gtk_widget_get_toplevel(widget);

    l1 = g_object_get_data(G_OBJECT(top), "1_status");
    l2 = g_object_get_data(G_OBJECT(top), "2_status");
    l3 = g_object_get_data(G_OBJECT(top), "3_status");

    label_printf(l1, _("mother atoms: %d"), n1);
    label_printf(l2, _("doughter atoms: %d"), n2);
    label_printf(l3, _("grandchild atoms: %d"), n3);
}

void update_status_time(GtkWidget *widget, gdouble time)
{
    GtkWidget *top, *ltime;

    top = gtk_widget_get_toplevel(widget);

    ltime = g_object_get_data(G_OBJECT(top), "time_status");
    label_printf(ltime, _("time: %.3f"), time);
}
