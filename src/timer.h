#ifndef _TIMER_H
#define _TIMER_H

#include <glib.h>

struct _MyTimer {
    GTimer *gtimer;
    gdouble stopped;
    gdouble tbuf;
};

typedef struct _MyTimer MyTimer;

MyTimer *timer_new(void);
gdouble timer_elapsed(MyTimer *timer);
void timer_start(MyTimer *timer);
void timer_stop(MyTimer *timer);
void timer_reset(MyTimer *timer);
gboolean timer_is_running(MyTimer *timer);
void timer_destroy(MyTimer *timer);

#endif /* _TIMER_H */
