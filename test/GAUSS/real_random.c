#include <stdio.h>

#ifdef DEV_URANDOM
#elif MS_CRYPTO_API
#include <windows.h>
#include <wincrypt.h>
#endif
#include <time.h>

unsigned long int get_random_seed(void)
{
    unsigned long int seed;
    int use_time;

#ifdef DEV_URANDOM
    FILE *rnd;
    use_time = 0;
#elif MS_CRYPTO_API
    HCRYPTPROV h_prov;
    BYTE buf[sizeof(unsigned long int)];
    use_time = 0;
#else
    use_time = 1;
#endif

#ifdef DEV_URANDOM
    rnd = fopen(DEV_URANDOM, "rb");

    if (rnd == NULL)
        use_time = 1;
    else {
        if (fread(&seed, sizeof(seed), 1, rnd) != 1)
            use_time = 1;
        fclose(rnd);
    }
#elif MS_CRYPTO_API
    if (!CryptAcquireContext(&h_prov, NULL, NULL, PROV_RSA_FULL, (CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET)) && !CryptAcquireContext(&h_prov, NULL, NULL, PROV_RSA_FULL, (CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET | CRYPT_NEWKEYSET)))
        use_time = 1;
    else {
        if (!CryptGenRandom(h_prov, sizeof(unsigned long int), buf))
            use_time = 1;
	seed = *((unsigned long int *) buf);
        CryptReleaseContext(h_prov, 0);
    }
#endif

    if (use_time)
        seed = (unsigned long int) time(NULL);

    return seed;
}

int main()
{
    unsigned long int seed;

    seed = get_random_seed();

    printf("seed: %ld\n", seed);

    return 0;
}
