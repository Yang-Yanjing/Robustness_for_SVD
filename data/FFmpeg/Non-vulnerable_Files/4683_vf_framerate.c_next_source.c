AVFILTER_DEFINE_CLASS(framerate);
static void next_source(AVFilterContext *ctx)
{
    FrameRateContext *s = ctx->priv;
    int i;
    ff_dlog(ctx,  "next_source()\n");
    if (s->srce[s->last] && s->srce[s->last] != s->srce[s->last-1]) {
        ff_dlog(ctx, "next_source() unlink %d\n", s->last);
        av_frame_free(&s->srce[s->last]);
    }
    for (i = s->last; i > s->frst; i--) {
        ff_dlog(ctx, "next_source() copy %d to %d\n", i - 1, i);
        s->srce[i] = s->srce[i - 1];
    }
    ff_dlog(ctx, "next_source() make %d null\n", s->frst);
    s->srce[s->frst] = NULL;
}
