void av_bprint_append_data(AVBPrint *buf, const char *data, unsigned size)
{
    unsigned room, real_n;
    while (1) {
        room = av_bprint_room(buf);
        if (size < room)
            break;
        if (av_bprint_alloc(buf, size))
            break;
    }
    if (room) {
        real_n = FFMIN(size, room - 1);
        memcpy(buf->str + buf->len, data, real_n);
    }
    av_bprint_grow(buf, size);
}
