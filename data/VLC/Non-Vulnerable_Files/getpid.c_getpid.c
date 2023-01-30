#endif
pid_t getpid (void)
{
#if defined (_WIN32)
    return (pid_t) GetCurrentProcessId ();
#elif defined (__native_client__)
    return 1;
#else
# error Unimplemented!
#endif
}
