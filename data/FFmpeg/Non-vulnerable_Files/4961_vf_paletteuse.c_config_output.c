static int config_output(AVFilterLink *outlink)
{
    int ret;
    AVFilterContext *ctx = outlink->src;
    PaletteUseContext *s = ctx->priv;
    outlink->w = ctx->inputs[0]->w;
    outlink->h = ctx->inputs[0]->h;
    outlink->time_base = ctx->inputs[0]->time_base;
    if ((ret = ff_dualinput_init(ctx, &s->dinput)) < 0)
        return ret;
    return 0;
}
