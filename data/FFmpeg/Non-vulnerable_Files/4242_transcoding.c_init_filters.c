static int init_filters(void)
{
    const char *filter_spec;
    unsigned int i;
    int ret;
    filter_ctx = av_malloc_array(ifmt_ctx->nb_streams, sizeof(*filter_ctx));
    if (!filter_ctx)
        return AVERROR(ENOMEM);
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        filter_ctx[i].buffersrc_ctx  = NULL;
        filter_ctx[i].buffersink_ctx = NULL;
        filter_ctx[i].filter_graph   = NULL;
        if (!(ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO
                || ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO))
            continue;
        if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            filter_spec = "null"; 
        else
            filter_spec = "anull"; 
        ret = init_filter(&filter_ctx[i], ifmt_ctx->streams[i]->codec,
                ofmt_ctx->streams[i]->codec, filter_spec);
        if (ret)
            return ret;
    }
    return 0;
}
