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

#include "timer.h"

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

#ifdef OS_WINDOWS
#define TIMEVAL gint
#define PREPARE_TIMEVAL(t)
#define GET_TIME(t) (GetTickCount() / 1000.0)
#else
#define TIMEVAL struct timeval
#define PREPARE_TIMEVAL(t) gettimeofday(&t, NULL)
#define GET_TIME(t) (t.tv_sec + ((gdouble) t.tv_usec / USEC_PER_SEC))
#endif

/* erstellt einen neuen Timer und startet ihn */
MyTimer* timer_new(gdouble speed)
{
    MyTimer *timer;
    TIMEVAL tval;

    timer = (MyTimer *) g_malloc(sizeof(MyTimer));
    timer->active = TRUE;
    timer->speed = speed;

    timer->delay = 0;
    timer->speed_offset = 0;
    timer->speed_set = 0;
    timer->speed_set_changed = 0;
    timer->speed_diff = 0;

    PREPARE_TIMEVAL(tval);
    timer->start = timer->diff = GET_TIME(tval);

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
    TIMEVAL tval;

    PREPARE_TIMEVAL(tval);
    timer->delay += GET_TIME(tval) - timer->stop;

    timer->diff = timer->start + timer->delay;

    timer->active = TRUE;
}

/* stoppt den Timer */
void timer_stop(MyTimer *timer)
{
    TIMEVAL tval;

    PREPARE_TIMEVAL(tval);
    timer->stop = GET_TIME(tval);

    timer->active = FALSE;
}

/* setzt den Timer auf Null zurück */
void timer_reset(MyTimer *timer)
{
    TIMEVAL tval;

    PREPARE_TIMEVAL(tval);
    timer->start = GET_TIME(tval) - timer->delay;

    timer->diff = timer->start + timer->delay;
}

/* gibt die Zeit in Sekunden zurück, die seit dem Start vergangen ist */
gdouble timer_elapsed(MyTimer *timer)
{
    TIMEVAL tval;

    if (timer->active) {
        PREPARE_TIMEVAL(tval);
        return (GET_TIME(tval) - timer->diff) *
               timer->speed + timer->speed_diff;
    } else
        return (timer->stop - timer->diff) *
               timer->speed + timer->speed_diff;
}

/* überprüft, ob der Timer gerade läuft */
gboolean timer_is_running(MyTimer *timer)
{
    return timer->active;
}

/* setzt die neue Geschwindigkeit des Timers */
void timer_set_speed(MyTimer *timer, gdouble speed)
{
    gdouble curr, curr_changed;
    TIMEVAL tval;

    PREPARE_TIMEVAL(tval);
    curr = GET_TIME(tval) - timer->diff;

    curr_changed = timer->speed * curr + timer->speed_diff;
    timer->speed_offset += (curr_changed - timer->speed_set_changed) -
                           (curr - timer->speed_set);

    timer->speed_set = curr;
    timer->speed_set_changed = curr_changed;
    timer->speed = speed;

    timer->speed_diff = timer->speed_set - timer->speed * timer->speed_set +
                        timer->speed_offset;
}
