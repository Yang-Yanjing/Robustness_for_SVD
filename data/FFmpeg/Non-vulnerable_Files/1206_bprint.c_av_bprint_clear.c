void av_bprint_clear(AVBPrint *buf)
{
    if (buf->len) {
        *buf->str = 0;
        buf->len  = 0;
    }
}
