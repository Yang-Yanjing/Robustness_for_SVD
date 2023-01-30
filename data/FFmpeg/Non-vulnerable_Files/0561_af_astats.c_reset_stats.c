static void reset_stats(AudioStatsContext *s)
{
    int c;
    memset(s->chstats, 0, sizeof(*s->chstats));
    for (c = 0; c < s->nb_channels; c++) {
        ChannelStats *p = &s->chstats[c];
        p->min = p->min_sigma_x2 = DBL_MAX;
        p->max = p->max_sigma_x2 = DBL_MIN;
        p->min_diff = p->max_diff = -1;
    }
}
