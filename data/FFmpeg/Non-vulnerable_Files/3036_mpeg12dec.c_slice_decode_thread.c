static int slice_decode_thread(AVCodecContext *c, void *arg)
{
    MpegEncContext *s   = *(void **) arg;
    const uint8_t *buf  = s->gb.buffer;
    int mb_y            = s->start_mb_y;
    const int field_pic = s->picture_structure != PICT_FRAME;
    s->er.error_count = (3 * (s->end_mb_y - s->start_mb_y) * s->mb_width) >> field_pic;
    for (;;) {
        uint32_t start_code;
        int ret;
        ret = mpeg_decode_slice(s, mb_y, &buf, s->gb.buffer_end - buf);
        emms_c();
        ff_dlog(c, "ret:%d resync:%d/%d mb:%d/%d ts:%d/%d ec:%d\n",
                ret, s->resync_mb_x, s->resync_mb_y, s->mb_x, s->mb_y,
                s->start_mb_y, s->end_mb_y, s->er.error_count);
        if (ret < 0) {
            if (c->err_recognition & AV_EF_EXPLODE)
                return ret;
            if (s->resync_mb_x >= 0 && s->resync_mb_y >= 0)
                ff_er_add_slice(&s->er, s->resync_mb_x, s->resync_mb_y,
                                s->mb_x, s->mb_y,
                                ER_AC_ERROR | ER_DC_ERROR | ER_MV_ERROR);
        } else {
            ff_er_add_slice(&s->er, s->resync_mb_x, s->resync_mb_y,
                            s->mb_x - 1, s->mb_y,
                            ER_AC_END | ER_DC_END | ER_MV_END);
        }
        if (s->mb_y == s->end_mb_y)
            return 0;
        start_code = -1;
        buf        = avpriv_find_start_code(buf, s->gb.buffer_end, &start_code);
        mb_y       = start_code - SLICE_MIN_START_CODE;
        if (s->codec_id != AV_CODEC_ID_MPEG1VIDEO && s->mb_height > 2800/16)
            mb_y += (*buf&0xE0)<<2;
        mb_y <<= field_pic;
        if (s->picture_structure == PICT_BOTTOM_FIELD)
            mb_y++;
        if (mb_y < 0 || mb_y >= s->end_mb_y)
            return AVERROR_INVALIDDATA;
    }
}
