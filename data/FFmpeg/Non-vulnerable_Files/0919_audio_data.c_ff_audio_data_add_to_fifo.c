int ff_audio_data_add_to_fifo(AVAudioFifo *af, AudioData *a, int offset,
                              int nb_samples)
{
    uint8_t *offset_data[AVRESAMPLE_MAX_CHANNELS];
    int offset_size, p;
    if (offset >= a->nb_samples)
        return 0;
    offset_size = offset * a->stride;
    for (p = 0; p < a->planes; p++)
        offset_data[p] = a->data[p] + offset_size;
    return av_audio_fifo_write(af, (void **)offset_data, nb_samples);
}
