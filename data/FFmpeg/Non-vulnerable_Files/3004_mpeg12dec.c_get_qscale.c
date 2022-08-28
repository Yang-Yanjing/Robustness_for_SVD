static inline int get_qscale(MpegEncContext *s)
{
    int qscale = get_bits(&s->gb, 5);
    if (s->q_scale_type)
        return non_linear_qscale[qscale];
    else
        return qscale << 1;
}
