static av_cold void uninit(AVFilterContext *ctx)
{
    PullupContext *s = ctx->priv;
    int i;
    free_field_queue(s->head);
    s->last = NULL;
    for (i = 0; i < FF_ARRAY_ELEMS(s->buffers); i++) {
        av_freep(&s->buffers[i].planes[0]);
        av_freep(&s->buffers[i].planes[1]);
        av_freep(&s->buffers[i].planes[2]);
    }
}
