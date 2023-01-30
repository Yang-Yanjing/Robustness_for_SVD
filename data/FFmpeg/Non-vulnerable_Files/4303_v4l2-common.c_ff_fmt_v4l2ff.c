enum AVPixelFormat ff_fmt_v4l2ff(uint32_t v4l2_fmt, enum AVCodecID codec_id)
{
    int i;
    for (i = 0; ff_fmt_conversion_table[i].codec_id != AV_CODEC_ID_NONE; i++) {
        if (ff_fmt_conversion_table[i].v4l2_fmt == v4l2_fmt &&
            ff_fmt_conversion_table[i].codec_id == codec_id) {
            return ff_fmt_conversion_table[i].ff_fmt;
        }
    }
    return AV_PIX_FMT_NONE;
}
