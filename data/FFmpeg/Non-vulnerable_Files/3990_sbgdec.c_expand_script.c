static int expand_script(void *log, struct sbg_script *s)
{
    int i, r, nb_events_max = 0;
    expand_timestamps(log, s);
    for (i = 0; i < s->nb_tseq; i++) {
        r = expand_tseq(log, s, &nb_events_max, 0, &s->tseq[i]);
        if (r < 0)
            return r;
    }
    if (!s->nb_events) {
        av_log(log, AV_LOG_ERROR, "No events in script\n");
        return AVERROR_INVALIDDATA;
    }
    if (s->opt_end_at_last)
        s->end_ts = s->events[s->nb_events - 1].ts;
    return 0;
}
