int av_usleep(unsigned usec)
{
#if HAVE_NANOSLEEP
    struct timespec ts = { usec / 1000000, usec % 1000000 * 1000 };
    while (nanosleep(&ts, &ts) < 0 && errno == EINTR);
    return 0;
#elif HAVE_USLEEP
    return usleep(usec);
#elif HAVE_SLEEP
    Sleep(usec / 1000);
    return 0;
#else
    return AVERROR(ENOSYS);
#endif
}
