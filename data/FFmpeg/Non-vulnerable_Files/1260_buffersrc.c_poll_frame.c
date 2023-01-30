static int poll_frame(AVFilterLink *link)
{
    BufferSourceContext *c = link->src->priv;
    int size = av_fifo_size(c->fifo);
    if (!size && c->eof)
        return AVERROR_EOF;
    return size/sizeof(AVFrame*);
}
