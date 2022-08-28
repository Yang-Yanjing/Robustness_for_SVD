static av_cold int smptebars_init(AVFilterContext *ctx)
{
    TestSourceContext *test = ctx->priv;
    test->fill_picture_fn = smptebars_fill_picture;
    test->draw_once = 1;
    return init(ctx);
}
