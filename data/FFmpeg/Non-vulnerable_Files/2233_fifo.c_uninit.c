static av_cold void uninit(AVFilterContext *ctx)
{
    FifoContext *fifo = ctx->priv;
    Buf *buf, *tmp;
    for (buf = fifo->root.next; buf; buf = tmp) {
        tmp = buf->next;
        av_frame_free(&buf->frame);
        av_free(buf);
    }
    av_frame_free(&fifo->out);
}
