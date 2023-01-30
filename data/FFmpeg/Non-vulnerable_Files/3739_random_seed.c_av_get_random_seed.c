uint32_t av_get_random_seed(void)
{
    uint32_t seed;
#if HAVE_CRYPTGENRANDOM
    HCRYPTPROV provider;
    if (CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL,
                            CRYPT_VERIFYCONTEXT | CRYPT_SILENT)) {
        BOOL ret = CryptGenRandom(provider, sizeof(seed), (PBYTE) &seed);
        CryptReleaseContext(provider, 0);
        if (ret)
            return seed;
    }
#endif
    if (read_random(&seed, "/dev/urandom") == sizeof(seed))
        return seed;
    if (read_random(&seed, "/dev/random")  == sizeof(seed))
        return seed;
    return get_generic_seed();
}
