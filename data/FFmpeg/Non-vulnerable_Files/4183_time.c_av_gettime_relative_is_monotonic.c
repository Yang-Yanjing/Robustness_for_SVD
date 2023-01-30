int av_gettime_relative_is_monotonic(void)
{
#if HAVE_CLOCK_GETTIME && defined(CLOCK_MONOTONIC)
    return 1;
#else
    return 0;
#endif
}
