int av_audio_fifo_space(AVAudioFifo *af)
{
    return af->allocated_samples - af->nb_samples;
}
