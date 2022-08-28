static int lex_char(struct sbg_parser *p, char c)
{
    int r = p->cursor < p->end && *p->cursor == c;
    p->cursor += r;
    return r;
}
