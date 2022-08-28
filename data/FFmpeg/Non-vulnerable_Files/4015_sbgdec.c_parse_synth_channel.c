static int parse_synth_channel(struct sbg_parser *p)
{
    int r;
    struct sbg_script_synth *synth;
    synth = alloc_array_elem((void **)&p->scs.synth, sizeof(*synth),
                             &p->scs.nb_synth, &p->nb_synth_max);
    if (!synth)
        return AVERROR(ENOMEM);
    r = lex_char(p, '-');
    if (!r)
        r = parse_synth_channel_pink(p, synth);
    if (!r)
        r = parse_synth_channel_bell(p, synth);
    if (!r)
        r = parse_synth_channel_mix(p, synth);
    if (!r)
        r = parse_synth_channel_spin(p, synth);
    
    if (!r)
        r = parse_synth_channel_sine(p, synth);
    if (r <= 0)
        p->scs.nb_synth--;
    return r;
}
