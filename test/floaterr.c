#include <stdio.h>
#include <math.h>
#include <time.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

static int decay_poisson(double t, int n, double thalf, gsl_rng *rand)
{
    return gsl_ran_poisson(rand, ((1.0 - pow(0.5, (t / thalf)))) * n);
}

static int decay_binom(double t, int n, double thalf, gsl_rng *rand)
{
    return (gsl_ran_binomial(rand, (1.0 - pow(0.5, (t / thalf))), n));
}

int main()
{
    gsl_rng *rand;
    int i, p;

    rand = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(rand, time(NULL));

    for (;;) {
        p = decay_binom(0.001, 1, 0.1, rand);
        if (p != 0 && p != 1)
            printf("%d\n", p);
    }

    gsl_rng_free(rand);
}
