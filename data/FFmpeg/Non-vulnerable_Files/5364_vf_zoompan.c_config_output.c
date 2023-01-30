static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    ZPContext *s = ctx->priv;
    outlink->w = s->w;
    outlink->h = s->h;
    return 0;
}
