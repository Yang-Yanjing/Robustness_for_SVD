char *av_get_token(const char **buf, const char *term)
{
    char *out     = av_malloc(strlen(*buf) + 1);
    char *ret     = out, *end = out;
    const char *p = *buf;
    if (!out)
        return NULL;
    p += strspn(p, WHITESPACES);
    while (*p && !strspn(p, term)) {
        char c = *p++;
        if (c == '\\' && *p) {
            *out++ = *p++;
            end    = out;
        } else if (c == '\'') {
            while (*p && *p != '\'')
                *out++ = *p++;
            if (*p) {
                p++;
                end = out;
            }
        } else {
            *out++ = c;
        }
    }
    do
        *out-- = 0;
    while (out >= end && strspn(out, WHITESPACES));
    *buf = p;
    return ret;
}
