static av_cold int rgbtest_init(AVFilterContext *ctx)
{
    TestSourceContext *test = ctx->priv;
    test->draw_once = 1;
    test->fill_picture_fn = rgbtest_fill_picture;
    return init(ctx);
}
