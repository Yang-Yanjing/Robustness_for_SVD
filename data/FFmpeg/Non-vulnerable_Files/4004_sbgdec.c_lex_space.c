static int lex_space(struct sbg_parser *p)
{
    char *c = p->cursor;
    while (p->cursor < p->end && is_space(*p->cursor))
        p->cursor++;
    return p->cursor > c;
}
