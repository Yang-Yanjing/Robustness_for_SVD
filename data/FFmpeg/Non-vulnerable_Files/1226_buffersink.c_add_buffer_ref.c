static int add_buffer_ref(AVFilterContext *ctx, AVFrame *ref)
{
    BufferSinkContext *buf = ctx->priv;
    if (av_fifo_space(buf->fifo) < FIFO_INIT_ELEMENT_SIZE) {
        
        if (av_fifo_realloc2(buf->fifo, av_fifo_size(buf->fifo) * 2) < 0) {
            av_log(ctx, AV_LOG_ERROR,
                   "Cannot buffer more frames. Consume some available frames "
                   "before adding new ones.\n");
            return AVERROR(ENOMEM);
        }
    }
    
    av_fifo_generic_write(buf->fifo, &ref, FIFO_INIT_ELEMENT_SIZE, NULL);
    return 0;
}
