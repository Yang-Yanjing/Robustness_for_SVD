static int parse_timestamp(struct sbg_parser *p,
                               struct sbg_timestamp *rts, int64_t *rrel)
{
    int64_t abs = 0, rel = 0, dt;
    char type = 0;
    int r;
    if (lex_fixed(p, "NOW", 3)) {
        type = 'N';
        r = 1;
    } else {
        r = lex_time(p, &abs);
        if (r)
            type = 'T';
    }
    while (lex_char(p, '+')) {
        if (!lex_time(p, &dt))
            return AVERROR_INVALIDDATA;
        rel += dt;
        r = 1;
    }
    if (r) {
        if (!lex_space(p))
            return AVERROR_INVALIDDATA;
        rts->type = type;
        rts->t    = abs;
        *rrel     = rel;
    }
    return r;
}
