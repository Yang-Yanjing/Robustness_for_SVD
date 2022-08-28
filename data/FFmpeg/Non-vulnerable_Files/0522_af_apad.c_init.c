AVFILTER_DEFINE_CLASS(apad);
static av_cold int init(AVFilterContext *ctx)
{
    APadContext *s = ctx->priv;
    s->next_pts = AV_NOPTS_VALUE;
    if (s->whole_len >= 0 && s->pad_len >= 0) {
        av_log(ctx, AV_LOG_ERROR, "Both whole and pad length are set, this is not possible\n");
        return AVERROR(EINVAL);
    }
    s->pad_len_left   = s->pad_len;
    s->whole_len_left = s->whole_len;
    return 0;
}
