static int request_frame(AVFilterLink *link)
{
    BufferSourceContext *c = link->src->priv;
    AVFrame *frame;
    if (!av_fifo_size(c->fifo)) {
        if (c->eof)
            return AVERROR_EOF;
        c->nb_failed_requests++;
        return AVERROR(EAGAIN);
    }
    av_fifo_generic_read(c->fifo, &frame, sizeof(frame), NULL);
    return ff_filter_frame(link, frame);
}
