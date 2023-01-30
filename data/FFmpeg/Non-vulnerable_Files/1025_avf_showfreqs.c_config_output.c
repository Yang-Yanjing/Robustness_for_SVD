static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = ctx->inputs[0];
    ShowFreqsContext *s = ctx->priv;
    float overlap;
    int i;
    s->nb_freq = 1 << (s->fft_bits - 1);
    s->win_size = s->nb_freq << 1;
    av_audio_fifo_free(s->fifo);
    av_fft_end(s->fft);
    s->fft = av_fft_init(s->fft_bits, 0);
    if (!s->fft) {
        av_log(ctx, AV_LOG_ERROR, "Unable to create FFT context. "
               "The window size might be too high.\n");
        return AVERROR(ENOMEM);
    }
    


    for (i = 0; i < s->nb_channels; i++) {
        av_freep(&s->fft_data[i]);
        av_freep(&s->avg_data[i]);
    }
    av_freep(&s->fft_data);
    av_freep(&s->avg_data);
    s->nb_channels = inlink->channels;
    s->fft_data = av_calloc(s->nb_channels, sizeof(*s->fft_data));
    if (!s->fft_data)
        return AVERROR(ENOMEM);
    s->avg_data = av_calloc(s->nb_channels, sizeof(*s->avg_data));
    if (!s->fft_data)
        return AVERROR(ENOMEM);
    for (i = 0; i < s->nb_channels; i++) {
        s->fft_data[i] = av_calloc(s->win_size, sizeof(**s->fft_data));
        s->avg_data[i] = av_calloc(s->nb_freq, sizeof(**s->avg_data));
        if (!s->fft_data[i] || !s->avg_data[i])
            return AVERROR(ENOMEM);
    }
    
    s->window_func_lut = av_realloc_f(s->window_func_lut, s->win_size,
                                      sizeof(*s->window_func_lut));
    if (!s->window_func_lut)
        return AVERROR(ENOMEM);
    generate_window_func(s->window_func_lut, s->win_size, s->win_func, &overlap);
    if (s->overlap == 1.)
        s->overlap = overlap;
    s->skip_samples = (1. - s->overlap) * s->win_size;
    if (s->skip_samples < 1) {
        av_log(ctx, AV_LOG_ERROR, "overlap %f too big\n", s->overlap);
        return AVERROR(EINVAL);
    }
    for (s->scale = 0, i = 0; i < s->win_size; i++) {
        s->scale += s->window_func_lut[i] * s->window_func_lut[i];
    }
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    outlink->frame_rate = av_make_q(inlink->sample_rate, s->win_size * (1.-s->overlap));
    outlink->sample_aspect_ratio = (AVRational){1,1};
    outlink->w = s->w;
    outlink->h = s->h;
    s->fifo = av_audio_fifo_alloc(inlink->format, inlink->channels, s->win_size);
    if (!s->fifo)
        return AVERROR(ENOMEM);
    return 0;
}
