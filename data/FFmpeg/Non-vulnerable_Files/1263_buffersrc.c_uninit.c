static av_cold void uninit(AVFilterContext *ctx)
{
    BufferSourceContext *s = ctx->priv;
    while (s->fifo && av_fifo_size(s->fifo)) {
        AVFrame *frame;
        av_fifo_generic_read(s->fifo, &frame, sizeof(frame), NULL);
        av_frame_free(&frame);
    }
    av_fifo_freep(&s->fifo);
}
