 */
uint64_t NTPtime64 (void)
{
#if (_POSIX_TIMERS > 0)
    struct timespec ts;
    clock_gettime (CLOCK_REALTIME, &ts);
#else
    struct timeval tv;
    struct
    {
        uint32_t tv_sec;
        uint32_t tv_nsec;
    } ts;
    gettimeofday (&tv, NULL);
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;
#endif
    /* Convert nanoseconds to 32-bits fraction (232 picosecond units) */
    uint64_t t = (uint64_t)(ts.tv_nsec) << 32;
    t /= 1000000000;
    /* There is 70 years (incl. 17 leap ones) offset to the Unix Epoch.
     * No leap seconds during that period since they were not invented yet.
     */
    assert (t < 0x100000000);
    t |= ((UINT64_C(70) * 365 + 17) * 24 * 60 * 60 + ts.tv_sec) << 32;
    return t;
}
