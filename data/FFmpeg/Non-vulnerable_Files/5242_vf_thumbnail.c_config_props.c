static int config_props(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    ThumbContext *thumb = ctx->priv;
    thumb->tb = inlink->time_base;
    return 0;
}
