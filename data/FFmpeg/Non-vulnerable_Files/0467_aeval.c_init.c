static av_cold int init(AVFilterContext *ctx)
{
    EvalContext *eval = ctx->priv;
    int ret = 0;
    if (eval->chlayout_str) {
        if (!strcmp(eval->chlayout_str, "same") && !strcmp(ctx->filter->name, "aeval")) {
            eval->same_chlayout = 1;
        } else {
            ret = ff_parse_channel_layout(&eval->chlayout, NULL, eval->chlayout_str, ctx);
            if (ret < 0)
                return ret;
            ret = parse_channel_expressions(ctx, av_get_channel_layout_nb_channels(eval->chlayout));
            if (ret < 0)
                return ret;
        }
    } else {
        
        if ((ret = parse_channel_expressions(ctx, -1)) < 0)
            return ret;
        eval->chlayout = av_get_default_channel_layout(eval->nb_channels);
        if (!eval->chlayout && eval->nb_channels <= 0) {
            av_log(ctx, AV_LOG_ERROR, "Invalid number of channels '%d' provided\n",
                   eval->nb_channels);
            return AVERROR(EINVAL);
        }
    }
    if (eval->sample_rate_str)
        if ((ret = ff_parse_sample_rate(&eval->sample_rate, eval->sample_rate_str, ctx)))
            return ret;
    eval->n = 0;
    return ret;
}
