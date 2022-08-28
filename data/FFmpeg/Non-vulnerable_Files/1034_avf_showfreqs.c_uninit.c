static av_cold void uninit(AVFilterContext *ctx)
{
    ShowFreqsContext *s = ctx->priv;
    int i;
    av_fft_end(s->fft);
    for (i = 0; i < s->nb_channels; i++) {
        av_freep(&s->fft_data[i]);
        av_freep(&s->avg_data[i]);
    }
    av_freep(&s->fft_data);
    av_freep(&s->avg_data);
    av_freep(&s->window_func_lut);
    av_audio_fifo_free(s->fifo);
}
