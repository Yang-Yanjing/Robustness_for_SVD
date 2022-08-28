static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    ChorusContext *s = ctx->priv;
    float sum_in_volume = 1.0;
    int n;
    s->channels = outlink->channels;
    for (n = 0; n < s->num_chorus; n++) {
        int samples = (int) ((s->delays[n] + s->depths[n]) * outlink->sample_rate / 1000.0);
        int depth_samples = (int) (s->depths[n] * outlink->sample_rate / 1000.0);
        s->length[n] = outlink->sample_rate / s->speeds[n];
        s->lookup_table[n] = av_malloc(sizeof(int32_t) * s->length[n]);
        if (!s->lookup_table[n])
            return AVERROR(ENOMEM);
        ff_generate_wave_table(WAVE_SIN, AV_SAMPLE_FMT_S32, s->lookup_table[n],
                               s->length[n], 0., depth_samples, 0);
        s->max_samples = FFMAX(s->max_samples, samples);
    }
    for (n = 0; n < s->num_chorus; n++)
        sum_in_volume += s->decays[n];
    if (s->in_gain * (sum_in_volume) > 1.0 / s->out_gain)
        av_log(ctx, AV_LOG_WARNING, "output gain can cause saturation or clipping of output\n");
    s->counter = av_calloc(outlink->channels, sizeof(*s->counter));
    if (!s->counter)
        return AVERROR(ENOMEM);
    s->phase = av_calloc(outlink->channels, sizeof(*s->phase));
    if (!s->phase)
        return AVERROR(ENOMEM);
    for (n = 0; n < outlink->channels; n++) {
        s->phase[n] = av_calloc(s->num_chorus, sizeof(int));
        if (!s->phase[n])
            return AVERROR(ENOMEM);
    }
    s->fade_out = s->max_samples;
    return av_samples_alloc_array_and_samples(&s->chorusbuf, NULL,
                                              outlink->channels,
                                              s->max_samples,
                                              outlink->format, 0);
}
