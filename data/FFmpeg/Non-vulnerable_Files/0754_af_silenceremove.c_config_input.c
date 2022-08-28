static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    SilenceRemoveContext *s = ctx->priv;
    s->window_size = (inlink->sample_rate / 50) * inlink->channels;
    s->window = av_malloc_array(s->window_size, sizeof(*s->window));
    if (!s->window)
        return AVERROR(ENOMEM);
    clear_rms(s);
    s->start_duration = av_rescale(s->start_duration, inlink->sample_rate,
                                   AV_TIME_BASE);
    s->stop_duration  = av_rescale(s->stop_duration, inlink->sample_rate,
                                   AV_TIME_BASE);
    s->start_holdoff = av_malloc_array(FFMAX(s->start_duration, 1),
                                       sizeof(*s->start_holdoff) *
                                       inlink->channels);
    if (!s->start_holdoff)
        return AVERROR(ENOMEM);
    s->start_holdoff_offset = 0;
    s->start_holdoff_end    = 0;
    s->start_found_periods  = 0;
    s->stop_holdoff = av_malloc_array(FFMAX(s->stop_duration, 1),
                                      sizeof(*s->stop_holdoff) *
                                      inlink->channels);
    if (!s->stop_holdoff)
        return AVERROR(ENOMEM);
    s->stop_holdoff_offset = 0;
    s->stop_holdoff_end    = 0;
    s->stop_found_periods  = 0;
    if (s->start_periods)
        s->mode = SILENCE_TRIM;
    else
        s->mode = SILENCE_COPY;
    return 0;
}
