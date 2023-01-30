static int parse_synth_channel_spin(struct sbg_parser *p,
                                    struct sbg_script_synth *synth)
{
    double carrierf, beatf;
    int carrier, beat, vol;
    if (!lex_fixed(p, "spin:", 5))
        return 0;
    if (!lex_double(p, &carrierf))
        return AVERROR_INVALIDDATA;
    if (!lex_double(p, &beatf))
        return AVERROR_INVALIDDATA;
    FORWARD_ERROR(parse_volume(p, &vol));
    if (scale_double(p->log, carrierf, 1, &carrier) < 0 ||
        scale_double(p->log, beatf, 1, &beat) < 0)
        return AVERROR(EDOM);
    synth->type    = SBG_TYPE_SPIN;
    synth->carrier = carrier;
    synth->beat    = beat;
    synth->vol     = vol;
    return 1;
}
