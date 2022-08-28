char *av_stristr(const char *s1, const char *s2)
{
    if (!*s2)
        return (char*)(intptr_t)s1;
    do
        if (av_stristart(s1, s2, NULL))
            return (char*)(intptr_t)s1;
    while (*s1++);
    return NULL;
}
