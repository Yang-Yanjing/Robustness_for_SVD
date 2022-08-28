static void compute_affinity(PullupContext *s, PullupField *f)
{
    int i, max_l = 0, max_r = 0, l;
    if (f->flags & F_HAVE_AFFINITY)
        return;
    f->flags |= F_HAVE_AFFINITY;
    if (f->buffer == f->next->next->buffer) {
        f->affinity             =  1;
        f->next->affinity       =  0;
        f->next->next->affinity = -1;
        f->next->flags         |= F_HAVE_AFFINITY;
        f->next->next->flags   |= F_HAVE_AFFINITY;
        return;
    }
    for (i = 0; i < s->metric_length; i++) {
        int v  = f->vars[i];
        int lv = f->prev->vars[i];
        int rv = f->next->vars[i];
        int lc = f->      combs[i] - 2*(v < lv ? v : lv);
        int rc = f->next->combs[i] - 2*(v < rv ? v : rv);
        lc = FFMAX(lc, 0);
        rc = FFMAX(rc, 0);
        l  = lc - rc;
        if ( l > max_l)
            max_l =  l;
        if (-l > max_r)
            max_r = -l;
    }
    if (max_l + max_r < 64)
        return;
    if (max_r > 6 * max_l)
        f->affinity = -1;
    else if (max_l > 6 * max_r)
        f->affinity =  1;
}
