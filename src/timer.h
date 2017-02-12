/* 
 * timer.h - Zeitmessung
 *
 * Copyright 2004-2017 Johannes Weißl
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
 * along with rdecay.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TIMER_H
#define _TIMER_H

#include <glib.h>

/* Mikrosekunden pro Sekunde */
#define USEC_PER_SEC 1000000

typedef struct _MyTimer MyTimer;

struct _MyTimer
{
    gdouble start;
    gdouble stop;
    gdouble delay;
    gdouble diff;

    gdouble speed;
    gdouble speed_to_set;
    gdouble speed_offset;
    gdouble speed_diff;
    gdouble speed_set;
    gdouble speed_set_changed;

    gboolean active;
};


MyTimer* timer_new(gdouble speed);
void timer_destroy(MyTimer *timer);
void timer_start(MyTimer *timer);
void timer_stop(MyTimer *timer);
void timer_reset(MyTimer *timer);
gdouble timer_elapsed(MyTimer *timer);
gboolean timer_is_running(MyTimer *timer);
void timer_set_speed(MyTimer *timer, gdouble speed);

#endif /* _TIMER_H */
