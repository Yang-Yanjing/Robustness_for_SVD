static av_cold int init(AVFilterContext *ctx)
{
    StackContext *s = ctx->priv;
    int i, ret;
    if (!strcmp(ctx->filter->name, "vstack"))
        s->is_vertical = 1;
    s->frames = av_calloc(s->nb_inputs, sizeof(*s->frames));
    if (!s->frames)
        return AVERROR(ENOMEM);
    for (i = 0; i < s->nb_inputs; i++) {
        AVFilterPad pad = { 0 };
        pad.type = AVMEDIA_TYPE_VIDEO;
        pad.name = av_asprintf("input%d", i);
        if (!pad.name)
            return AVERROR(ENOMEM);
        pad.filter_frame = filter_frame;
        if ((ret = ff_insert_inpad(ctx, i, &pad)) < 0) {
            av_freep(&pad.name);
            return ret;
        }
    }
    return 0;
}
