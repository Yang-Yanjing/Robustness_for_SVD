static av_cold int init(AVFilterContext *ctx)
{
    ChorusContext *s = ctx->priv;
    int nb_delays, nb_decays, nb_speeds, nb_depths;
    if (!s->delays_str || !s->decays_str || !s->speeds_str || !s->depths_str) {
        av_log(ctx, AV_LOG_ERROR, "Both delays & decays & speeds & depths must be set.\n");
        return AVERROR(EINVAL);
    }
    count_items(s->delays_str, &nb_delays);
    count_items(s->decays_str, &nb_decays);
    count_items(s->speeds_str, &nb_speeds);
    count_items(s->depths_str, &nb_depths);
    s->delays = av_realloc_f(s->delays, nb_delays, sizeof(*s->delays));
    s->decays = av_realloc_f(s->decays, nb_decays, sizeof(*s->decays));
    s->speeds = av_realloc_f(s->speeds, nb_speeds, sizeof(*s->speeds));
    s->depths = av_realloc_f(s->depths, nb_depths, sizeof(*s->depths));
    if (!s->delays || !s->decays || !s->speeds || !s->depths)
        return AVERROR(ENOMEM);
    fill_items(s->delays_str, &nb_delays, s->delays);
    fill_items(s->decays_str, &nb_decays, s->decays);
    fill_items(s->speeds_str, &nb_speeds, s->speeds);
    fill_items(s->depths_str, &nb_depths, s->depths);
    if (nb_delays != nb_decays && nb_delays != nb_speeds && nb_delays != nb_depths) {
        av_log(ctx, AV_LOG_ERROR, "Number of delays & decays & speeds & depths given must be same.\n");
        return AVERROR(EINVAL);
    }
    s->num_chorus = nb_delays;
    if (s->num_chorus < 1) {
        av_log(ctx, AV_LOG_ERROR, "At least one delay & decay & speed & depth must be set.\n");
        return AVERROR(EINVAL);
    }
    s->length = av_calloc(s->num_chorus, sizeof(*s->length));
    s->lookup_table = av_calloc(s->num_chorus, sizeof(*s->lookup_table));
    if (!s->length || !s->lookup_table)
        return AVERROR(ENOMEM);
    s->next_pts = AV_NOPTS_VALUE;
    return 0;
}
