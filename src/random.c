#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef RND_DEVICE
#include <stdio.h>
#elif MS_CRYPTO_API
#include <windows.h>
#include <wincrypt.h>
#endif

#include <time.h>
#include "random.h"

unsigned long int get_random_seed(void)
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
