int av_audio_fifo_write(AVAudioFifo *af, void **data, int nb_samples)
{
    int i, ret, size;
    
    if (av_audio_fifo_space(af) < nb_samples) {
        int current_size = av_audio_fifo_size(af);
        
        if (INT_MAX / 2 - current_size < nb_samples)
            return AVERROR(EINVAL);
        
        if ((ret = av_audio_fifo_realloc(af, 2 * (current_size + nb_samples))) < 0)
            return ret;
    }
    size = nb_samples * af->sample_size;
    for (i = 0; i < af->nb_buffers; i++) {
        ret = av_fifo_generic_write(af->buf[i], data[i], size, NULL);
        if (ret != size)
            return AVERROR_BUG;
    }
    af->nb_samples += nb_samples;
    return nb_samples;
}
