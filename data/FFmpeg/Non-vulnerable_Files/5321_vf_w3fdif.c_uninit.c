static av_cold void uninit(AVFilterContext *ctx)
{
    W3FDIFContext *s = ctx->priv;
    int i;
    av_frame_free(&s->prev);
    av_frame_free(&s->cur );
    av_frame_free(&s->next);
    for (i = 0; i < s->nb_threads; i++)
        av_freep(&s->work_line[i]);
    av_freep(&s->work_line);
}
