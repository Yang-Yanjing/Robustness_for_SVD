static int lex_line_end(struct sbg_parser *p)
{
    if (p->cursor < p->end && *p->cursor == '#') {
        p->cursor++;
        while (p->cursor < p->end && *p->cursor != '\n')
            p->cursor++;
    }
    if (p->cursor == p->end)
        
        return 1;
    if (*p->cursor != '\n')
        return 0;
    p->cursor++;
    p->line_no++;
    lex_space(p);
    return 1;
}
