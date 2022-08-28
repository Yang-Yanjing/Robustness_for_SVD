static int alloc_buffer(PullupContext *s, PullupBuffer *b)
{
    int i;
    if (b->planes[0])
        return 0;
    for (i = 0; i < s->nb_planes; i++) {
        b->planes[i] = av_malloc(s->planeheight[i] * s->planewidth[i]);
    }
    if (s->nb_planes == 1)
        b->planes[1] = av_malloc(4*256);
    return 0;
}
