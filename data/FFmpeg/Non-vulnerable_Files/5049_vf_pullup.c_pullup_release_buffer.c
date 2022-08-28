static void pullup_release_buffer(PullupBuffer *b, int parity)
{
    if (!b)
        return;
    if ((parity + 1) & 1)
        b->lock[0]--;
    if ((parity + 1) & 2)
        b->lock[1]--;
}
