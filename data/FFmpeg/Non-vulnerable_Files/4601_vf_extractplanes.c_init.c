static av_cold int init(AVFilterContext *ctx)
{
    ExtractPlanesContext *s = ctx->priv;
    int planes = (s->requested_planes & 0xf) | (s->requested_planes >> 4);
    int i;
    for (i = 0; i < 4; i++) {
        char *name;
        AVFilterPad pad = { 0 };
        if (!(planes & (1 << i)))
            continue;
        name = av_asprintf("out%d", ctx->nb_outputs);
        if (!name)
            return AVERROR(ENOMEM);
        s->map[ctx->nb_outputs] = i;
        pad.name = name;
        pad.type = AVMEDIA_TYPE_VIDEO;
        pad.config_props = config_output;
        ff_insert_outpad(ctx, ctx->nb_outputs, &pad);
    }
    return 0;
}
