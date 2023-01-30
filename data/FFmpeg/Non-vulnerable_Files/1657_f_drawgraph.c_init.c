static av_cold int init(AVFilterContext *ctx)
{
    DrawGraphContext *s = ctx->priv;
    int ret, i;
    if (s->max <= s->min) {
        av_log(ctx, AV_LOG_ERROR, "max is same or lower than min\n");
        return AVERROR(EINVAL);
    }
    for (i = 0; i < 4; i++) {
        if (s->fg_str[i]) {
            ret = av_expr_parse(&s->fg_expr[i], s->fg_str[i], var_names,
                                NULL, NULL, NULL, NULL, 0, ctx);
            if (ret < 0)
                return ret;
        }
    }
    s->first = 1;
    return 0;
}
