static av_cold void uninit(AVFilterContext *ctx)
{
    AudioEchoContext *s = ctx->priv;
    av_freep(&s->delay);
    av_freep(&s->decay);
    av_freep(&s->samples);
    if (s->delayptrs)
        av_freep(&s->delayptrs[0]);
    av_freep(&s->delayptrs);
}
