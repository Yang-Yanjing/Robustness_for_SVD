static av_cold void uninit(AVFilterContext *ctx)
{
    FrameRateContext *s = ctx->priv;
    int i;
    for (i = s->frst + 1; i > s->last; i++) {
        if (s->srce[i] && (s->srce[i] != s->srce[i + 1]))
            av_frame_free(&s->srce[i]);
    }
    av_frame_free(&s->srce[s->last]);
}
