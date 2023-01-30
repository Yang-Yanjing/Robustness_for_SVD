static int parse_synth_channel_sine(struct sbg_parser *p,
                                    struct sbg_script_synth *synth)
{
    double carrierf, beatf;
    int carrier, beat, vol;
    if (!lex_double(p, &carrierf))
        return 0;
    if (!lex_double(p, &beatf))
        beatf = 0;
    FORWARD_ERROR(parse_volume(p, &vol));
    if (scale_double(p->log, carrierf, 1, &carrier) < 0 ||
        scale_double(p->log, beatf, 1, &beat) < 0)
        return AVERROR(EDOM);
    synth->type    = SBG_TYPE_SINE;
    synth->carrier = carrier;
    synth->beat    = beat;
    synth->vol     = vol;
    return 1;
}
