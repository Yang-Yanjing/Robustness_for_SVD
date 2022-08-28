static int check_field_queue(PullupContext *s)
{
    int ret;
    if (s->head->next == s->first) {
        PullupField *f = av_mallocz(sizeof(*f));
        if (!f)
            return AVERROR(ENOMEM);
        if ((ret = alloc_metrics(s, f)) < 0) {
            av_free(f);
            return ret;
        }
        f->prev        = s->head;
        f->next        = s->first;
        s->head->next  = f;
        s->first->prev = f;
    }
    return 0;
}
