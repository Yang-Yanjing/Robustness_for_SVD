AVFILTER_DEFINE_CLASS(kerndeint);
static av_cold void uninit(AVFilterContext *ctx)
{
    KerndeintContext *kerndeint = ctx->priv;
    av_freep(&kerndeint->tmp_data[0]);
}
