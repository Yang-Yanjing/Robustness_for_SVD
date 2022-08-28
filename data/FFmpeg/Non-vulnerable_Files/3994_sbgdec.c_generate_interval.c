static int generate_interval(void *log, struct sbg_script *s,
                             struct ws_intervals *inter,
                             int64_t ts1, int64_t ts2,
                             struct sbg_script_synth *s1,
                             struct sbg_script_synth *s2,
                             int transition)
{
    int r;
    if (ts2 <= ts1 || (s1->vol == 0 && s2->vol == 0))
        return 0;
    switch (s1->type) {
        case SBG_TYPE_NONE:
            break;
        case SBG_TYPE_SINE:
            if (s1->beat == 0 && s2->beat == 0) {
                r = add_interval(inter, WS_SINE, 3, s1->ref.l,
                                 ts1, s1->carrier, s1->vol,
                                 ts2, s2->carrier, s2->vol);
                if (r < 0)
                    return r;
                s2->ref.l = s2->ref.r = r;
            } else {
                r = add_interval(inter, WS_SINE, 1, s1->ref.l,
                                 ts1, s1->carrier + s1->beat / 2, s1->vol,
                                 ts2, s2->carrier + s2->beat / 2, s2->vol);
                if (r < 0)
                    return r;
                s2->ref.l = r;
                r = add_interval(inter, WS_SINE, 2, s1->ref.r,
                                 ts1, s1->carrier - s1->beat / 2, s1->vol,
                                 ts2, s2->carrier - s2->beat / 2, s2->vol);
                if (r < 0)
                    return r;
                s2->ref.r = r;
            }
            break;
        case SBG_TYPE_BELL:
            if (transition == 2) {
                r = add_bell(inter, s, ts1, ts2, s1->carrier, s2->vol);
                if (r < 0)
                    return r;
            }
            break;
        case SBG_TYPE_SPIN:
            av_log(log, AV_LOG_WARNING, "Spinning noise not implemented, "
                                        "using pink noise instead.\n");
            
        case SBG_TYPE_NOISE:
            









            r = add_interval(inter, WS_NOISE, 3, s1->ref.l,
                             ts1, 0, s1->vol - s1->vol / 4,
                             ts2, 0, s2->vol - s2->vol / 4);
            if (r < 0)
                return r;
            s2->ref.l = s2->ref.r = r;
            break;
        case SBG_TYPE_MIX:
            
        default:
            av_log(log, AV_LOG_ERROR,
                   "Type %d is not implemented\n", s1->type);
            return AVERROR_PATCHWELCOME;
    }
    return 0;
}
