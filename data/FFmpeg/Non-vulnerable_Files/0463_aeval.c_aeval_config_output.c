static int aeval_config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    EvalContext *eval = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    int ret;
    if (eval->same_chlayout) {
        eval->chlayout = inlink->channel_layout;
        if ((ret = parse_channel_expressions(ctx, inlink->channels)) < 0)
            return ret;
    }
    eval->n = 0;
    eval->nb_in_channels = eval->var_values[VAR_NB_IN_CHANNELS] = inlink->channels;
    eval->var_values[VAR_NB_OUT_CHANNELS] = outlink->channels;
    eval->var_values[VAR_S] = inlink->sample_rate;
    eval->var_values[VAR_T] = NAN;
    eval->channel_values = av_realloc_f(eval->channel_values,
                                        inlink->channels, sizeof(*eval->channel_values));
    if (!eval->channel_values)
        return AVERROR(ENOMEM);
    return 0;
}
