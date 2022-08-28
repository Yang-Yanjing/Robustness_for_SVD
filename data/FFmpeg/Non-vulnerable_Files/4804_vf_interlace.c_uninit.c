static av_cold void uninit(AVFilterContext *ctx)
{
    InterlaceContext *s = ctx->priv;
    av_frame_free(&s->cur);
    av_frame_free(&s->next);
}
