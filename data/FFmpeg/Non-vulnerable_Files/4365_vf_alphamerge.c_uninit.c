static av_cold void uninit(AVFilterContext *ctx)
{
    AlphaMergeContext *merge = ctx->priv;
    ff_bufqueue_discard_all(&merge->queue_main);
    ff_bufqueue_discard_all(&merge->queue_alpha);
}
