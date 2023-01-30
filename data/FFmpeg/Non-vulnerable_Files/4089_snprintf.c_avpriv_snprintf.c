int avpriv_snprintf(char *s, size_t n, const char *fmt, ...)
{
    va_list ap;
    int ret;
    va_start(ap, fmt);
    ret = avpriv_vsnprintf(s, n, fmt, ap);
    va_end(ap);
    return ret;
}
