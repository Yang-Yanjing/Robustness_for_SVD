static int parse_synth_channel_bell(struct sbg_parser *p,
                                    struct sbg_script_synth *synth)
{
    double carrierf;
    int carrier, vol;
    if (!lex_fixed(p, "bell", 4))
        return 0;
    if (!lex_double(p, &carrierf))
        return AVERROR_INVALIDDATA;
    FORWARD_ERROR(parse_volume(p, &vol));
    if (scale_double(p->log, carrierf, 1, &carrier) < 0)
        return AVERROR(EDOM);
    synth->type    = SBG_TYPE_BELL;
    synth->carrier = carrier;
    synth->vol     = vol;
    return 1;
}
