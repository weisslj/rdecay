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
#define GET_TIME(t) (GetTickCount() / 1000.0)
#else
#define GET_TIME(t) (t.tv_sec + ((gdouble) t.tv_usec / USEC_PER_SEC))
#endif

/* erstellt einen neuen Timer und startet ihn */
MyTimer* timer_new(void)
{
    MyTimer *timer;
#ifndef OS_WINDOWS
    struct timeval tval;
#endif

    timer = (MyTimer *) g_malloc(sizeof(MyTimer));
    timer->active = TRUE;

#ifndef OS_WINDOWS
    gettimeofday(&tval, NULL);
    timer->start = timer->diff = GET_TIME(tval);
#else
    timer->start = timer->diff = GET_TIME();
#endif
    timer->delay = 0;

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
#ifndef OS_WINDOWS
    struct timeval tval;
#endif

#ifndef OS_WINDOWS
    gettimeofday(&tval, NULL);
    timer->delay += GET_TIME(tval) - timer->stop;
#else
    timer->delay += GET_TIME() - timer->stop;
#endif
    timer->diff = timer->start + timer->delay;

    timer->active = TRUE;
}

/* stoppt den Timer */
void timer_stop(MyTimer *timer)
{
#ifndef OS_WINDOWS
    struct timeval tval;
#endif

#ifndef OS_WINDOWS
    gettimeofday(&tval, NULL);
    timer->stop = GET_TIME(tval);
#else
    timer->stop = GET_TIME();
#endif

    timer->active = FALSE;
}

/* setzt den Timer auf Null zurück */
void timer_reset(MyTimer *timer)
{
#ifndef OS_WINDOWS
    struct timeval tval;
#endif

#ifndef OS_WINDOWS
    gettimeofday(&tval, NULL);
    timer->start = GET_TIME(tval) - timer->delay;
#else
    timer->start = GET_TIME() - timer->delay;
#endif

    timer->diff = timer->start + timer->delay;
}

/* gibt die Zeit in Sekunden zurück, die seit dem Start vergangen ist */
gdouble timer_elapsed(MyTimer *timer)
{
#ifndef OS_WINDOWS
    struct timeval tval;
#endif

    if (timer->active) {
#ifndef OS_WINDOWS
        gettimeofday(&tval, NULL);
        return GET_TIME(tval) - timer->diff;
#else
        return GET_TIME() - timer->diff;
#endif
    } else
        return timer->stop - timer->diff;
}

/* überprüft, ob der Timer gerade läuft */
gboolean timer_is_running(MyTimer *timer)
{
    return timer->active;
}
