static av_cold int init(AVFilterContext *ctx)
{
    TrimContext *s = ctx->priv;
    s->first_pts = AV_NOPTS_VALUE;
    return 0;
}
