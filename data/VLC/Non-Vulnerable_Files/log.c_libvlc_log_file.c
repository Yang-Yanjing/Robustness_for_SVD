/*** Helpers for logging to files ***/
static void libvlc_log_file (void *data, int level, const libvlc_log_t *log,
                             const char *fmt, va_list ap)
{
    FILE *stream = data;
    flockfile (stream);
    vfprintf (stream, fmt, ap);
    fputc ('\n', stream);
    funlockfile (stream);
    (void) level; (void) log;
}
