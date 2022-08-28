static int parse_time_sequence(struct sbg_parser *p, int inblock)
{
    struct sbg_timestamp ts;
    int64_t rel_ts;
    int r;
    struct sbg_fade fade = { SBG_FADE_SAME, SBG_FADE_SAME, 0 };
    struct sbg_string name;
    struct sbg_script_tseq *tseq;
    r = parse_timestamp(p, &ts, &rel_ts);
    if (!r)
        return 0;
    if (r < 0)
        return r;
    if (ts.type) {
        if (inblock)
            return AVERROR_INVALIDDATA;
        p->current_time.type = ts.type;
        p->current_time.t    = ts.t;
    } else if(!inblock && !p->current_time.type) {
        snprintf(p->err_msg, sizeof(p->err_msg),
                 "relative time without previous absolute time");
        return AVERROR_INVALIDDATA;
    }
    ts.type = p->current_time.type;
    ts.t    = p->current_time.t + rel_ts;
    r = parse_fade(p, &fade);
    if (r < 0)
        return r;
    lex_space(p);
    if (!lex_name(p, &name))
        return AVERROR_INVALIDDATA;
    lex_space(p);
    if (lex_fixed(p, "->", 2)) {
        fade.slide = SBG_FADE_ADAPT;
        lex_space(p);
    }
    if (!lex_line_end(p))
        return AVERROR_INVALIDDATA;
    tseq = inblock ?
           alloc_array_elem((void **)&p->scs.block_tseq, sizeof(*tseq),
                            &p->nb_block_tseq, &p->nb_block_tseq_max) :
           alloc_array_elem((void **)&p->scs.tseq, sizeof(*tseq),
                            &p->scs.nb_tseq, &p->nb_tseq_max);
    if (!tseq)
        return AVERROR(ENOMEM);
    tseq->ts       = ts;
    tseq->name     = name.s;
    tseq->name_len = name.e - name.s;
    tseq->fade     = fade;
    return 1;
}
