static int parse_optarg(struct sbg_parser *p, char o, struct sbg_string *r)
{
    if (!lex_wsword(p, r)) {
        snprintf(p->err_msg, sizeof(p->err_msg),
                 "option '%c' requires an argument", o);
        return AVERROR_INVALIDDATA;
    }
    return 1;
}
