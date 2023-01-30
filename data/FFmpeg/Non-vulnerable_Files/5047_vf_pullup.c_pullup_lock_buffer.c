static PullupBuffer *pullup_lock_buffer(PullupBuffer *b, int parity)
{
    if (!b)
        return NULL;
    if ((parity + 1) & 1)
        b->lock[0]++;
    if ((parity + 1) & 2)
        b->lock[1]++;
    return b;
}
