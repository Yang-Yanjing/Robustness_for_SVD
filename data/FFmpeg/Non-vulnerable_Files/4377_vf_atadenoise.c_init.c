static av_cold int init(AVFilterContext *ctx)
{
    ATADenoiseContext *s = ctx->priv;
    if (!(s->size & 1)) {
        av_log(ctx, AV_LOG_ERROR, "size %d is invalid. Must be an odd value.\n", s->size);
        return AVERROR(EINVAL);
    }
    s->mid = s->size / 2 + 1;
    return 0;
}
