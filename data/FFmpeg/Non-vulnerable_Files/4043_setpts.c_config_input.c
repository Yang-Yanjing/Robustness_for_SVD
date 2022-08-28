static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    SetPTSContext *setpts = ctx->priv;
    setpts->type = inlink->type;
    setpts->var_values[VAR_TB] = av_q2d(inlink->time_base);
    setpts->var_values[VAR_RTCSTART] = av_gettime();
    setpts->var_values[VAR_SR] =
    setpts->var_values[VAR_SAMPLE_RATE] =
        setpts->type == AVMEDIA_TYPE_AUDIO ? inlink->sample_rate : NAN;
    setpts->var_values[VAR_FRAME_RATE] = inlink->frame_rate.num && inlink->frame_rate.den ?
        av_q2d(inlink->frame_rate) : NAN;
    av_log(inlink->src, AV_LOG_VERBOSE, "TB:%f FRAME_RATE:%f SAMPLE_RATE:%f\n",
           setpts->var_values[VAR_TB],
           setpts->var_values[VAR_FRAME_RATE],
           setpts->var_values[VAR_SAMPLE_RATE]);
    return 0;
}
