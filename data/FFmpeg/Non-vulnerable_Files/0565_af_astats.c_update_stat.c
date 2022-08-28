static inline void update_stat(AudioStatsContext *s, ChannelStats *p, double d)
{
    if (d < p->min) {
        p->min = d;
        p->min_run = 1;
        p->min_runs = 0;
        p->min_count = 1;
    } else if (d == p->min) {
        p->min_count++;
        p->min_run = d == p->last ? p->min_run + 1 : 1;
    } else if (p->last == p->min) {
        p->min_runs += p->min_run * p->min_run;
    }
    if (d > p->max) {
        p->max = d;
        p->max_run = 1;
        p->max_runs = 0;
        p->max_count = 1;
    } else if (d == p->max) {
        p->max_count++;
        p->max_run = d == p->last ? p->max_run + 1 : 1;
    } else if (p->last == p->max) {
        p->max_runs += p->max_run * p->max_run;
    }
    p->sigma_x += d;
    p->sigma_x2 += d * d;
    p->avg_sigma_x2 = p->avg_sigma_x2 * s->mult + (1.0 - s->mult) * d * d;
    p->min_diff = FFMIN(p->min_diff == -1 ? DBL_MAX : p->min_diff, FFABS(d - (p->min_diff == -1 ? DBL_MAX : p->last)));
    p->max_diff = FFMAX(p->max_diff, FFABS(d - (p->max_diff == -1 ? d : p->last)));
    p->diff1_sum += FFABS(d - p->last);
    p->last = d;
    p->mask |= llrint(d * (UINT64_C(1) << 63));
    if (p->nb_samples >= s->tc_samples) {
        p->max_sigma_x2 = FFMAX(p->max_sigma_x2, p->avg_sigma_x2);
        p->min_sigma_x2 = FFMIN(p->min_sigma_x2, p->avg_sigma_x2);
    }
    p->nb_samples++;
}
