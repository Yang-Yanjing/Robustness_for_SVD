static int config_output(AVFilterLink *outlink)
{
    AudioStatsContext *s = outlink->src->priv;
    s->chstats = av_calloc(sizeof(*s->chstats), outlink->channels);
    if (!s->chstats)
        return AVERROR(ENOMEM);
    s->nb_channels = outlink->channels;
    s->mult = exp((-1 / s->time_constant / outlink->sample_rate));
    s->tc_samples = 5 * s->time_constant * outlink->sample_rate + .5;
    reset_stats(s);
    return 0;
}
