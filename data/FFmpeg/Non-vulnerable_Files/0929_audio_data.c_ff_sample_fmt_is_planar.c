int ff_sample_fmt_is_planar(enum AVSampleFormat sample_fmt, int channels)
{
    if (channels == 1)
        return 1;
    else
        return av_sample_fmt_is_planar(sample_fmt);
}
