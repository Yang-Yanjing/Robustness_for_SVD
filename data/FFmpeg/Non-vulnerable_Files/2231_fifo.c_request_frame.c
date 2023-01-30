static int request_frame(AVFilterLink *outlink)
{
    FifoContext *fifo = outlink->src->priv;
    int ret = 0;
    if (!fifo->root.next) {
        if ((ret = ff_request_frame(outlink->src->inputs[0])) < 0) {
            if (ret == AVERROR_EOF && outlink->request_samples)
                return return_audio_frame(outlink->src);
            return ret;
        }
        av_assert0(fifo->root.next);
    }
    if (outlink->request_samples) {
        return return_audio_frame(outlink->src);
    } else {
        ret = ff_filter_frame(outlink, fifo->root.next->frame);
        queue_pop(fifo);
    }
    return ret;
}
