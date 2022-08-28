static av_cold int test_init(AVFilterContext *ctx)
{
    TestSourceContext *test = ctx->priv;
    test->fill_picture_fn = test_fill_picture;
    return init(ctx);
}
