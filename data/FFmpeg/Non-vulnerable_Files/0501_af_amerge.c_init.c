static av_cold int init(AVFilterContext *ctx)
{
    AMergeContext *s = ctx->priv;
    int i;
    s->in = av_calloc(s->nb_inputs, sizeof(*s->in));
    if (!s->in)
        return AVERROR(ENOMEM);
    for (i = 0; i < s->nb_inputs; i++) {
        char *name = av_asprintf("in%d", i);
        AVFilterPad pad = {
            .name             = name,
            .type             = AVMEDIA_TYPE_AUDIO,
            .filter_frame     = filter_frame,
        };
        if (!name)
            return AVERROR(ENOMEM);
        ff_insert_inpad(ctx, i, &pad);
    }
    return 0;
}
