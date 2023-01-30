int av_samples_alloc_array_and_samples(uint8_t ***audio_data, int *linesize, int nb_channels,
                                       int nb_samples, enum AVSampleFormat sample_fmt, int align)
{
    int ret, nb_planes = av_sample_fmt_is_planar(sample_fmt) ? nb_channels : 1;
    *audio_data = av_calloc(nb_planes, sizeof(**audio_data));
    if (!*audio_data)
        return AVERROR(ENOMEM);
    ret = av_samples_alloc(*audio_data, linesize, nb_channels,
                           nb_samples, sample_fmt, align);
    if (ret < 0)
        av_freep(audio_data);
    return ret;
}
