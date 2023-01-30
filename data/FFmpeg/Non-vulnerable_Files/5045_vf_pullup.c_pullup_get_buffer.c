static PullupBuffer *pullup_get_buffer(PullupContext *s, int parity)
{
    int i;
    
    if (parity < 2 && s->last && parity != s->last->parity
        && !s->last->buffer->lock[parity]) {
        alloc_buffer(s, s->last->buffer);
        return pullup_lock_buffer(s->last->buffer, parity);
    }
    
    for (i = 0; i < FF_ARRAY_ELEMS(s->buffers); i++) {
        if (s->buffers[i].lock[0])
            continue;
        if (s->buffers[i].lock[1])
            continue;
        alloc_buffer(s, &s->buffers[i]);
        return pullup_lock_buffer(&s->buffers[i], parity);
    }
    if (parity == 2)
        return 0;
    
    for (i = 0; i < FF_ARRAY_ELEMS(s->buffers); i++) {
        if (((parity + 1) & 1) && s->buffers[i].lock[0])
            continue;
        if (((parity + 1) & 2) && s->buffers[i].lock[1])
            continue;
        alloc_buffer(s, &s->buffers[i]);
        return pullup_lock_buffer(&s->buffers[i], parity);
    }
    return NULL;
}
