#include <stdio.h>
#include <math.h>
#include <glib.h>

#define ATOM_STATES 3

typedef struct _AtomList {
    gint8 state;
    gdouble x;
    gdouble y;
} AtomList;

typedef struct _Atoms {
    gint32 number;
    gint32 states[ATOM_STATES];
    gdouble wide;
    AtomList *list;
} Atoms;

Atoms *create_atoms(gint32 number)
{
    Atoms *atoms;

    atoms = (Atoms *) g_malloc(sizeof(Atoms));
    atoms->list = (AtomList *) g_malloc(number * sizeof(AtomList));

    atoms->number = number;
    atoms->states[0] = number;
    atoms->states[1] = 0;
    atoms->states[2] = 0;

    return atoms;
}

gdouble gauss(gdouble x, gdouble sigma)
{
    gdouble d;
    d = (exp((-1.0 * x * x) / (2.0 * sigma * sigma)) / (sigma * sqrt(2.0 * 3.141592652)));
    fprintf(stderr, "DEBUG: d = %.3f\n", d);
    return d;
}

void destroy_atoms(Atoms *atoms)
{
    g_free(atoms->list);
    g_free(atoms);
}

gint32 decay_atoms(gint32 n, gdouble time, gdouble htime, GRand *rand)
{
    gdouble p, mu, sigma;

    p = exp(-1.0 * (log(2.0) / htime) * time);
    mu = p * n;
    sigma = pow((p * (1.0 - p) * n), p);
    n = (gint32) (gauss(mu, sigma) + 0.5);
    fprintf(stderr, "DEBUG: p = %.3f\n", p);
    return n;
}

gint main(gint argc, gchar *argv[])
{
    GRand *rand;
    GTimer *timer;
    gdouble halftime, ctime, starttime;
    gint32 number, i, nnew;
    Atoms *atoms;

    ctime = starttime = 0;

    if (argc != 3) {
        printf("Usage: zerfall NUMBER HALFTIME\n");
        exit(1);
    }

    number = (gint32) g_ascii_strtod(argv[1], NULL);
    halftime = g_ascii_strtod(argv[2], NULL);

    atoms = create_atoms(number);

    rand = g_rand_new();

    timer = g_timer_new();

    printf("state0\t\t\t\tstate1\t\t\t\tctime\t\tstarttime\n");
    printf("-----------------------------------------------------------------------------------------------------\n");

    starttime = g_timer_elapsed(timer, NULL);

    while (atoms->states[0] > 0) {
        ctime = g_timer_elapsed(timer, NULL);

        nnew = decay_atoms(atoms->states[0], (ctime - starttime), halftime, rand);
        if (nnew != atoms->states[0]) {
            atoms->states[1] += (atoms->states[0] - nnew);
            atoms->states[0] = nnew;
            printf("%d\t\t%.2f%%\t\t%d\t\t%.2f%%\t\t%.4f\t\t%.6f\n",
                   atoms->states[0],
                   100.0 * ((gdouble) atoms->states[0] / (gdouble) atoms->number),
                   atoms->states[1],
                   100.0 * ((gdouble) atoms->states[1] / (gdouble) atoms->number),
                   ctime,
                   starttime);
        }
        
    }

    printf("%d\t\t%.2f%%\t\t%d\t\t%.2f%%\t\t%.4f\t\t%.6f\n",
           atoms->states[0],
           100.0 * ((gdouble) atoms->states[0] / (gdouble) atoms->number),
           atoms->states[1],
           100.0 * ((gdouble) atoms->states[1] / (gdouble) atoms->number),
           ctime,
           starttime);

    g_timer_stop(timer);
    g_timer_destroy(timer);

    g_rand_free(rand);

    destroy_atoms(atoms);
    return 0;
}
