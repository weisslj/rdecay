#include <stdio.h>
#include "gtk/gtk.h"

static gboolean delete_event(GtkWidget *widget, GdkEvent *event,
        gpointer data)
{
    gtk_main_quit();
    return FALSE;
}

static void *print_a(void)
{
    printf("a\n");
    return NULL;
}

static void *print_b(void)
{
    printf("b\n");
    return NULL;
}

static void t_a(void)
{
    g_usleep(G_USEC_PER_SEC * 3);
    gtk_idle_add_priority(G_PRIORITY_HIGH_IDLE, (GtkFunction) print_a, NULL);
}

static void t_b(void)
{
    gtk_idle_add_priority(G_PRIORITY_HIGH_IDLE, (GtkFunction) print_b, NULL);
}

static void testxy(void)
{
    GThread *a, *b;
    a = g_thread_create((GThreadFunc) t_a, NULL, TRUE, NULL);
    b = g_thread_create((GThreadFunc) t_b, NULL, TRUE, NULL);

/*    g_thread_join(a);
    g_thread_join(b); */
}

gint main(gint argc, gchar *argv[])
{
    GtkWidget *window, *button;

    g_thread_init(NULL);
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(G_OBJECT(window), "delete_event",
            G_CALLBACK(delete_event), NULL);

    button = gtk_button_new_with_label("Hallo");
    g_signal_connect(G_OBJECT(button), "clicked",
            G_CALLBACK(testxy), NULL);
    gtk_container_add(GTK_CONTAINER(window), button);

    gtk_widget_show(button);
    gtk_widget_show(window);

    gtk_main();

    return 0;
}
