int ff_sdp_parse(AVFormatContext *s, const char *content)
{
    RTSPState *rt = s->priv_data;
    const char *p;
    int letter, i;
    







    char buf[16384], *q;
    SDPParseState sdp_parse_state = { { 0 } }, *s1 = &sdp_parse_state;
    p = content;
    for (;;) {
        p += strspn(p, SPACE_CHARS);
        letter = *p;
        if (letter == '\0')
            break;
        p++;
        if (*p != '=')
            goto next_line;
        p++;
        
        q = buf;
        while (*p != '\n' && *p != '\r' && *p != '\0') {
            if ((q - buf) < sizeof(buf) - 1)
                *q++ = *p;
            p++;
        }
        *q = '\0';
        sdp_parse_line(s, s1, letter, buf);
    next_line:
        while (*p != '\n' && *p != '\0')
            p++;
        if (*p == '\n')
            p++;
    }
    for (i = 0; i < s1->nb_default_include_source_addrs; i++)
        av_freep(&s1->default_include_source_addrs[i]);
    av_freep(&s1->default_include_source_addrs);
    for (i = 0; i < s1->nb_default_exclude_source_addrs; i++)
        av_freep(&s1->default_exclude_source_addrs[i]);
    av_freep(&s1->default_exclude_source_addrs);
    rt->p = av_malloc_array(rt->nb_rtsp_streams + 1, sizeof(struct pollfd) * 2);
    if (!rt->p) return AVERROR(ENOMEM);
    return 0;
}
