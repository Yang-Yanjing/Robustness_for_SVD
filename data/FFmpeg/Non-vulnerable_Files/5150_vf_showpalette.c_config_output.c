static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    const ShowPaletteContext *s = ctx->priv;
    outlink->w = outlink->h = 16 * s->size;
    return 0;
}
