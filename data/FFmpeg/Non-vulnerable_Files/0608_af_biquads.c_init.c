static av_cold int init(AVFilterContext *ctx)
{
    BiquadsContext *s = ctx->priv;
    if (s->filter_type != biquad) {
        if (s->frequency <= 0 || s->width <= 0) {
            av_log(ctx, AV_LOG_ERROR, "Invalid frequency %f and/or width %f <= 0\n",
                   s->frequency, s->width);
            return AVERROR(EINVAL);
        }
    }
    return 0;
}
