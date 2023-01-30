AVAudioFifo *av_audio_fifo_alloc(enum AVSampleFormat sample_fmt, int channels,
                                 int nb_samples)
{
    AVAudioFifo *af;
    int buf_size, i;
    
    if (av_samples_get_buffer_size(&buf_size, channels, nb_samples, sample_fmt, 1) < 0)
        return NULL;
    af = av_mallocz(sizeof(*af));
    if (!af)
        return NULL;
    af->channels    = channels;
    af->sample_fmt  = sample_fmt;
    af->sample_size = buf_size / nb_samples;
    af->nb_buffers  = av_sample_fmt_is_planar(sample_fmt) ? channels : 1;
    af->buf = av_mallocz_array(af->nb_buffers, sizeof(*af->buf));
    if (!af->buf)
        goto error;
    for (i = 0; i < af->nb_buffers; i++) {
        af->buf[i] = av_fifo_alloc(buf_size);
        if (!af->buf[i])
            goto error;
    }
    af->allocated_samples = nb_samples;
    return af;
error:
    av_audio_fifo_free(af);
    return NULL;
}
