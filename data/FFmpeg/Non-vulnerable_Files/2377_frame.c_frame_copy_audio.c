static int frame_copy_audio(AVFrame *dst, const AVFrame *src)
{
    int planar   = av_sample_fmt_is_planar(dst->format);
    int channels = dst->channels;
    int planes   = planar ? channels : 1;
    int i;
    if (dst->nb_samples     != src->nb_samples ||
        dst->channels       != src->channels ||
        dst->channel_layout != src->channel_layout)
        return AVERROR(EINVAL);
    CHECK_CHANNELS_CONSISTENCY(src);
    for (i = 0; i < planes; i++)
        if (!dst->extended_data[i] || !src->extended_data[i])
            return AVERROR(EINVAL);
    av_samples_copy(dst->extended_data, src->extended_data, 0, 0,
                    dst->nb_samples, channels, dst->format);
    return 0;
}
