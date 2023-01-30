}
unsigned vlc_CPU (void)
{
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once (&once, vlc_CPU_init);
    return cpu_flags;
}
#else /* CPU_FLAGS */
unsigned vlc_CPU (void)
{
    return 0;
}
