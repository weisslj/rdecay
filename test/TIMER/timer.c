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

#include "timer.h"

/* erstellt einen neuen Timer und startet ihn */
MyTimer *timer_new(void)
{
    MyTimer *timer;

    timer = (MyTimer *) g_malloc(sizeof(MyTimer));
    timer->gtimer = g_timer_new();
    timer->stopped = 0;
    timer->tbuf = 0;

    return timer;
}

/* stellt den Speicher für den Timer wieder zur Verfügung */
void timer_destroy(MyTimer *timer)
{
    g_timer_destroy(timer->gtimer);
    g_free(timer);
}

/* gibt die Zeit in Sekunden zurück, die seit dem Start vergangen ist */
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

/* startet den Timer */
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

/* stoppt den Timer */
void timer_stop(MyTimer *timer)
{
    /*
    timer->pause = timer_elapsed(timer);
    */
    g_timer_stop(timer->gtimer);
    timer->stopped = g_timer_elapsed(timer->gtimer, NULL) - timer->tbuf;
}

/* setzt den Timer auf Null zurück */
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

/* überprüft, ob der Timer zur Zeit läuft */
gboolean timer_is_running(MyTimer *timer)
{
    return (timer->stopped) ? FALSE : TRUE;
}
