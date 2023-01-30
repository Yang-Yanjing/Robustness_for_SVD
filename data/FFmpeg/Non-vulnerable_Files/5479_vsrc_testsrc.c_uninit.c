static av_cold void uninit(AVFilterContext *ctx)
{
    TestSourceContext *test = ctx->priv;
    av_frame_free(&test->picref);
}
