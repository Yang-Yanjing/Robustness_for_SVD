static av_cold int nullsrc_init(AVFilterContext *ctx)
{
    TestSourceContext *test = ctx->priv;
    test->fill_picture_fn = nullsrc_fill_picture;
    return init(ctx);
}
