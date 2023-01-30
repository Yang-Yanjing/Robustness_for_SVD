AVFILTER_DEFINE_CLASS(vignette);
static av_cold int init(AVFilterContext *ctx)
{
    VignetteContext *s = ctx->priv;
#define PARSE_EXPR(name) do {                                               \
    int ret = av_expr_parse(&s->name##_pexpr,  s->name##_expr, var_names,   \
                            NULL, NULL, NULL, NULL, 0, ctx);                \
    if (ret < 0) {                                                          \
        av_log(ctx, AV_LOG_ERROR, "Unable to parse expression for '"        \
               AV_STRINGIFY(name) "'\n");                                   \
        return ret;                                                         \
    }                                                                       \
} while (0)
    PARSE_EXPR(angle);
    PARSE_EXPR(x0);
    PARSE_EXPR(y0);
    return 0;
}
