int ff_audio_data_read_from_fifo(AVAudioFifo *af, AudioData *a, int nb_samples)
{
    int ret;
    if (a->read_only)
        return AVERROR(EINVAL);
    ret = ff_audio_data_realloc(a, nb_samples);
    if (ret < 0)
        return ret;
    ret = av_audio_fifo_read(af, (void **)a->data, nb_samples);
    if (ret >= 0)
        a->nb_samples = ret;
    return ret;
}
