static void rtsp_parse_range(int *min_ptr, int *max_ptr, const char **pp)
{
    const char *q;
    char *p;
    int v;
    q = *pp;
    q += strspn(q, SPACE_CHARS);
    v = strtol(q, &p, 10);
    if (*p == '-') {
        p++;
        *min_ptr = v;
        v = strtol(p, &p, 10);
        *max_ptr = v;
    } else {
        *min_ptr = v;
        *max_ptr = v;
    }
    *pp = p;
}
