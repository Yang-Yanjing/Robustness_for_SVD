static void rtsp_parse_range_npt(const char *p, int64_t *start, int64_t *end)
{
    char buf[256];
    p += strspn(p, SPACE_CHARS);
    if (!av_stristart(p, "npt=", &p))
        return;
    *start = AV_NOPTS_VALUE;
    *end = AV_NOPTS_VALUE;
    get_word_sep(buf, sizeof(buf), "-", &p);
    if (av_parse_time(start, buf, 1) < 0)
        return;
    if (*p == '-') {
        p++;
        get_word_sep(buf, sizeof(buf), "-", &p);
        if (av_parse_time(end, buf, 1) < 0)
            av_log(NULL, AV_LOG_DEBUG, "Failed to parse interval end specification '%s'\n", buf);
    }
}
