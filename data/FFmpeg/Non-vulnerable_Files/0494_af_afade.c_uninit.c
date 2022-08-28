static av_cold void uninit(AVFilterContext *ctx)
{
    AudioFadeContext *s = ctx->priv;
    av_audio_fifo_free(s->fifo[0]);
    av_audio_fifo_free(s->fifo[1]);
}
