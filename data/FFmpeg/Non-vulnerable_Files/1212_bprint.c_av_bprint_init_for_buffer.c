void av_bprint_init_for_buffer(AVBPrint *buf, char *buffer, unsigned size)
{
    buf->str      = buffer;
    buf->len      = 0;
    buf->size     = size;
    buf->size_max = size;
    *buf->str = 0;
}
