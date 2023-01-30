static int generate_plateau(void *log, struct sbg_script *s,
                            struct ws_intervals *inter,
                            struct sbg_script_event *ev1)
{
    int64_t ts1 = ev1->ts_int, ts2 = ev1->ts_trans;
    int i, r;
    struct sbg_script_synth *s1;
    for (i = 0; i < ev1->nb_elements; i++) {
        s1 = &s->synth[ev1->elements + i];
        r = generate_interval(log, s, inter, ts1, ts2, s1, s1, 0);
        if (r < 0)
            return r;
    }
    return 0;
}
