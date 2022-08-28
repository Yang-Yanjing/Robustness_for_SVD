int avpriv_vsnprintf(char *s, size_t n, const char *fmt,
                     va_list ap)
{
    int ret;
    va_list ap_copy;
    if (n == 0)
        return _vscprintf(fmt, ap);
    else if (n > INT_MAX)
        return AVERROR(EOVERFLOW);
    





    memset(s, 0, n);
    va_copy(ap_copy, ap);
    ret = _vsnprintf(s, n - 1, fmt, ap_copy);
    va_end(ap_copy);
    if (ret == -1)
        ret = _vscprintf(fmt, ap);
    return ret;
}
