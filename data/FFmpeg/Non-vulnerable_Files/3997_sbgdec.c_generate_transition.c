static int generate_transition(void *log, struct sbg_script *s,
                               struct ws_intervals *inter,
                               struct sbg_script_event *ev1,
                               struct sbg_script_event *ev2)
{
    int64_t ts1 = ev1->ts_trans, ts2 = ev1->ts_next;
    
    int64_t tsmid = (ts1 >> 1) + (ts2 >> 1) + (ts1 & ts2 & 1);
    enum sbg_fade_type type = ev1->fade.slide | (ev1->fade.out & ev2->fade.in);
    int nb_elements = FFMAX(ev1->nb_elements, ev2->nb_elements);
    struct sbg_script_synth *s1, *s2, s1mod, s2mod, smid;
    int pass, i, r;
    for (pass = 0; pass < 2; pass++) {
        






        for (i = 0; i < nb_elements; i++) {
            s1 = i < ev1->nb_elements ? &s->synth[ev1->elements + i] : &s1mod;
            s2 = i < ev2->nb_elements ? &s->synth[ev2->elements + i] : &s2mod;
            s1mod = s1 != &s1mod ? *s1 : (struct sbg_script_synth){ 0 };
            s2mod = s2 != &s2mod ? *s2 : (struct sbg_script_synth){ 0 };
            if (ev1->fade.slide) {
                

                if (s1mod.type == SBG_TYPE_NONE) {
                    s1mod = s2mod;
                    s1mod.vol = 0;
                } else if (s2mod.type == SBG_TYPE_NONE) {
                    s2mod = s1mod;
                    s2mod.vol = 0;
                }
            }
            if (s1mod.type == s2mod.type &&
                s1mod.type != SBG_TYPE_BELL &&
                (type == SBG_FADE_ADAPT ||
                 (s1mod.carrier == s2mod.carrier &&
                  s1mod.beat == s2mod.beat))) {
                
                if (!pass) {
                    r = generate_interval(log, s, inter,
                                          ts1, ts2, &s1mod, &s2mod, 3);
                    if (r < 0)
                        return r;
                    s2->ref = s2mod.ref;
                }
            } else {
                
                if (!pass) {
                    smid = s1mod;
                    smid.vol = 0;
                    r = generate_interval(log, s, inter,
                                          ts1, tsmid, &s1mod, &smid, 1);
                    if (r < 0)
                        return r;
                } else {
                    smid = s2mod;
                    smid.vol = 0;
                    r = generate_interval(log, s, inter,
                                          tsmid, ts2, &smid, &s2mod, 2);
                    if (r < 0)
                        return r;
                    s2->ref = s2mod.ref;
                }
            }
        }
    }
    return 0;
}
