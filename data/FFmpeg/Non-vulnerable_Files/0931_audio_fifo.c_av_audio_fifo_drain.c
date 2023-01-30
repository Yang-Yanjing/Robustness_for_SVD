int av_audio_fifo_drain(AVAudioFifo *af, int nb_samples)
{
    int i, size;
    if (nb_samples < 0)
        return AVERROR(EINVAL);
    nb_samples = FFMIN(nb_samples, af->nb_samples);
    if (nb_samples) {
        size = nb_samples * af->sample_size;
        for (i = 0; i < af->nb_buffers; i++)
            av_fifo_drain(af->buf[i], size);
        af->nb_samples -= nb_samples;
    }
    return 0;
}
