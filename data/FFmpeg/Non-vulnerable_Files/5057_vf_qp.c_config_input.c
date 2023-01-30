AVFILTER_DEFINE_CLASS(qp);
static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    QPContext *s = ctx->priv;
    int i;
    int ret;
    AVExpr *e = NULL;
    static const char *var_names[] = { "known", "qp", "x", "y", "w", "h", NULL };
    if (!s->qp_expr_str)
        return 0;
    ret = av_expr_parse(&e, s->qp_expr_str, var_names, NULL, NULL, NULL, NULL, 0, ctx);
    if (ret < 0)
        return ret;
    s->h       = (inlink->h + 15) >> 4;
    s->qstride = (inlink->w + 15) >> 4;
    for (i = -129; i < 128; i++) {
        double var_values[] = { i != -129, i, NAN, NAN, s->qstride, s->h, 0};
        double temp_val = av_expr_eval(e, var_values, NULL);
        if (isnan(temp_val)) {
            if(strchr(s->qp_expr_str, 'x') || strchr(s->qp_expr_str, 'y'))
                s->evaluate_per_mb = 1;
            else {
                av_expr_free(e);
                return AVERROR(EINVAL);
            }
        }
        s->lut[i + 129] = lrintf(temp_val);
    }
    av_expr_free(e);
    return 0;
}
