static av_cold int init(AVFilterContext *ctx)
{
    FifoContext *fifo = ctx->priv;
    fifo->last = &fifo->root;
    return 0;
}
