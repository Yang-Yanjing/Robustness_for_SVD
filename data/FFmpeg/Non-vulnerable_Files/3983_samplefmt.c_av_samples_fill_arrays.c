int av_samples_fill_arrays(uint8_t **audio_data, int *linesize,
                           const uint8_t *buf, int nb_channels, int nb_samples,
                           enum AVSampleFormat sample_fmt, int align)
{
    int ch, planar, buf_size, line_size;
    planar   = av_sample_fmt_is_planar(sample_fmt);
    buf_size = av_samples_get_buffer_size(&line_size, nb_channels, nb_samples,
                                          sample_fmt, align);
    if (buf_size < 0)
        return buf_size;
    audio_data[0] = (uint8_t *)buf;
    for (ch = 1; planar && ch < nb_channels; ch++)
        audio_data[ch] = audio_data[ch-1] + line_size;
    if (linesize)
        *linesize = line_size;
    return buf_size;
}
