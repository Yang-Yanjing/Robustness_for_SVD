int ff_free_filters(SwsContext *c)
{
    int i;
    if (c->desc) {
        for (i = 0; i < c->numDesc; ++i)
            av_freep(&c->desc[i].instance);
        av_freep(&c->desc);
    }
    if (c->slice) {
        for (i = 0; i < c->numSlice; ++i)
            free_slice(&c->slice[i]);
        av_freep(&c->slice);
    }
    return 0;
}
