static int lex_time(struct sbg_parser *p, int64_t *rt)
{
    int r = str_to_time(p->cursor, rt);
    p->cursor += r;
    return r > 0;
}
