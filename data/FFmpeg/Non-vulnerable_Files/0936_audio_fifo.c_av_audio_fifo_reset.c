void av_audio_fifo_reset(AVAudioFifo *af)
{
    int i;
    for (i = 0; i < af->nb_buffers; i++)
        av_fifo_reset(af->buf[i]);
    af->nb_samples = 0;
}
