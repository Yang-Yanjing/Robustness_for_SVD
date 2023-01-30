static av_cold void uninit(AVFilterContext *ctx)
{
    int i;
    PaletteGenContext *s = ctx->priv;
    for (i = 0; i < HIST_SIZE; i++)
        av_freep(&s->histogram[i].entries);
    av_freep(&s->refs);
    av_frame_free(&s->prev_frame);
}
