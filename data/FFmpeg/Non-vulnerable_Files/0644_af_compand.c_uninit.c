static av_cold void uninit(AVFilterContext *ctx)
{
    CompandContext *s = ctx->priv;
    av_freep(&s->channels);
    av_freep(&s->segments);
    av_frame_free(&s->delay_frame);
}
