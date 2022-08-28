int av_samples_alloc(uint8_t **audio_data, int *linesize, int nb_channels,
                     int nb_samples, enum AVSampleFormat sample_fmt, int align)
{
    uint8_t *buf;
    int size = av_samples_get_buffer_size(NULL, nb_channels, nb_samples,
                                          sample_fmt, align);
    if (size < 0)
        return size;
    buf = av_malloc(size);
    if (!buf)
        return AVERROR(ENOMEM);
    size = av_samples_fill_arrays(audio_data, linesize, buf, nb_channels,
                                  nb_samples, sample_fmt, align);
    if (size < 0) {
        av_free(buf);
        return size;
    }
    av_samples_set_silence(audio_data, 0, nb_samples, nb_channels, sample_fmt);
    return size;
}
