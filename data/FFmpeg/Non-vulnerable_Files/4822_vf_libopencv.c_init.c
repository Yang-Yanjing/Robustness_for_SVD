static av_cold int init(AVFilterContext *ctx)
{
    OCVContext *s = ctx->priv;
    int i;
    if (!s->name) {
        av_log(ctx, AV_LOG_ERROR, "No libopencv filter name specified\n");
        return AVERROR(EINVAL);
    }
    for (i = 0; i < FF_ARRAY_ELEMS(ocv_filter_entries); i++) {
        const OCVFilterEntry *entry = &ocv_filter_entries[i];
        if (!strcmp(s->name, entry->name)) {
            s->init             = entry->init;
            s->uninit           = entry->uninit;
            s->end_frame_filter = entry->end_frame_filter;
            if (!(s->priv = av_mallocz(entry->priv_size)))
                return AVERROR(ENOMEM);
            return s->init(ctx, s->params);
        }
    }
    av_log(ctx, AV_LOG_ERROR, "No libopencv filter named '%s'\n", s->name);
    return AVERROR(EINVAL);
}
