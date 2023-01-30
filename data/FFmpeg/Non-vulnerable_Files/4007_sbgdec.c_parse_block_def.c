static int parse_block_def(struct sbg_parser *p,
                           struct sbg_script_definition *def)
{
    int r, tseq;
    lex_space(p);
    if (!lex_line_end(p))
        return AVERROR_INVALIDDATA;
    tseq = p->nb_block_tseq;
    while (1) {
        r = parse_time_sequence(p, 1);
        if (r < 0)
            return r;
        if (!r)
            break;
    }
    if (!lex_char(p, '}'))
