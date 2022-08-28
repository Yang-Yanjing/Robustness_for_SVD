static void compute_breaks(PullupContext *s, PullupField *f0)
{
    PullupField *f1 = f0->next;
    PullupField *f2 = f1->next;
    PullupField *f3 = f2->next;
    int i, l, max_l = 0, max_r = 0;
    if (f0->flags & F_HAVE_BREAKS)
        return;
    f0->flags |= F_HAVE_BREAKS;
    
    if (f0->buffer == f2->buffer && f1->buffer != f3->buffer) {
        f2->breaks |= BREAK_RIGHT;
        return;
    }
    if (f0->buffer != f2->buffer && f1->buffer == f3->buffer) {
        f1->breaks |= BREAK_LEFT;
        return;
    }
    for (i = 0; i < s->metric_length; i++) {
        l = f2->diffs[i] - f3->diffs[i];
        if ( l > max_l)
            max_l =  l;
        if (-l > max_r)
            max_r = -l;
    }
    
    if (max_l + max_r < 128)
        return;
    if (max_l > 4 * max_r)
        f1->breaks |= BREAK_LEFT;
    if (max_r > 4 * max_l)
        f2->breaks |= BREAK_RIGHT;
}
