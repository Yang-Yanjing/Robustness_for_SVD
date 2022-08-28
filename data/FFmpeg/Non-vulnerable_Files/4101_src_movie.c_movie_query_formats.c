static int movie_query_formats(AVFilterContext *ctx)
{
    MovieContext *movie = ctx->priv;
    int list[] = { 0, -1 };
    int64_t list64[] = { 0, -1 };
    int i;
    for (i = 0; i < ctx->nb_outputs; i++) {
        MovieStream *st = &movie->st[i];
        AVCodecContext *c = st->st->codec;
        AVFilterLink *outlink = ctx->outputs[i];
        switch (c->codec_type) {
        case AVMEDIA_TYPE_VIDEO:
            list[0] = c->pix_fmt;
            ff_formats_ref(ff_make_format_list(list), &outlink->in_formats);
            break;
        case AVMEDIA_TYPE_AUDIO:
            list[0] = c->sample_fmt;
            ff_formats_ref(ff_make_format_list(list), &outlink->in_formats);
            list[0] = c->sample_rate;
            ff_formats_ref(ff_make_format_list(list), &outlink->in_samplerates);
            list64[0] = c->channel_layout;
            ff_channel_layouts_ref(avfilter_make_format64_list(list64),
                                   &outlink->in_channel_layouts);
            break;
        }
    }
    return 0;
}
