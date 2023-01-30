static av_cold void uninit(AVFilterContext *ctx)
{
    FlangerContext *s = ctx->priv;
    av_freep(&s->lfo);
    av_freep(&s->delay_last);
    if (s->delay_buffer)
        av_freep(&s->delay_buffer[0]);
    av_freep(&s->delay_buffer);
}
