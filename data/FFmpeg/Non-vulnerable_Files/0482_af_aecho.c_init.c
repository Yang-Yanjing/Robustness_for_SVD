static av_cold int init(AVFilterContext *ctx)
{
    AudioEchoContext *s = ctx->priv;
    int nb_delays, nb_decays, i;
    if (!s->delays || !s->decays) {
        av_log(ctx, AV_LOG_ERROR, "Missing delays and/or decays.\n");
        return AVERROR(EINVAL);
    }
    count_items(s->delays, &nb_delays);
    count_items(s->decays, &nb_decays);
    s->delay = av_realloc_f(s->delay, nb_delays, sizeof(*s->delay));
    s->decay = av_realloc_f(s->decay, nb_decays, sizeof(*s->decay));
    if (!s->delay || !s->decay)
        return AVERROR(ENOMEM);
    fill_items(s->delays, &nb_delays, s->delay);
    fill_items(s->decays, &nb_decays, s->decay);
    if (nb_delays != nb_decays) {
        av_log(ctx, AV_LOG_ERROR, "Number of delays %d differs from number of decays %d.\n", nb_delays, nb_decays);
        return AVERROR(EINVAL);
    }
    s->nb_echoes = nb_delays;
    if (!s->nb_echoes) {
        av_log(ctx, AV_LOG_ERROR, "At least one decay & delay must be set.\n");
        return AVERROR(EINVAL);
    }
    s->samples = av_realloc_f(s->samples, nb_delays, sizeof(*s->samples));
    if (!s->samples)
        return AVERROR(ENOMEM);
    for (i = 0; i < nb_delays; i++) {
        if (s->delay[i] <= 0 || s->delay[i] > 90000) {
            av_log(ctx, AV_LOG_ERROR, "delay[%d]: %f is out of allowed range: (0, 90000]\n", i, s->delay[i]);
            return AVERROR(EINVAL);
        }
        if (s->decay[i] <= 0 || s->decay[i] > 1) {
            av_log(ctx, AV_LOG_ERROR, "decay[%d]: %f is out of allowed range: (0, 1]\n", i, s->decay[i]);
            return AVERROR(EINVAL);
        }
    }
    s->next_pts = AV_NOPTS_VALUE;
    av_log(ctx, AV_LOG_DEBUG, "nb_echoes:%d\n", s->nb_echoes);
    return 0;
}
