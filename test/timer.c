#include <glib.h>

int main()
{
    GTimer *timer;

    timer = g_timer_new();

    g_usleep(1000000);

    printf("1: %g\n", g_timer_elapsed(timer, NULL));

    g_timer_stop(timer);

    g_usleep(2000000);

//    g_timer_stop(timer);

    printf("2: %g\n", g_timer_elapsed(timer, NULL));

    g_timer_stop(timer);
    g_timer_destroy(timer);

    return 0;
}
