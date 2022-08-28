static av_cold int init(AVFilterContext *ctx)
{
    ReverseContext *s = ctx->priv;
    s->pts = av_fast_realloc(NULL, &s->pts_size,
                             DEFAULT_LENGTH * sizeof(*(s->pts)));
    if (!s->pts)
        return AVERROR(ENOMEM);
    s->frames = av_fast_realloc(NULL, &s->frames_size,
                                DEFAULT_LENGTH * sizeof(*(s->frames)));
    if (!s->frames) {
        av_freep(&s->pts);
        return AVERROR(ENOMEM);
    }
    return 0;
}
