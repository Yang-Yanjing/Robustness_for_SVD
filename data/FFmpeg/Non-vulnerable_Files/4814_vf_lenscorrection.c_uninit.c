static av_cold void uninit(AVFilterContext *ctx)
{
    LenscorrectionCtx *rect = ctx->priv;
    int i;
    for (i = 0; i < FF_ARRAY_ELEMS(rect->correction); i++) {
        av_freep(&rect->correction[i]);
    }
}
