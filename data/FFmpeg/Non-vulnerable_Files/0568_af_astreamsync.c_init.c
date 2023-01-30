AVFILTER_DEFINE_CLASS(astreamsync);
static av_cold int init(AVFilterContext *ctx)
{
    AStreamSyncContext *as = ctx->priv;
    int r, i;
    r = av_expr_parse(&as->expr, as->expr_str, var_names,
                      NULL, NULL, NULL, NULL, 0, ctx);
    if (r < 0) {
        av_log(ctx, AV_LOG_ERROR, "Error in expression \"%s\"\n", as->expr_str);
        return r;
    }
    for (i = 0; i < 42; i++)
        av_expr_eval(as->expr, as->var_values, NULL); 
    return 0;
}
