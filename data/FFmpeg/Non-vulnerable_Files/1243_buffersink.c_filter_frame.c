static int filter_frame(AVFilterLink *link, AVFrame *frame)
{
    AVFilterContext *ctx = link->dst;
    BufferSinkContext *buf = link->dst->priv;
    int ret;
    if ((ret = add_buffer_ref(ctx, frame)) < 0)
        return ret;
    if (buf->warning_limit &&
        av_fifo_size(buf->fifo) / FIFO_INIT_ELEMENT_SIZE >= buf->warning_limit) {
        av_log(ctx, AV_LOG_WARNING,
               "%d buffers queued in %s, something may be wrong.\n",
               buf->warning_limit,
               (char *)av_x_if_null(ctx->name, ctx->filter->name));
        buf->warning_limit *= 10;
    }
    return 0;
}
