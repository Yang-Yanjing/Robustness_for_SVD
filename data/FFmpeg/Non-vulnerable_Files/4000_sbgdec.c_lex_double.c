static int lex_double(struct sbg_parser *p, double *r)
{
    double d;
    char *end;
    if (p->cursor == p->end || is_space(*p->cursor) || *p->cursor == '\n')
        return 0;
    d = strtod(p->cursor, &end);
    if (end > p->cursor) {
        *r = d;
        p->cursor = end;
        return 1;
    }
    return 0;
}
