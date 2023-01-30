static int add_to_queue(AVFilterLink *inlink, AVFrame *frame)
{
    FifoContext *fifo = inlink->dst->priv;
    fifo->last->next = av_mallocz(sizeof(Buf));
    if (!fifo->last->next) {
        av_frame_free(&frame);
        return AVERROR(ENOMEM);
    }
    fifo->last = fifo->last->next;
    fifo->last->frame = frame;
    return 0;
}
