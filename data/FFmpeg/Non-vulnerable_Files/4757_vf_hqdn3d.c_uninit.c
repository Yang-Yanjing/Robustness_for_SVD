static av_cold void uninit(AVFilterContext *ctx)
{
    HQDN3DContext *s = ctx->priv;
    av_freep(&s->coefs[0]);
    av_freep(&s->coefs[1]);
    av_freep(&s->coefs[2]);
    av_freep(&s->coefs[3]);
    av_freep(&s->line);
    av_freep(&s->frame_prev[0]);
    av_freep(&s->frame_prev[1]);
    av_freep(&s->frame_prev[2]);
}
