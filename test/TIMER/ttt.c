#include <glib.h>
#include <stdio.h>
#include "timer.h"

int main()
{
    gint i;
    gdouble x;
    MyTimer *timer;

    timer = timer_new();

    for (i = 0; i < 100000000; i++) {
        x = timer_elapsed(timer);
        if (i == 10000) {
            timer_stop(timer);
        } else if (i == 100000) {
            timer_start(timer);
        } else if (i == 1000000) {
            timer_stop(timer);
        } else if (i == 10000000) {
            timer_start(timer);
        } else if (i == 17000000) {
            timer_reset(timer);
        }
    }

    printf("%f\n", x);

    timer_destroy(timer);

    return 0;
}
