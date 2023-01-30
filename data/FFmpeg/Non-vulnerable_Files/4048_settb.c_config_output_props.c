static int config_output_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    SetTBContext *settb = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    AVRational time_base;
    int ret;
    double res;
    settb->var_values[VAR_AVTB] = av_q2d(AV_TIME_BASE_Q);
    settb->var_values[VAR_INTB] = av_q2d(inlink->time_base);
    settb->var_values[VAR_SR]   = inlink->sample_rate;
    outlink->w = inlink->w;
    outlink->h = inlink->h;
    if ((ret = av_expr_parse_and_eval(&res, settb->tb_expr, var_names, settb->var_values,
                                      NULL, NULL, NULL, NULL, NULL, 0, NULL)) < 0) {
        av_log(ctx, AV_LOG_ERROR, "Invalid expression '%s' for timebase.\n", settb->tb_expr);
        return ret;
    }
    time_base = av_d2q(res, INT_MAX);
    if (time_base.num <= 0 || time_base.den <= 0) {
        av_log(ctx, AV_LOG_ERROR,
               "Invalid non-positive values for the timebase num:%d or den:%d.\n",
               time_base.num, time_base.den);
        return AVERROR(EINVAL);
    }
    outlink->time_base = time_base;
    av_log(outlink->src, AV_LOG_VERBOSE, "tb:%d/%d -> tb:%d/%d\n",
           inlink ->time_base.num, inlink ->time_base.den,
           outlink->time_base.num, outlink->time_base.den);
    return 0;
}
