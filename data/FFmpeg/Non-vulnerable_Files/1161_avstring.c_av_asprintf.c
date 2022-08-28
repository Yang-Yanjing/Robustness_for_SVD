char *av_asprintf(const char *fmt, ...)
{
    char *p = NULL;
    va_list va;
    int len;
    va_start(va, fmt);
    len = vsnprintf(NULL, 0, fmt, va);
    va_end(va);
    if (len < 0)
        goto end;
    p = av_malloc(len + 1);
    if (!p)
        goto end;
    va_start(va, fmt);
    len = vsnprintf(p, len + 1, fmt, va);
    va_end(va);
    if (len < 0)
        av_freep(&p);
end:
    return p;
}
