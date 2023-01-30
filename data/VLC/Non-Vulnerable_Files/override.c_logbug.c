}
static void logbug (unsigned *pc, const char *level, const char *func,
                    const char *fmt, ...)
{
    va_list ap;
    va_start (ap, fmt);
    vlogbug (pc, level, func, fmt, ap);
    va_end (ap);
}
