static int lex_wsword(struct sbg_parser *p, struct sbg_string *rs)
{
    char *s = p->cursor, *c = s;
    if (s == p->end || *s == '\n')
        return 0;
    while (c < p->end && *c != '\n' && !is_space(*c))
        c++;
    rs->s = s;
    rs->e = p->cursor = c;
    lex_space(p);
    return 1;
}
