static void pullup_submit_field(PullupContext *s, PullupBuffer *b, int parity)
{
    PullupField *f;
    
    if (check_field_queue(s) < 0)
        return;
    
    if (s->last && s->last->parity == parity)
        return;
    f = s->head;
    f->parity   = parity;
    f->buffer   = pullup_lock_buffer(b, parity);
    f->flags    = 0;
    f->breaks   = 0;
    f->affinity = 0;
    compute_metric(s, f->diffs, f, parity, f->prev->prev, parity, s->diff);
    compute_metric(s, f->combs, parity ? f->prev : f, 0, parity ? f : f->prev, 1, s->comb);
    compute_metric(s, f->vars, f, parity, f, -1, s->var);
    emms_c();
    
    if (!s->first)
        s->first = s->head;
    s->last = s->head;
    s->head = s->head->next;
}
