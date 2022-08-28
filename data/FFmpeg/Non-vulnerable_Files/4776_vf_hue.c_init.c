static av_cold int init(AVFilterContext *ctx)
{
    HueContext *hue = ctx->priv;
    int ret;
    if (hue->hue_expr && hue->hue_deg_expr) {
        av_log(ctx, AV_LOG_ERROR,
               "H and h options are incompatible and cannot be specified "
               "at the same time\n");
        return AVERROR(EINVAL);
    }
#define SET_EXPR(expr, option)                                          \
    if (hue->expr##_expr) do {                                          \
        ret = set_expr(&hue->expr##_pexpr, &hue->expr##_expr,           \
                       hue->expr##_expr, option, ctx);                  \
        if (ret < 0)                                                    \
            return ret;                                                 \
    } while (0)
    SET_EXPR(brightness, "b");
    SET_EXPR(saturation, "s");
    SET_EXPR(hue_deg,    "h");
    SET_EXPR(hue,        "H");
#undef SET_EXPR
    av_log(ctx, AV_LOG_VERBOSE,
           "H_expr:%s h_deg_expr:%s s_expr:%s b_expr:%s\n",
           hue->hue_expr, hue->hue_deg_expr, hue->saturation_expr, hue->brightness_expr);
    compute_sin_and_cos(hue);
    hue->is_first = 1;
    return 0;
}
