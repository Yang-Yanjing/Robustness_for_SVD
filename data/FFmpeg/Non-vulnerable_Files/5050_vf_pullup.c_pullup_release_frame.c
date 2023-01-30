static void pullup_release_frame(PullupFrame *f)
{
    int i;
    for (i = 0; i < f->length; i++)
        pullup_release_buffer(f->ifields[i], f->parity ^ (i & 1));
    pullup_release_buffer(f->ofields[0], 0);
    pullup_release_buffer(f->ofields[1], 1);
    if (f->buffer)
        pullup_release_buffer(f->buffer, 2);
    f->lock--;
}
