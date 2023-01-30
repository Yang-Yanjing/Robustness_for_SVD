void av_bprint_chars(AVBPrint *buf, char c, unsigned n)
{
    unsigned room, real_n;
    while (1) {
        room = av_bprint_room(buf);
        if (n < room)
            break;
        if (av_bprint_alloc(buf, n))
            break;
    }
    if (room) {
        real_n = FFMIN(n, room - 1);
        memset(buf->str + buf->len, c, real_n);
    }
    av_bprint_grow(buf, n);
}
