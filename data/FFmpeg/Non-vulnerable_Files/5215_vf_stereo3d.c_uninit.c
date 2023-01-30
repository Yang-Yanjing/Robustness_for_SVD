static av_cold void uninit(AVFilterContext *ctx)
{
    Stereo3DContext *s = ctx->priv;
    av_frame_free(&s->prev);
}
