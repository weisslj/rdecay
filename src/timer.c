#include <glib.h>

#include "timer.h"

MyTimer *timer_new(void)
{
    MyTimer *timer;

    timer = g_malloc(sizeof(MyTimer));
    timer->gtimer = g_timer_new();
    timer->stopped = 0;
    timer->tbuf = 0;

    return timer;
}

gdouble timer_elapsed(MyTimer *timer)
{
    /*
    if (timer->pause)
        return timer->pause;
    else
        return (g_timer_elapsed(timer->gtimer, NULL) - timer->tbuf);
    */
    if (! timer->stopped)
        g_timer_stop(timer->gtimer);

    return (g_timer_elapsed(timer->gtimer, NULL) - timer->tbuf);
}

void timer_start(MyTimer *timer)
{
    /*
    timer->tbuf += (timer_elapsed(timer) - timer->pause);
    timer->pause = 0;
    */
    g_timer_stop(timer->gtimer);
    timer->tbuf += ((g_timer_elapsed(timer->gtimer, NULL) - timer->tbuf) - timer->stopped);
    timer->stopped = 0;
}

void timer_stop(MyTimer *timer)
{
    /*
    timer->pause = timer_elapsed(timer);
    */
    g_timer_stop(timer->gtimer);
    timer->stopped = g_timer_elapsed(timer->gtimer, NULL) - timer->tbuf;
}

void timer_reset(MyTimer *timer)
{
    /*
    g_timer_start(timer->gtimer);
    timer->tbuf = 0;
    timer->pause = 0;
    */
    g_timer_start(timer->gtimer);
    timer->stopped = 1;
}

gboolean timer_is_running(MyTimer *timer)
{
    return (timer->stopped) ? FALSE : TRUE;
}

void timer_free(MyTimer *timer)
{
    g_timer_destroy(timer->gtimer);
    g_free(timer);
}
