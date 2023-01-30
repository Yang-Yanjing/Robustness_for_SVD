}
static int64_t strtoi (const char *str)
{
    char *end;
    long long l;
    errno = 0;
    l = strtoll (str, &end, 0);
    if (!errno)
    {
#if (LLONG_MAX > 0x7fffffffffffffffLL)
        if (l > 0x7fffffffffffffffLL
         || l < -0x8000000000000000LL)
            errno = ERANGE;
#endif
        if (*end)
            errno = EINVAL;
    }
    return l;
}
