static av_cold int init(AVFilterContext *ctx)
{
#if FF_API_OLD_FILTER_OPTS
    AspectContext *s = ctx->priv;
    int ret;
    if (s->ratio_expr && s->aspect_den > 0) {
        double num;
        av_log(ctx, AV_LOG_WARNING,
               "num:den syntax is deprecated, please use num/den or named options instead\n");
        ret = av_expr_parse_and_eval(&num, s->ratio_expr, NULL, NULL,
                                     NULL, NULL, NULL, NULL, NULL, 0, ctx);
        if (ret < 0) {
            av_log(ctx, AV_LOG_ERROR, "Unable to parse ratio numerator \"%s\"\n", s->ratio_expr);
            return AVERROR(EINVAL);
        }
        s->sar = s->dar = av_d2q(num / s->aspect_den, s->max);
    }
#endif
    return 0;
}
