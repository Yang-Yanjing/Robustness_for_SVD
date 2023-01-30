static int skip_line(const char *p)
{
    while (*p && av_isspace(*p))
        p++;
    return !*p || *p == '#';
}
