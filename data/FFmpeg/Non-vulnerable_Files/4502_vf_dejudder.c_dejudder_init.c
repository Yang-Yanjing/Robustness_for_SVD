static av_cold int dejudder_init(AVFilterContext *ctx)
{
    DejudderContext *s = ctx->priv;
    s->ringbuff = av_mallocz_array(s->cycle+2, sizeof(*s->ringbuff));
    if (!s->ringbuff)
        return AVERROR(ENOMEM);
    s->new_pts = 0;
    s->i1 = 0;
    s->i2 = 1;
    s->i3 = 2;
    s->i4 = 3;
    s->start_count = s->cycle + 2;
    return 0;
}
