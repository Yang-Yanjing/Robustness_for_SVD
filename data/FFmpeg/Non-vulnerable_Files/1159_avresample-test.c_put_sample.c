PUT_FUNC(dbl, AV_SAMPLE_FMT_DBL, double,  v_dbl)
static void put_sample(void **data, enum AVSampleFormat sample_fmt,
                       int channels, int sample, int ch, double v_dbl)
{
    switch (av_get_packed_sample_fmt(sample_fmt)) {
    case AV_SAMPLE_FMT_U8:
        put_sample_u8(data, sample_fmt, channels, sample, ch, v_dbl);
        break;
    case AV_SAMPLE_FMT_S16:
        put_sample_s16(data, sample_fmt, channels, sample, ch, v_dbl);
        break;
    case AV_SAMPLE_FMT_S32:
        put_sample_s32(data, sample_fmt, channels, sample, ch, v_dbl);
        break;
    case AV_SAMPLE_FMT_FLT:
        put_sample_flt(data, sample_fmt, channels, sample, ch, v_dbl);
        break;
    case AV_SAMPLE_FMT_DBL:
        put_sample_dbl(data, sample_fmt, channels, sample, ch, v_dbl);
        break;
    }
}
