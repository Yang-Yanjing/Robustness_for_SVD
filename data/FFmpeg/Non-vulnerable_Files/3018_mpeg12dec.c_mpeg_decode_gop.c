static void mpeg_decode_gop(AVCodecContext *avctx,
                            const uint8_t *buf, int buf_size)
{
    Mpeg1Context *s1  = avctx->priv_data;
    MpegEncContext *s = &s1->mpeg_enc_ctx;
    int broken_link;
    int64_t tc;
    init_get_bits(&s->gb, buf, buf_size * 8);
    tc = avctx->timecode_frame_start = get_bits(&s->gb, 25);
    s->closed_gop = get_bits1(&s->gb);
    


    broken_link = get_bits1(&s->gb);
    if (s->avctx->debug & FF_DEBUG_PICT_INFO) {
        char tcbuf[AV_TIMECODE_STR_SIZE];
        av_timecode_make_mpeg_tc_string(tcbuf, tc);
        av_log(s->avctx, AV_LOG_DEBUG,
               "GOP (%s) closed_gop=%d broken_link=%d\n",
               tcbuf, s->closed_gop, broken_link);
    }
}
