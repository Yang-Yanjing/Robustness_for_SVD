static void queue_pop(FifoContext *s)
{
    Buf *tmp = s->root.next->next;
    if (s->last == s->root.next)
        s->last = &s->root;
    av_freep(&s->root.next);
    s->root.next = tmp;
}
