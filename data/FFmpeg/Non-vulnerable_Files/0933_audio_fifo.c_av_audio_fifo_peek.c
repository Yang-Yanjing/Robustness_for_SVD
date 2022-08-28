int av_audio_fifo_peek(AVAudioFifo *af, void **data, int nb_samples)
{
    int i, ret, size;
    if (nb_samples < 0)
        return AVERROR(EINVAL);
    nb_samples = FFMIN(nb_samples, af->nb_samples);
    if (!nb_samples)
        return 0;
    size = nb_samples * af->sample_size;
    for (i = 0; i < af->nb_buffers; i++) {
        if ((ret = av_fifo_generic_peek(af->buf[i], data[i], size, NULL)) < 0)
            return AVERROR_BUG;
    }
    return nb_samples;
}
