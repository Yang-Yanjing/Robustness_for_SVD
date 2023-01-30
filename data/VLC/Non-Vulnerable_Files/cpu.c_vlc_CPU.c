 */
unsigned vlc_CPU (void)
{
/* On Windows and OS/2,
 * initialized from DllMain() and _DLL_InitTerm() respectively, instead */
#if !defined(_WIN32) && !defined(__OS2__)
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once (&once, vlc_CPU_init);
#endif
    return cpu_flags;
}
