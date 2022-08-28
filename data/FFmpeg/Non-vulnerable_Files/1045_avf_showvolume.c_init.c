static av_cold int init(AVFilterContext *ctx)
{
    ShowVolumeContext *s = ctx->priv;
    int ret;
    if (s->color) {
        ret = av_expr_parse(&s->c_expr, s->color, var_names,
                            NULL, NULL, NULL, NULL, 0, ctx);
        if (ret < 0)
            return ret;
    }
    return 0;
}
