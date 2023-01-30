static int parse_synth_channel_pink(struct sbg_parser *p,
                                    struct sbg_script_synth *synth)
{
    int vol;
    if (!lex_fixed(p, "pink", 4))
        return 0;
    FORWARD_ERROR(parse_volume(p, &vol));
    synth->type    = SBG_TYPE_NOISE;
    synth->vol     = vol;
    return 1;
}
