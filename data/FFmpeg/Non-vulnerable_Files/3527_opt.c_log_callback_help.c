static void log_callback_help(void *ptr, int level, const char *fmt, va_list vl)
{
    vfprintf(stdout, fmt, vl);
}
