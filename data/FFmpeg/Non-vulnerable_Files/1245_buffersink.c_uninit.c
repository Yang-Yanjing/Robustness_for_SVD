static av_cold void uninit(AVFilterContext *ctx)
{
    BufferSinkContext *sink = ctx->priv;
    AVFrame *frame;
    if (sink->audio_fifo)
        av_audio_fifo_free(sink->audio_fifo);
    if (sink->fifo) {
        while (av_fifo_size(sink->fifo) >= FIFO_INIT_ELEMENT_SIZE) {
            av_fifo_generic_read(sink->fifo, &frame, sizeof(frame), NULL);
            av_frame_free(&frame);
        }
        av_fifo_freep(&sink->fifo);
    }
}
