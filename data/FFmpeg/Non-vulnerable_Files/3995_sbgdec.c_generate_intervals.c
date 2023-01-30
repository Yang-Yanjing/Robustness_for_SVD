static int generate_intervals(void *log, struct sbg_script *s, int sample_rate,
                              struct ws_intervals *inter)
{
    int64_t trans_time = s->opt_fade_time / 2;
    struct sbg_script_event ev0, *ev1, *ev2;
    int64_t period;
    int i, r;
    


    period = s->events[s->nb_events - 1].ts - s->events[0].ts;
    period = (period + (DAY_TS - 1)) / DAY_TS * DAY_TS;
    period = FFMAX(period, DAY_TS);
    
    for (i = 0; i < s->nb_events; i++) {
        ev1 = &s->events[i];
        ev2 = &s->events[(i + 1) % s->nb_events];
        ev1->ts_int   = ev1->ts;
        ev1->ts_trans = ev1->fade.slide ? ev1->ts
                                        : ev2->ts + (ev1 < ev2 ? 0 : period);
    }
    for (i = 0; i < s->nb_events; i++) {
        ev1 = &s->events[i];
        ev2 = &s->events[(i + 1) % s->nb_events];
        if (!ev1->fade.slide) {
            ev1->ts_trans = FFMAX(ev1->ts_int,   ev1->ts_trans - trans_time);
            ev2->ts_int   = FFMIN(ev2->ts_trans, ev2->ts_int   + trans_time);
        }
        ev1->ts_next  = ev2->ts_int + (ev1 < ev2 ? 0 : period);
    }
    
    ev0 = s->events[s->nb_events - 1];
    ev0.ts_int   -= period;
    ev0.ts_trans -= period;
    ev0.ts_next  -= period;
    
    for (i = -1; i < s->nb_events; i++) {
        ev1 = i < 0 ? &ev0 : &s->events[i];
        ev1->ts_int   = av_rescale(ev1->ts_int,   sample_rate, AV_TIME_BASE);
        ev1->ts_trans = av_rescale(ev1->ts_trans, sample_rate, AV_TIME_BASE);
        ev1->ts_next  = av_rescale(ev1->ts_next,  sample_rate, AV_TIME_BASE);
    }
    
    for (i = 0; i < s->nb_synth; i++)
        s->synth[i].ref.l = s->synth[i].ref.r = -1;
    for (i = -1; i < s->nb_events; i++) {
        ev1 = i < 0 ? &ev0 : &s->events[i];
        ev2 = &s->events[(i + 1) % s->nb_events];
        r = generate_plateau(log, s, inter, ev1);
        if (r < 0)
            return r;
        r = generate_transition(log, s, inter, ev1, ev2);
        if (r < 0)
            return r;
    }
    if (!inter->nb_inter)
        av_log(log, AV_LOG_WARNING, "Completely silent script.\n");
    return 0;
}
