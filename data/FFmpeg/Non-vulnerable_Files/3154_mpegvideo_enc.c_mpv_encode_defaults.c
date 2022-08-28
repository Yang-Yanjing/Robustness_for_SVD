static void mpv_encode_defaults(MpegEncContext *s)
{
    int i;
    ff_mpv_common_defaults(s);
    for (i = -16; i < 16; i++) {
        default_fcode_tab[i + MAX_MV] = 1;
    }
    s->me.mv_penalty = default_mv_penalty;
    s->fcode_tab     = default_fcode_tab;
    s->input_picture_number  = 0;
    s->picture_in_gop_number = 0;
}
