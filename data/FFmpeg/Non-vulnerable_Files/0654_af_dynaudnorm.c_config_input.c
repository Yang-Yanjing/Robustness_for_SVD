static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    DynamicAudioNormalizerContext *s = ctx->priv;
    int c;
    s->frame_len =
    inlink->min_samples =
    inlink->max_samples =
    inlink->partial_buf_size = frame_size(inlink->sample_rate, s->frame_len_msec);
    av_log(ctx, AV_LOG_DEBUG, "frame len %d\n", s->frame_len);
    s->fade_factors[0] = av_malloc(s->frame_len * sizeof(*s->fade_factors[0]));
    s->fade_factors[1] = av_malloc(s->frame_len * sizeof(*s->fade_factors[1]));
    s->prev_amplification_factor = av_malloc(inlink->channels * sizeof(*s->prev_amplification_factor));
    s->dc_correction_value = av_calloc(inlink->channels, sizeof(*s->dc_correction_value));
    s->compress_threshold = av_calloc(inlink->channels, sizeof(*s->compress_threshold));
    s->gain_history_original = av_calloc(inlink->channels, sizeof(*s->gain_history_original));
    s->gain_history_minimum = av_calloc(inlink->channels, sizeof(*s->gain_history_minimum));
    s->gain_history_smoothed = av_calloc(inlink->channels, sizeof(*s->gain_history_smoothed));
    s->weights = av_malloc(s->filter_size * sizeof(*s->weights));
    if (!s->prev_amplification_factor || !s->dc_correction_value ||
        !s->compress_threshold || !s->fade_factors[0] || !s->fade_factors[1] ||
        !s->gain_history_original || !s->gain_history_minimum ||
        !s->gain_history_smoothed || !s->weights)
        return AVERROR(ENOMEM);
    for (c = 0; c < inlink->channels; c++) {
        s->prev_amplification_factor[c] = 1.0;
        s->gain_history_original[c] = cqueue_create(s->filter_size);
        s->gain_history_minimum[c]  = cqueue_create(s->filter_size);
        s->gain_history_smoothed[c] = cqueue_create(s->filter_size);
        if (!s->gain_history_original[c] || !s->gain_history_minimum[c] ||
            !s->gain_history_smoothed[c])
            return AVERROR(ENOMEM);
    }
    precalculate_fade_factors(s->fade_factors, s->frame_len);
    init_gaussian_filter(s);
    s->channels = inlink->channels;
    s->delay = s->filter_size;
    return 0;
}
