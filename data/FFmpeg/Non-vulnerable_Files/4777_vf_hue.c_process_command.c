static int process_command(AVFilterContext *ctx, const char *cmd, const char *args,
                           char *res, int res_len, int flags)
{
    HueContext *hue = ctx->priv;
    int ret;
#define SET_EXPR(expr, option)                                          \
    do {                                                                \
        ret = set_expr(&hue->expr##_pexpr, &hue->expr##_expr,           \
                       args, option, ctx);                              \
        if (ret < 0)                                                    \
            return ret;                                                 \
    } while (0)
    if (!strcmp(cmd, "h")) {
        SET_EXPR(hue_deg, "h");
        av_freep(&hue->hue_expr);
    } else if (!strcmp(cmd, "H")) {
        SET_EXPR(hue, "H");
        av_freep(&hue->hue_deg_expr);
    } else if (!strcmp(cmd, "s")) {
        SET_EXPR(saturation, "s");
    } else if (!strcmp(cmd, "b")) {
        SET_EXPR(brightness, "b");
    } else
        return AVERROR(ENOSYS);
    return 0;
}
