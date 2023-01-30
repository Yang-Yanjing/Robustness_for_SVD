static av_cold int haldclutsrc_init(AVFilterContext *ctx)
{
    TestSourceContext *hc = ctx->priv;
    hc->fill_picture_fn = haldclutsrc_fill_picture;
    hc->draw_once = 1;
    return init(ctx);
}
