/* 
 * random.c - Zufallszahlen
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

#include "random.h"

#include <time.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef RND_DEVICE
#include <stdio.h>
#elif MS_CRYPTO_API
#include <windows.h>
#include <wincrypt.h>
#endif

/* Gibt eine mehr oder weniger zufällige Zahl zurück:
   Je nach Konfiguration beim Kompilieren entweder vom einem virtuellen
   Zufallsgerät (z.B. /dev/urandom) oder den Zufallsfunktionen aus der
   Windows Crypto API. Beide benutzen dazu Nutzereingaben wie z.B. die
   Mausbewegung. Falls beides nicht vorhanden ist, wird einfach die
   aktuelle Zeit wiedergegeben */
unsigned long int random_get_seed(void)
{
    unsigned long int seed;

#ifdef RND_DEVICE
    int use_time = 0;
    FILE *rnd;
#elif MS_CRYPTO_API
    int use_time = 0;
    HCRYPTPROV h_prov;
    BYTE buf[sizeof(unsigned long int)];
#else
    int use_time = 1;
#endif

#ifdef RND_DEVICE
    rnd = fopen(RND_DEVICE, "rb");

    if (rnd == NULL)
        use_time = 1;
    else {
        if (fread(&seed, sizeof(seed), 1, rnd) != 1)
            use_time = 1;
        fclose(rnd);
    }
#elif MS_CRYPTO_API
    if (!CryptAcquireContext(&h_prov, NULL, NULL, PROV_RSA_FULL,
                    (CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET))
            && !CryptAcquireContext(&h_prov, NULL, NULL, PROV_RSA_FULL,
                    (CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET |
                            CRYPT_NEWKEYSET)))
        use_time = 1;
    else {
        if (!CryptGenRandom(h_prov, sizeof(seed), buf))
            use_time = 1;
        seed = *((unsigned long int *) buf);
        CryptReleaseContext(h_prov, 0);
    }
#endif

    if (use_time)
        seed = (unsigned long int) time(NULL);

    return seed;
}
