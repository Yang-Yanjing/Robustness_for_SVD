static av_cold int allrgb_init(AVFilterContext *ctx)
{
    TestSourceContext *test = ctx->priv;
    test->w = test->h = 4096;
    test->draw_once = 1;
    test->fill_picture_fn = allrgb_fill_picture;
    return init(ctx);
}
