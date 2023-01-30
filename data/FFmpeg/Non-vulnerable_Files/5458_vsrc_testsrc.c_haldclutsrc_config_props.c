static int haldclutsrc_config_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    TestSourceContext *hc = ctx->priv;
    hc->w = hc->h = hc->level * hc->level * hc->level;
    return config_props(outlink);
}
