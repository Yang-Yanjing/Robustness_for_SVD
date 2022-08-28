static int config_props(AVFilterLink *inlink)
{
    PixdescTestContext *priv = inlink->dst->priv;
    priv->pix_desc = av_pix_fmt_desc_get(inlink->format);
    av_freep(&priv->line);
    if (!(priv->line = av_malloc_array(sizeof(*priv->line), inlink->w)))
        return AVERROR(ENOMEM);
    return 0;
}
