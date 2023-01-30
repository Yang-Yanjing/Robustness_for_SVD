static av_cold int common_init(AVFilterContext *ctx)
{
    BufferSinkContext *buf = ctx->priv;
    buf->fifo = av_fifo_alloc_array(FIFO_INIT_SIZE, FIFO_INIT_ELEMENT_SIZE);
    if (!buf->fifo) {
        av_log(ctx, AV_LOG_ERROR, "Failed to allocate fifo\n");
        return AVERROR(ENOMEM);
    }
    buf->warning_limit = 100;
    buf->next_pts = AV_NOPTS_VALUE;
    return 0;
}
