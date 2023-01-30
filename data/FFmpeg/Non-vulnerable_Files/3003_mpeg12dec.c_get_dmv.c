static inline int get_dmv(MpegEncContext *s)
{
    if (get_bits1(&s->gb))
        return 1 - (get_bits1(&s->gb) << 1);
    else
        return 0;
}
