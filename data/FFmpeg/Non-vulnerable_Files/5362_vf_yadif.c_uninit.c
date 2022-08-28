static av_cold void uninit(AVFilterContext *ctx)
{
    YADIFContext *yadif = ctx->priv;
    av_frame_free(&yadif->prev);
    av_frame_free(&yadif->cur );
    av_frame_free(&yadif->next);
}
