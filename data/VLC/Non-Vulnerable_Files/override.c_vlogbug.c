}
static void vlogbug (unsigned *pc, const char *level, const char *func,
                     const char *fmt, va_list ap)
{
#ifdef HAVE_BACKTRACE
    const size_t framec = 5;
    void *framev[framec];
    backtrace (framev, framec);
#endif
    flockfile (stderr);
    if (*pc < MAX_ERRORS)
    {
        (*pc)++;
        fprintf (stderr, "%s: call to %s(", level, func);
        vfprintf (stderr, fmt, ap);
        fputs (")\n", stderr);
        fflush (stderr);
#ifdef HAVE_BACKTRACE
        backtrace_symbols_fd (framev + 2, framec - 2, STDERR_FILENO);
#endif
    }
    funlockfile (stderr);
}
