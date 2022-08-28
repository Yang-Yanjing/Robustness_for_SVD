static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    LUT3DContext *lut3d = ctx->priv;
    int ret;
    outlink->w = ctx->inputs[0]->w;
    outlink->h = ctx->inputs[0]->h;
    outlink->time_base = ctx->inputs[0]->time_base;
    if ((ret = ff_dualinput_init(ctx, &lut3d->dinput)) < 0)
        return ret;
    return 0;
}
