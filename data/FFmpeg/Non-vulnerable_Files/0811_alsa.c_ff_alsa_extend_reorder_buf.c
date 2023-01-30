int ff_alsa_extend_reorder_buf(AlsaData *s, int min_size)
{
    int size = s->reorder_buf_size;
    void *r;
    av_assert0(size != 0);
    while (size < min_size)
        size *= 2;
    r = av_realloc_array(s->reorder_buf, size, s->frame_size);
    if (!r)
        return AVERROR(ENOMEM);
    s->reorder_buf = r;
    s->reorder_buf_size = size;
    return 0;
}
