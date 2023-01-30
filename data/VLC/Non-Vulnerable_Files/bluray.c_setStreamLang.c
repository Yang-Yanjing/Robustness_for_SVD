}
static void setStreamLang(es_format_t *p_fmt,
                          const BLURAY_STREAM_INFO *p_streams, int i_stream_count)
{
    for (int i = 0; i < i_stream_count; i++) {
        if (p_fmt->i_id == p_streams[i].pid) {
            free(p_fmt->psz_language);
            p_fmt->psz_language = strndup((const char *)p_streams[i].lang, 3);
            return;
        }
    }
}
