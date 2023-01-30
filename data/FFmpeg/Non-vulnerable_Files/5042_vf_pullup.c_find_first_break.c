static int find_first_break(PullupField *f, int max)
{
    int i;
    for (i = 0; i < max; i++) {
        if (f->breaks & BREAK_RIGHT || f->next->breaks & BREAK_LEFT)
            return i + 1;
        f = f->next;
    }
    return 0;
}
