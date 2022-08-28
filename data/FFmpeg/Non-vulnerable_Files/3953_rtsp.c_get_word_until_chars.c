static void get_word_until_chars(char *buf, int buf_size,
                                 const char *sep, const char **pp)
{
    const char *p;
    char *q;
    p = *pp;
    p += strspn(p, SPACE_CHARS);
    q = buf;
    while (!strchr(sep, *p) && *p != '\0') {
        if ((q - buf) < buf_size - 1)
            *q++ = *p;
        p++;
    }
    if (buf_size > 0)
        *q = '\0';
    *pp = p;
}
