static av_cold void uninit(AVFilterContext *ctx)
{
    SignalstatsContext *s = ctx->priv;
    av_frame_free(&s->frame_prev);
    av_frame_free(&s->frame_sat);
    av_frame_free(&s->frame_hue);
    av_freep(&s->jobs_rets);
}
