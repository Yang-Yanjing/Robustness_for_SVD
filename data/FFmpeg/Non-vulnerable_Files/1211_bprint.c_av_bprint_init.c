void av_bprint_init(AVBPrint *buf, unsigned size_init, unsigned size_max)
{
    unsigned size_auto = (char *)buf + sizeof(*buf) -
                         buf->reserved_internal_buffer;
    if (size_max == 1)
        size_max = size_auto;
    buf->str      = buf->reserved_internal_buffer;
    buf->len      = 0;
    buf->size     = FFMIN(size_auto, size_max);
    buf->size_max = size_max;
    *buf->str = 0;
    if (size_init > buf->size)
        av_bprint_alloc(buf, size_init - 1);
}
