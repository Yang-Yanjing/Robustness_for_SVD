static int lex_fixed(struct sbg_parser *p, const char *t, int l)
{
    if (p->end - p->cursor < l || memcmp(p->cursor, t, l))
        return 0;
    p->cursor += l;
    return 1;
}
