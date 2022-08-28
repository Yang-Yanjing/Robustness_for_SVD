static void print_stats(AVFilterContext *ctx)
{
    AudioStatsContext *s = ctx->priv;
    uint64_t mask = 0, min_count = 0, max_count = 0, nb_samples = 0;
    double min_runs = 0, max_runs = 0,
           min = DBL_MAX, max = DBL_MIN, min_diff = DBL_MAX, max_diff = 0,
           max_sigma_x = 0,
           diff1_sum = 0,
           sigma_x = 0,
           sigma_x2 = 0,
           min_sigma_x2 = DBL_MAX,
           max_sigma_x2 = DBL_MIN;
    int c;
    for (c = 0; c < s->nb_channels; c++) {
        ChannelStats *p = &s->chstats[c];
        if (p->nb_samples < s->tc_samples)
            p->min_sigma_x2 = p->max_sigma_x2 = p->sigma_x2 / p->nb_samples;
        min = FFMIN(min, p->min);
        max = FFMAX(max, p->max);
        min_diff = FFMIN(min_diff, p->min_diff);
        max_diff = FFMAX(max_diff, p->max_diff);
        diff1_sum += p->diff1_sum,
        min_sigma_x2 = FFMIN(min_sigma_x2, p->min_sigma_x2);
        max_sigma_x2 = FFMAX(max_sigma_x2, p->max_sigma_x2);
        sigma_x += p->sigma_x;
        sigma_x2 += p->sigma_x2;
        min_count += p->min_count;
        max_count += p->max_count;
        min_runs += p->min_runs;
        max_runs += p->max_runs;
        mask |= p->mask;
        nb_samples += p->nb_samples;
        if (fabs(p->sigma_x) > fabs(max_sigma_x))
            max_sigma_x = p->sigma_x;
        av_log(ctx, AV_LOG_INFO, "Channel: %d\n", c + 1);
        av_log(ctx, AV_LOG_INFO, "DC offset: %f\n", p->sigma_x / p->nb_samples);
        av_log(ctx, AV_LOG_INFO, "Min level: %f\n", p->min);
        av_log(ctx, AV_LOG_INFO, "Max level: %f\n", p->max);
        av_log(ctx, AV_LOG_INFO, "Min difference: %f\n", p->min_diff);
        av_log(ctx, AV_LOG_INFO, "Max difference: %f\n", p->max_diff);
        av_log(ctx, AV_LOG_INFO, "Mean difference: %f\n", p->diff1_sum / (p->nb_samples - 1));
        av_log(ctx, AV_LOG_INFO, "Peak level dB: %f\n", LINEAR_TO_DB(FFMAX(-p->min, p->max)));
        av_log(ctx, AV_LOG_INFO, "RMS level dB: %f\n", LINEAR_TO_DB(sqrt(p->sigma_x2 / p->nb_samples)));
        av_log(ctx, AV_LOG_INFO, "RMS peak dB: %f\n", LINEAR_TO_DB(sqrt(p->max_sigma_x2)));
        if (p->min_sigma_x2 != 1)
            av_log(ctx, AV_LOG_INFO, "RMS trough dB: %f\n",LINEAR_TO_DB(sqrt(p->min_sigma_x2)));
        av_log(ctx, AV_LOG_INFO, "Crest factor: %f\n", p->sigma_x2 ? FFMAX(-p->min, p->max) / sqrt(p->sigma_x2 / p->nb_samples) : 1);
        av_log(ctx, AV_LOG_INFO, "Flat factor: %f\n", LINEAR_TO_DB((p->min_runs + p->max_runs) / (p->min_count + p->max_count)));
        av_log(ctx, AV_LOG_INFO, "Peak count: %"PRId64"\n", p->min_count + p->max_count);
        av_log(ctx, AV_LOG_INFO, "Bit depth: %u\n", bit_depth(p->mask));
    }
    av_log(ctx, AV_LOG_INFO, "Overall\n");
    av_log(ctx, AV_LOG_INFO, "DC offset: %f\n", max_sigma_x / (nb_samples / s->nb_channels));
    av_log(ctx, AV_LOG_INFO, "Min level: %f\n", min);
    av_log(ctx, AV_LOG_INFO, "Max level: %f\n", max);
    av_log(ctx, AV_LOG_INFO, "Min difference: %f\n", min_diff);
    av_log(ctx, AV_LOG_INFO, "Max difference: %f\n", max_diff);
    av_log(ctx, AV_LOG_INFO, "Mean difference: %f\n", diff1_sum / (nb_samples - s->nb_channels));
    av_log(ctx, AV_LOG_INFO, "Peak level dB: %f\n", LINEAR_TO_DB(FFMAX(-min, max)));
    av_log(ctx, AV_LOG_INFO, "RMS level dB: %f\n", LINEAR_TO_DB(sqrt(sigma_x2 / nb_samples)));
    av_log(ctx, AV_LOG_INFO, "RMS peak dB: %f\n", LINEAR_TO_DB(sqrt(max_sigma_x2)));
    if (min_sigma_x2 != 1)
        av_log(ctx, AV_LOG_INFO, "RMS trough dB: %f\n", LINEAR_TO_DB(sqrt(min_sigma_x2)));
    av_log(ctx, AV_LOG_INFO, "Flat factor: %f\n", LINEAR_TO_DB((min_runs + max_runs) / (min_count + max_count)));
    av_log(ctx, AV_LOG_INFO, "Peak count: %f\n", (min_count + max_count) / (double)s->nb_channels);
    av_log(ctx, AV_LOG_INFO, "Bit depth: %u\n", bit_depth(mask));
    av_log(ctx, AV_LOG_INFO, "Number of samples: %"PRId64"\n", nb_samples / s->nb_channels);
}
