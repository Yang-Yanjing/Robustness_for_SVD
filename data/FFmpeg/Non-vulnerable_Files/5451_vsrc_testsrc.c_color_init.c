static av_cold int color_init(AVFilterContext *ctx)
{
    TestSourceContext *test = ctx->priv;
    test->fill_picture_fn = color_fill_picture;
    test->draw_once = 1;
    return init(ctx);
}
