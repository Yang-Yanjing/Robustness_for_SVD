static av_always_inline void encode_mb(MpegEncContext *s, int motion_x, int motion_y)
{
    if (s->chroma_format == CHROMA_420) encode_mb_internal(s, motion_x, motion_y,  8, 8, 6);
    else if (s->chroma_format == CHROMA_422) encode_mb_internal(s, motion_x, motion_y, 16, 8, 8);
    else encode_mb_internal(s, motion_x, motion_y, 16, 16, 12);
}
