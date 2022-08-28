static av_cold int init(AVFilterContext *ctx)
{
    SetPTSContext *setpts = ctx->priv;
    int ret;
    if ((ret = av_expr_parse(&setpts->expr, setpts->expr_str,
                             var_names, NULL, NULL, NULL, NULL, 0, ctx)) < 0) {
        av_log(ctx, AV_LOG_ERROR, "Error while parsing expression '%s'\n", setpts->expr_str);
        return ret;
    }
    setpts->var_values[VAR_N]           = 0.0;
    setpts->var_values[VAR_S]           = 0.0;
    setpts->var_values[VAR_PREV_INPTS]  = NAN;
    setpts->var_values[VAR_PREV_INT]    = NAN;
    setpts->var_values[VAR_PREV_OUTPTS] = NAN;
    setpts->var_values[VAR_PREV_OUTT]   = NAN;
    setpts->var_values[VAR_STARTPTS]    = NAN;
    setpts->var_values[VAR_STARTT]      = NAN;
    return 0;
}
