void av_audio_fifo_free(AVAudioFifo *af)
{
    if (af) {
        if (af->buf) {
            int i;
            for (i = 0; i < af->nb_buffers; i++) {
                if (af->buf[i])
                    av_fifo_free(af->buf[i]);
            }
            av_freep(&af->buf);
        }
        av_free(af);
    }
}
