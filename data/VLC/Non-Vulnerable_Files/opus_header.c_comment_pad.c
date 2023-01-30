/* adds padding so that metadata can be updated without rewriting the whole file */
static int comment_pad(char **comments, size_t *length)
{
    const unsigned padding = 512; /* default from opus-tools */
    if(SIZE_MAX - *length < padding + 255)
        return 1;
    char *p = *comments;
    /* Make sure there is at least "padding" worth of padding free, and
       round up to the maximum that fits in the current ogg segments. */
    size_t newlen = ((*length + padding) / 255 + 1) * 255 - 1;
    p = realloc(p, newlen);
    if (p == NULL)
        return 1;
    memset(p + *length, 0, newlen - *length);
    *comments = p;
    *length = newlen;
    return 0;
}
