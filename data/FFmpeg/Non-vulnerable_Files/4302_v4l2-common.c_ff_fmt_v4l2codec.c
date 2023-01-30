enum AVCodecID ff_fmt_v4l2codec(uint32_t v4l2_fmt)
{
    int i;
    for (i = 0; ff_fmt_conversion_table[i].codec_id != AV_CODEC_ID_NONE; i++) {
        if (ff_fmt_conversion_table[i].v4l2_fmt == v4l2_fmt) {
            return ff_fmt_conversion_table[i].codec_id;
        }
    }
    return AV_CODEC_ID_NONE;
}
