static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = ctx->inputs[0];
    PaletteGenContext *s = ctx->priv;
    int r;
    r = ff_request_frame(inlink);
    if (r == AVERROR_EOF && !s->palette_pushed && s->nb_refs) {
        r = ff_filter_frame(outlink, get_palette_frame(ctx));
        s->palette_pushed = 1;
        return r;
    }
    return r;
}
