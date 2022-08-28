static int set_enable_expr(AVFilterContext *ctx, const char *expr)
{
    int ret;
    char *expr_dup;
    AVExpr *old = ctx->enable;
    if (!(ctx->filter->flags & AVFILTER_FLAG_SUPPORT_TIMELINE)) {
        av_log(ctx, AV_LOG_ERROR, "Timeline ('enable' option) not supported "
               "with filter '%s'\n", ctx->filter->name);
        return AVERROR_PATCHWELCOME;
    }
    expr_dup = av_strdup(expr);
    if (!expr_dup)
        return AVERROR(ENOMEM);
    if (!ctx->var_values) {
        ctx->var_values = av_calloc(VAR_VARS_NB, sizeof(*ctx->var_values));
        if (!ctx->var_values) {
            av_free(expr_dup);
            return AVERROR(ENOMEM);
        }
    }
    ret = av_expr_parse((AVExpr**)&ctx->enable, expr_dup, var_names,
                        NULL, NULL, NULL, NULL, 0, ctx->priv);
    if (ret < 0) {
        av_log(ctx->priv, AV_LOG_ERROR,
               "Error when evaluating the expression '%s' for enable\n",
               expr_dup);
        av_free(expr_dup);
        return ret;
    }
    av_expr_free(old);
    av_free(ctx->enable_str);
    ctx->enable_str = expr_dup;
    return 0;
}
