/* 
 * timer.c - Zeitmessung
 *
 * Copyright 2004 Johannes Weißl
 *
 * This file is part of rdecay.
 *
 * rdecay is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * rdecay is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rdecay; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

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

#ifdef OS_WINDOWS

#define TIME DWORD

#define ZERO_TIME(x) x = 0;

#define GET_TIME(v) v = GetTickCount();

#define SUBTR_TIME(z, x, y) z = (x) - (y);

#define ADD_TIME(z, x, y) z = (x) + (y);

#define TIME_TO_DOUBLE(d, t, type) d = (type) (t) / 1000.0;

#else

#define TIME struct timeval

#define ZERO_TIME(x) \
x.tv_sec = 0; \
x.tv_usec = 0;

#define GET_TIME(v) gettimeofday(&(v), NULL);

#define SUBTR_TIME(z, x, y) \
if (y.tv_usec > x.tv_usec) { \
    x.tv_usec += USEC_PER_SEC; \
    x.tv_sec--; \
} \
z.tv_usec = x.tv_usec - y.tv_usec; \
z.tv_sec = x.tv_sec - y.tv_sec;

#define ADD_TIME(z, x, y) \
z.tv_usec = x.tv_usec + y.tv_usec; \
z.tv_sec = x.tv_sec + y.tv_sec; \
if (z.tv_usec >= USEC_PER_SEC) { \
    z.tv_usec -= USEC_PER_SEC; \
    z.tv_sec++; \
}

#define TIME_TO_NUMBER(d, t, type) \
d = t.tv_sec + ((type) t.tv_usec / USEC_PER_SEC); \
if (d < 0) \
    d = 0;

#endif

struct _MyTimer
{
    TIME start;
    TIME stop;
    TIME diff;

    gboolean active;
};

/* erstellt einen neuen Timer und startet ihn */
MyTimer* timer_new(void)
{
    MyTimer *timer;

    timer = (MyTimer *) g_malloc(sizeof(MyTimer));
    timer->active = TRUE;

    GET_TIME(timer->start)
    ZERO_TIME(timer->diff)

    return timer;
}

/* beendet und zerstört den Timer */
void timer_destroy(MyTimer *timer)
{
    g_free(timer);
}

/* startet den Timer */
void timer_start(MyTimer *timer)
{
    TIME tmp;

    GET_TIME(tmp)
    SUBTR_TIME(tmp, tmp, timer->stop)
    ADD_TIME(timer->diff, timer->diff, tmp)

    timer->active = TRUE;
}

/* stoppt den Timer */
void timer_stop(MyTimer *timer)
{
    GET_TIME(timer->stop)

    timer->active = FALSE;
}

/* setzt den Timer auf Null zurück */
void timer_reset(MyTimer *timer)
{
    GET_TIME(timer->start)
    SUBTR_TIME(timer->start, timer->start, timer->diff);
}

#if 0
/* gibt die Zeit in Sekunden zurück, die seit dem Start vergangen ist */
gdouble timer_elapsed(MyTimer *timer)
{
    gdouble total;
#ifndef OS_WINDOWS
    struct timeval elapsed;
#endif

#ifdef OS_WINDOWS
    if (timer->active)
        timer->end = GetTickCount();

    /* überprüft auf eventuellen Umbruch, der alle 49,7 Tage passiert */
    if (timer->end < timer->start)
        total = (UINT_MAX - (timer->start - timer->end - 1)) / 1000.0;
    else
        total = (timer->end - timer->start) / 1000.0;
#else
    if (timer->active)
        gettimeofday(&timer->end, NULL);

    if (timer->start.tv_usec > timer->end.tv_usec) {
        timer->end.tv_usec += USEC_PER_SEC;
        timer->end.tv_sec--;
    }

    elapsed.tv_usec = timer->end.tv_usec - timer->start.tv_usec;
    elapsed.tv_sec = timer->end.tv_sec - timer->start.tv_sec;

    total = elapsed.tv_sec + ((gdouble) elapsed.tv_usec / 1e6);
    if (total < 0)
        total = 0;
#endif

    return total;
}
#endif

/* gibt die Zeit in Sekunden zurück, die seit dem Start vergangen ist */
gdouble timer_elapsed(MyTimer *timer)
{
    gdouble total;
    TIME tmp;

    if (timer->active) {
        GET_TIME(tmp)
        SUBTR_TIME(tmp, tmp, timer->diff)
    } else {
        SUBTR_TIME(tmp, timer->stop, timer->diff)
    }

    SUBTR_TIME(tmp, tmp, timer->start);
    TIME_TO_NUMBER(total, tmp, gdouble)

    return total;
}

/* überprüft, ob der Timer gerade läuft */
gboolean timer_is_running(MyTimer *timer)
{
    return timer->active;
}
