uint32_t ff_fmt_ff2v4l(enum AVPixelFormat pix_fmt, enum AVCodecID codec_id)
{
    int i;
    for (i = 0; ff_fmt_conversion_table[i].codec_id != AV_CODEC_ID_NONE; i++) {
        if ((codec_id == AV_CODEC_ID_NONE ||
             ff_fmt_conversion_table[i].codec_id == codec_id) &&
            (pix_fmt == AV_PIX_FMT_NONE ||
             ff_fmt_conversion_table[i].ff_fmt == pix_fmt)) {
            return ff_fmt_conversion_table[i].v4l2_fmt;
        }
    }
    return 0;
}
