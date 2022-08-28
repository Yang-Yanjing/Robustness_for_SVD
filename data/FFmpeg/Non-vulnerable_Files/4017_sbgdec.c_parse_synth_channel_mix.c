static int parse_synth_channel_mix(struct sbg_parser *p,
                                   struct sbg_script_synth *synth)
{
    int vol;
    if (!lex_fixed(p, "mix", 3))
        return 0;
    FORWARD_ERROR(parse_volume(p, &vol));
    synth->type    = SBG_TYPE_MIX;
    synth->vol     = vol;
    return 1;
}
