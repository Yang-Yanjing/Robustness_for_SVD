AVDictionary **setup_find_stream_info_opts(AVFormatContext *s,
                                           AVDictionary *codec_opts)
{
    int i;
    AVDictionary **opts;
    if (!s->nb_streams)
        return NULL;
    opts = av_mallocz_array(s->nb_streams, sizeof(*opts));
    if (!opts) {
        av_log(NULL, AV_LOG_ERROR,
               "Could not alloc memory for stream options.\n");
        return NULL;
    }
    for (i = 0; i < s->nb_streams; i++)
        opts[i] = filter_codec_opts(codec_opts, s->streams[i]->codecpar->codec_id,
                                    s, s->streams[i], NULL);
    return opts;
}
