void av_vbprintf(AVBPrint *buf, const char *fmt, va_list vl_arg)
{
    unsigned room;
    char *dst;
    int extra_len;
    va_list vl;
    while (1) {
        room = av_bprint_room(buf);
        dst = room ? buf->str + buf->len : NULL;
        va_copy(vl, vl_arg);
        extra_len = vsnprintf(dst, room, fmt, vl);
        va_end(vl);
        if (extra_len <= 0)
            return;
        if (extra_len < room)
            break;
        if (av_bprint_alloc(buf, extra_len))
            break;
    }
    av_bprint_grow(buf, extra_len);
}
