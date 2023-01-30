static av_cold int init(AVFilterContext *ctx)
{
    MixContext *s = ctx->priv;
    int i;
    for (i = 0; i < s->nb_inputs; i++) {
        char name[32];
        AVFilterPad pad = { 0 };
        snprintf(name, sizeof(name), "input%d", i);
        pad.type           = AVMEDIA_TYPE_AUDIO;
        pad.name           = av_strdup(name);
        if (!pad.name)
            return AVERROR(ENOMEM);
        pad.filter_frame   = filter_frame;
        ff_insert_inpad(ctx, i, &pad);
    }
    s->fdsp = avpriv_float_dsp_alloc(0);
    if (!s->fdsp)
        return AVERROR(ENOMEM);
    return 0;
}
