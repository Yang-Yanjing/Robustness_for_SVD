static int process_command(AVFilterContext *ctx, const char *cmd, const char *args,
                           char *res, int res_len, int flags)
{
    RotContext *rot = ctx->priv;
    int ret;
    if (!strcmp(cmd, "angle") || !strcmp(cmd, "a")) {
        AVExpr *old = rot->angle_expr;
        ret = av_expr_parse(&rot->angle_expr, args, var_names,
                            NULL, NULL, NULL, NULL, 0, ctx);
        if (ret < 0) {
            av_log(ctx, AV_LOG_ERROR,
                   "Error when parsing the expression '%s' for angle command\n", args);
            rot->angle_expr = old;
            return ret;
        }
        av_expr_free(old);
    } else
        ret = AVERROR(ENOSYS);
    return ret;
}
