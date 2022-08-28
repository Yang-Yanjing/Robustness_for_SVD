static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    OverlayContext *s = ctx->priv;
    int ret;
    if ((ret = ff_dualinput_init(ctx, &s->dinput)) < 0)
        return ret;
    outlink->w = ctx->inputs[MAIN]->w;
    outlink->h = ctx->inputs[MAIN]->h;
    outlink->time_base = ctx->inputs[MAIN]->time_base;
    return 0;
}
