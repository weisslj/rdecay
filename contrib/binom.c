/*
 * Copyright (C) 2004 Felix Wallner
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

double
get_nideal(int n0, int t, int thalf)
{
        return (double) n0 * pow(.5, (double) t / (double) thalf);
}

int
get_dif(int n, int tstep, int thalf, gsl_rng *rng)
{
        double p;

        p = 1. - pow(.5, (double) tstep / (double) thalf);

        return gsl_ran_binomial(rng, p, n);
}

int
main(int argc, char **argv)
{
        gsl_rng *rng;
        int n0, nelm_rnd, thalf, tstep, t = 0;
        double nelm_ideal, err = 0, err_sum = 0;

        if(argc != 4) {
                printf("usage: %s nelm halflife steptime\n", argv[0]);
                exit(1);
        }

        n0 = atoi(argv[1]);
        thalf = atoi(argv[2]);
        tstep = atoi(argv[3]);

        rng = gsl_rng_alloc(gsl_rng_taus);
        gsl_rng_set(rng, time(NULL));

        nelm_rnd = n0;
        nelm_ideal = (double) n0;

        while(nelm_ideal >= 1  && nelm_rnd >= 1) {
                printf("t = %d; ideal: %f random: %d error: %f%%\n",
                       t, nelm_ideal = get_nideal(n0, t, thalf),
                       nelm_rnd, err * 100.);

                nelm_rnd -= get_dif(nelm_rnd, tstep, thalf, rng);

                err = (nelm_ideal - (double) nelm_rnd) / nelm_ideal;
                err_sum += err;

                t += tstep;
        }

        printf("avarage error: %f\n", err_sum / (1 + (double) (t / tstep)));

        gsl_rng_free(rng);

        exit(0);
        return 0;
}
