void av_bprint_get_buffer(AVBPrint *buf, unsigned size,
                          unsigned char **mem, unsigned *actual_size)
{
    if (size > av_bprint_room(buf))
        av_bprint_alloc(buf, size);
    *actual_size = av_bprint_room(buf);
    *mem = *actual_size ? buf->str + buf->len : NULL;
}
