#include <glib.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef OS_WINDOWS
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#else
#include <windows.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "timer.h"

struct _MyTimer
{
    struct timeval start;
    struct timeval stop;
    struct timeval diff;

    gboolean active;
};


MyTimer* timer_new(void)
{
    MyTimer *timer;

    timer = (MyTimer *) g_malloc(sizeof(MyTimer));
    timer->active = (!(0));

    gettimeofday(&(timer->start), ((void *)0));

    timer->diff.tv_sec = 0;
    timer->diff.tv_usec = 0;

    return timer;
}


void timer_destroy(MyTimer *timer)
{
    g_free(timer);
}


void timer_start(MyTimer *timer)
{
    struct timeval tmp;

    gettimeofday(&(tmp), ((void *)0));
    if (timer->stop.tv_usec > tmp.tv_usec) { tmp.tv_usec += 1000000; tmp.tv_sec--; } tmp.tv_usec = tmp.tv_usec - timer->stop.tv_usec; tmp.tv_sec = tmp.tv_sec - timer->stop.tv_sec;
    timer->diff.tv_usec = timer->diff.tv_usec + tmp.tv_usec; timer->diff.tv_sec = timer->diff.tv_sec + tmp.tv_sec; if (timer->diff.tv_usec >= 1000000) { timer->diff.tv_usec -= 1000000; timer->diff.tv_sec++; }

    timer->active = (!(0));
}


void timer_stop(MyTimer *timer)
{
    gettimeofday(&(timer->stop), ((void *)0));

    timer->active = (0);
}


void timer_reset(MyTimer *timer)
{
    gettimeofday(&(timer->start), ((void *)0));
}

gdouble timer_elapsed(MyTimer *timer)
{
    gdouble total;
    struct timeval tmp;

    if (timer->active) {
        gettimeofday(&(tmp), ((void *)0));
        if (timer->diff.tv_usec > tmp.tv_usec) {
            tmp.tv_usec += 1000000;
            tmp.tv_sec--;
        }
        tmp.tv_usec = tmp.tv_usec - timer->diff.tv_usec;
        tmp.tv_sec = tmp.tv_sec - timer->diff.tv_sec;
    } else {
        if (timer->diff.tv_usec > timer->stop.tv_usec) {
            timer->stop.tv_usec += 1000000;
            timer->stop.tv_sec--;
        }
        tmp.tv_usec = timer->stop.tv_usec - timer->diff.tv_usec;
        tmp.tv_sec = timer->stop.tv_sec - timer->diff.tv_sec;
    }

    total = tmp.tv_sec + ((gdouble) tmp.tv_usec / 1000000);
    if (total < 0)
        total = 0;

    return total;
}


gboolean timer_is_running(MyTimer *timer)
{
    return timer->active;
}
