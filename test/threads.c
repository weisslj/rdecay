#include <stdio.h>
#include <glib-2.0/glib.h>

static void print_a(void)
{
    g_usleep(G_USEC_PER_SEC * 3);
    printf("a\n");
}

static void print_b(void)
{
    printf("b\n");
}

int main()
{
    GThread *a, *b;
    g_thread_init(NULL);
    a = g_thread_create((GThreadFunc) print_a, NULL, TRUE, NULL);
    b = g_thread_create((GThreadFunc) print_b, NULL, TRUE, NULL);

    g_thread_join(a);
    g_thread_join(b);
    return 0;
}
