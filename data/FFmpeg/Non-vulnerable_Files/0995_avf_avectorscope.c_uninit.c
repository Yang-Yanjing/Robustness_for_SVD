static av_cold void uninit(AVFilterContext *ctx)
{
    AudioVectorScopeContext *s = ctx->priv;
    av_frame_free(&s->outpicref);
}
