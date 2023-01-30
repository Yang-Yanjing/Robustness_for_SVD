static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    FlangerContext *s = ctx->priv;
    s->max_samples = (s->delay_min + s->delay_depth) * inlink->sample_rate + 2.5;
    s->lfo_length  = inlink->sample_rate / s->speed;
    s->delay_last  = av_calloc(inlink->channels, sizeof(*s->delay_last));
    s->lfo         = av_calloc(s->lfo_length, sizeof(*s->lfo));
    if (!s->lfo || !s->delay_last)
        return AVERROR(ENOMEM);
    ff_generate_wave_table(s->wave_shape, AV_SAMPLE_FMT_FLT, s->lfo, s->lfo_length,
                           floor(s->delay_min * inlink->sample_rate + 0.5),
                           s->max_samples - 2., 3 * M_PI_2);
    return av_samples_alloc_array_and_samples(&s->delay_buffer, NULL,
                                              inlink->channels, s->max_samples,
                                              inlink->format, 0);
}
