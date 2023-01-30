void ffserver_get_arg(char *buf, int buf_size, const char **pp)
{
    const char *p;
    char *q;
    int quote = 0;
    p = *pp;
    q = buf;
    while (av_isspace(*p)) p++;
    if (*p == '\"' || *p == '\'')
        quote = *p++;
    while (*p != '\0') {
        if (quote && *p == quote || !quote && av_isspace(*p))
            break;
        if ((q - buf) < buf_size - 1)
            *q++ = *p;
        p++;
    }
    *q = '\0';
    if (quote && *p == quote)
        p++;
    *pp = p;
}
