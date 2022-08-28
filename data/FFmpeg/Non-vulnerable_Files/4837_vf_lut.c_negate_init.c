AVFILTER_DEFINE_CLASS(negate);
static av_cold int negate_init(AVFilterContext *ctx)
{
    LutContext *s = ctx->priv;
    int i;
    av_log(ctx, AV_LOG_DEBUG, "negate_alpha:%d\n", s->negate_alpha);
    for (i = 0; i < 4; i++) {
        s->comp_expr_str[i] = av_strdup((i == 3 && !s->negate_alpha) ?
                                          "val" : "negval");
        if (!s->comp_expr_str[i]) {
            uninit(ctx);
            return AVERROR(ENOMEM);
        }
    }
    return 0;
}
