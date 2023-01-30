static int parse_synth_def(struct sbg_parser *p,
                           struct sbg_script_definition *def)
{
    int r, synth;
    synth = p->scs.nb_synth;
    while (1) {
        r = parse_synth_channel(p);
        if (r < 0)
            return r;
        if (!r || !lex_space(p))
            break;
    }
    lex_space(p);
    if (synth == p->scs.nb_synth)
        return AVERROR_INVALIDDATA;
    if (!lex_line_end(p))
        return AVERROR_INVALIDDATA;
    def->type        = 'S';
    def->elements    = synth;
    def->nb_elements = p->scs.nb_synth - synth;
    return 1;
}
