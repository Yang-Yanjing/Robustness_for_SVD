enum AVSampleFormat av_get_alt_sample_fmt(enum AVSampleFormat sample_fmt, int planar)
{
    if (sample_fmt < 0 || sample_fmt >= AV_SAMPLE_FMT_NB)
        return AV_SAMPLE_FMT_NONE;
    if (sample_fmt_info[sample_fmt].planar == planar)
        return sample_fmt;
    return sample_fmt_info[sample_fmt].altform;
}
