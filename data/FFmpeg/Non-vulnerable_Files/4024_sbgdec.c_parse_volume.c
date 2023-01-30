static int parse_volume(struct sbg_parser *p, int *vol)
{
    double v;
    if (!lex_char(p, '/'))
        return 0;
    if (!lex_double(p, &v))
        return AVERROR_INVALIDDATA;
    if (scale_double(p->log, v, 0.01, vol))
        return AVERROR(ERANGE);
    return 1;
}
