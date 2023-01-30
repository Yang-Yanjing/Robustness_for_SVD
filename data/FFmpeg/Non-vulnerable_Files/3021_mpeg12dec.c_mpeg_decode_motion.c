static int mpeg_decode_motion(MpegEncContext *s, int fcode, int pred)
{
    int code, sign, val, shift;
    code = get_vlc2(&s->gb, ff_mv_vlc.table, MV_VLC_BITS, 2);
    if (code == 0)
        return pred;
    if (code < 0)
        return 0xffff;
    sign  = get_bits1(&s->gb);
    shift = fcode - 1;
    val   = code;
    if (shift) {
        val  = (val - 1) << shift;
        val |= get_bits(&s->gb, shift);
        val++;
    }
    if (sign)
        val = -val;
    val += pred;
    
    return sign_extend(val, 5 + shift);
}
