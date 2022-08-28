static int config_props(AVFilterLink *outlink)
{
    EvalContext *eval = outlink->src->priv;
    char buf[128];
    outlink->time_base = (AVRational){1, eval->sample_rate};
    outlink->sample_rate = eval->sample_rate;
    eval->var_values[VAR_S] = eval->sample_rate;
    eval->var_values[VAR_NB_IN_CHANNELS] = NAN;
    eval->var_values[VAR_NB_OUT_CHANNELS] = outlink->channels;
    av_get_channel_layout_string(buf, sizeof(buf), 0, eval->chlayout);
    av_log(outlink->src, AV_LOG_VERBOSE,
           "sample_rate:%d chlayout:%s duration:%"PRId64"\n",
           eval->sample_rate, buf, eval->duration);
    return 0;
}
